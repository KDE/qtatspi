/*
 * Copyright 2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "qspiadaptorv2.h"

#include <qaccessible.h>
#include <qapplication.h>
#include <qdbusmessage.h>
#include <qwidget.h>

#include <qdebug.h>

#include "generated/socket_proxy.h"

#include "constant_mappings.h"


QSpiAdaptorV2::QSpiAdaptorV2(DBusConnection *connection, QObject *parent)
    :QDBusVirtualObject(parent), m_dbus(connection)
{
}

QSpiAdaptorV2::~QSpiAdaptorV2()
{
}

QString QSpiAdaptorV2::introspect(const QString &path) const
{
    qWarning() << "QSpiAdaptorV2::introspect on " << path;
    return QString();
}


QPair<QAccessibleInterface*, int> QSpiAdaptorV2::interfaceFromPath(const QString& dbusPath)
{
    QAccessibleInterface* inter = 0;
    int index = 0;

    if (dbusPath == QSPI_OBJECT_PATH_ROOT) {
        inter = QAccessible::queryAccessibleInterface(qApp);
        return QPair<QAccessibleInterface*, int>(inter, index);
    }

    QStringList parts = dbusPath.split('/');

    if (parts.size() <= 5) {
        qWarning() << "invalid path: " << dbusPath;
        return QPair<QAccessibleInterface*, int>(0, 0);
    }

    QString objectString = parts.at(5);

    quintptr uintptr = objectString.toULongLong();

    if (!uintptr)
        return QPair<QAccessibleInterface*, int>(0, 0);

    QObject* object = reinterpret_cast<QObject*>(uintptr);
    inter = QAccessible::queryAccessibleInterface(object);
    QAccessibleInterface* childInter;

    for (int i = 6; i < parts.size(); ++i) {
        index = inter->navigate(QAccessible::Child, parts.at(i).toInt(), &childInter);
        if (index == 0) {
            delete inter;
            inter = childInter;
        }
    }

    return QPair<QAccessibleInterface*, int>(inter, index);
}

bool QSpiAdaptorV2::handleMessage(const QDBusMessage &message, const QDBusConnection &connection)
{
    // get accessible interface
    QPair<QAccessibleInterface*, int> accessible = interfaceFromPath(message.path());
    if (!accessible.first) {
        qWarning() << "Could not find accessible on path: " << message.path();
        return false;
    }

    QString interface = message.interface();
    QString function = message.member();

    // handle properties like regular functions
    if (interface == "org.freedesktop.DBus.Properties") {
        interface = message.arguments().at(0).toString();
        // Get/Set + Name
        function = message.member() + message.arguments().at(1).toString();
    }

    // switch interface to call
    if (interface == QSPI_INTERFACE_ACCESSIBLE) {
        return accessibleInterface(accessible.first, accessible.second, function, message, connection);
    } else if (interface == QSPI_INTERFACE_APPLICATION) {
        return applicationInterface(accessible.first, accessible.second, function, message, connection);
    } else if (interface == QSPI_INTERFACE_COMPONENT) {
        return componentInterface(accessible.first, accessible.second, function, message, connection);
    } else {
        qDebug() << "QSpiAdaptorV2::handleMessage " << message.path() << interface << function;
    }
    return false;
}

// Application

bool QSpiAdaptorV2::applicationInterface(QAccessibleInterface *, int, const QString &function, const QDBusMessage &message, const QDBusConnection &connection)
{

        if (function == "SetId") {
            Q_ASSERT(message.signature() == "ssv");
            QVariant value = qvariant_cast<QDBusVariant>(message.arguments().at(2)).variant();

            m_applicationId = value.toInt();
            return true;

        } else if (function == "GetId") {
            Q_ASSERT(message.signature() == "ss");
            QDBusMessage reply = message.createReply(QVariant::fromValue(QDBusVariant(m_applicationId)));
            return connection.send(reply);
        }

        return false;
}


void QSpiAdaptorV2::registerApplication()
{
    SocketProxy *registry;
    registry = new SocketProxy(QSPI_REGISTRY_NAME,
                               QSPI_OBJECT_PATH_ROOT, m_dbus->connection());

    QDBusPendingReply<QSpiObjectReference> reply;
    QSpiObjectReference ref = QSpiObjectReference(m_dbus->connection(), QDBusObjectPath(QSPI_OBJECT_PATH_ROOT));
    reply = registry->Embed(ref);
    reply.waitForFinished();
    if (reply.isValid ()) {
        const QSpiObjectReference &socket = reply.value();
        accessibilityRegistry = QSpiObjectReference(socket);
    } else {
        qWarning() << "Error in contacting registry";
        qWarning() << reply.error().name();
        qWarning() << reply.error().message();
    }
    delete registry;
}

// Accessible

bool QSpiAdaptorV2::accessibleInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection)
{
    Q_ASSERT(child >= 0);

    if (function == "GetRole") {
        sendReply(connection, message, (uint) qSpiRoleMapping[interface->role(child)].spiRole());
        return true;
    } else if (function == "GetName") {
        sendReply(connection, message, QVariant::fromValue(QDBusVariant(interface->text(QAccessible::Name, child))));
        return true;
    } else if (function == "GetRoleName") {
        sendReply(connection, message, qSpiRoleMapping[interface->role(child)].name());
        return true;
    } else if (function == "GetLocalizedRoleName") {
        sendReply(connection, message, QVariant::fromValue(qSpiRoleMapping[interface->role(child)].localizedName()));
        return true;
    } else if (function == "GetChildCount") {
        int childCount = child ? 0 : interface->childCount();
        sendReply(connection, message, QVariant::fromValue(QDBusVariant(childCount)));
        return true;
    } else if (function == "GetIndexInParent") {
        int childIndex = -1;
        if (child) {
            childIndex = child;
        } else {
            QAccessibleInterface *parent = accessibleParent(interface, child);
            if (parent)
                childIndex = parent->indexOfChild(interface);
            delete parent;
        }
        sendReply(connection, message, childIndex);
        return true;
    } else if (function == "GetParent") {
        QString path;
        QAccessibleInterface *parent = accessibleParent(interface, child);
        if (!parent || parent->role(0) == QAccessible::Application) {
            path = QSPI_OBJECT_PATH_ROOT;
        } else {
            path = pathForInterface(parent, 0);
        }
        if (parent != interface)
            delete parent;
        sendReply(connection, message, QVariant::fromValue(
                      QSpiObjectReference(connection, QDBusObjectPath(path))));
        return true;
    } else if (function == "GetChildAtIndex") {
        Q_ASSERT(child == 0); // Don't support child of virtual child
        int index = message.arguments().first().toInt() + 1;
        sendReply(connection, message, QVariant::fromValue(
                      QSpiObjectReference(connection, QDBusObjectPath(pathForInterface(interface, index)))));
        return true;
    } else if (function == "GetInterfaces") {
        sendReply(connection, message, accessibleInterfaces(interface, child));
        return true;
    } else if (function == "GetDescription") {
        sendReply(connection, message, QVariant::fromValue(QDBusVariant(interface->text(QAccessible::Description, child))));
        return true;
    } else if (function == "GetState") {
        quint64 spiState = spiStatesFromQState(interface->state(child));
        if (interface->table2Interface()) {
            setSpiStateBit(&spiState, ATSPI_STATE_MANAGES_DESCENDANTS);
        }
        sendReply(connection, message,
                  QVariant::fromValue(spiStateSetFromSpiStates(spiState)));
        return true;
    } else if (function == "GetAttributes") {
        sendReply(connection, message, QVariant::fromValue(QSpiAttributeSet()));
        return true;
    } else if (function == "GetRelationSet") {
        sendReply(connection, message, QVariant::fromValue(relationSet(interface, child, connection)));
        return true;
    } else if (function == "GetApplication") {
        sendReply(connection, message, QVariant::fromValue(
                      QSpiObjectReference(connection, QDBusObjectPath(QSPI_OBJECT_PATH_ROOT))));
        return true;
    } else if (function == "GetChildren") {
        qWarning() << "IMPLEMENT GETCHILDREN";
        //    QSpiObjectReferenceArray ();
        return false;
    } else {
        qWarning() << "WARNING: QSpiAdaptorV2::handleMessage does not implement " << function << message.path();
    }
    return false;
}

QStringList QSpiAdaptorV2::accessibleInterfaces(QAccessibleInterface *interface, int index) const
{
    QStringList ifaces;
#ifdef ACCESSIBLE_CREATION_DEBUG
    qDebug() << "ACCESSIBLE: " << interface->object();
#endif
    ifaces << QSPI_INTERFACE_ACCESSIBLE;

    if (    (!interface->rect(index).isEmpty()) ||
            (interface->object() && interface->object()->isWidgetType()) ||
            (interface->role(index) == QAccessible::ListItem) ||
            (interface->role(index) == QAccessible::Cell) ||
            (interface->role(index) == QAccessible::TreeItem) ||
            (interface->role(index) == QAccessible::Row) ||
            (interface->object() && interface->object()->inherits("QSGItem"))
            ) {
        ifaces << QSPI_INTERFACE_COMPONENT;
        }
#ifdef ACCESSIBLE_CREATION_DEBUG
    else {
        qDebug() << " IS NOT a component";
    }
#endif

    if (!index) {
        if (interface->actionInterface())
            ifaces << QSPI_INTERFACE_ACTION;

        if (interface->textInterface()) {
            ifaces << QSPI_INTERFACE_TEXT;
            // Cache the last text?
            // oldText = interface->textInterface()->text(0, interface->textInterface()->characterCount());
        }

        if (interface->editableTextInterface())
            ifaces << QSPI_INTERFACE_EDITABLE_TEXT;

        if (interface->valueInterface())
            ifaces << QSPI_INTERFACE_VALUE;

        if (interface->table2Interface())
            ifaces << QSPI_INTERFACE_TABLE;
    }

    // Do we need to cache the state?
    //    state = interface->state(childIndex());

    return ifaces;
}

QSpiRelationArray QSpiAdaptorV2::relationSet(QAccessibleInterface *interface, int child, const QDBusConnection &connection) const
{
    QSpiRelationArray relations;
    if (child == 0) {
        qDebug() << "QSpiAdaptorV2::relationSet currently has a problem with child ids.";
        // FIXME for example trees need to express their child relations here.
        return relations;
    }

    const QAccessible::RelationFlag relationsToCheck[] = {QAccessible::Label, QAccessible::Labelled, QAccessible::Controller, QAccessible::Controlled, static_cast<QAccessible::RelationFlag>(-1)};
    const AtspiRelationType relationTypes[] = {ATSPI_RELATION_LABELLED_BY, ATSPI_RELATION_LABEL_FOR, ATSPI_RELATION_CONTROLLED_BY, ATSPI_RELATION_CONTROLLER_FOR};

    for (int i = 0; relationsToCheck[i] >= 0; i++) {
        QList<QSpiObjectReference> related;
        int navigateResult = 1;

        for (int j = 1; navigateResult >= 0; j++) {
            QAccessibleInterface *target;
            navigateResult = interface->navigate(relationsToCheck[i], j, &target);

            if (navigateResult >= 0) {
                QDBusObjectPath path = QDBusObjectPath(pathForInterface(target ? target : interface, navigateResult));
                related.append(QSpiObjectReference(connection, path));
                delete target;
            }
        }
        if (!related.isEmpty())
            relations.append(QSpiRelationArrayEntry(relationTypes[i], related));
    }
    return relations;
}

void QSpiAdaptorV2::sendReply(const QDBusConnection &connection, const QDBusMessage &message, const QVariant &argument)
{
    QDBusMessage reply = message.createReply(argument);
    connection.send(reply);
}

QAccessibleInterface *QSpiAdaptorV2::accessibleParent(QAccessibleInterface *iface, int child)
{
    if (child)
        return iface;
    QAccessibleInterface *parent = 0;
    iface->navigate(QAccessible::Ancestor, 1, &parent);
    return parent;
}

QString QSpiAdaptorV2::pathForObject(QObject *object)
{
    Q_ASSERT(object);
    return QSPI_OBJECT_PATH_PREFIX + QString::number(reinterpret_cast<size_t>(object));
}

QString QSpiAdaptorV2::pathForInterface(QAccessibleInterface *interface, int childIndex)
{
    // Try to navigate to the child. If we get a proper interface, use it since it might have an object associated.
    QAccessibleInterface* childInterface = 0;
    if (childIndex) {
        int ret = interface->navigate(QAccessible::Child, childIndex, &childInterface);
        if (ret == 0 && childInterface) {
            interface = childInterface;
            childIndex = 0;
        }
    }
    QString path;
    QAccessibleInterface* interfaceWithObject = interface;
    while(!interfaceWithObject->object()) {
        QAccessibleInterface* parentInterface;
        interfaceWithObject->navigate(QAccessible::Ancestor, 1, &parentInterface);
        Q_ASSERT(parentInterface->isValid());
        int index = parentInterface->indexOfChild(interfaceWithObject);
        //Q_ASSERT(index >= 0);
        // FIXME: This should never happen!
        if (index < 0) {

            index = 999;
            path.prepend("/BROKEN_OBJECT_HIERARCHY");
            qWarning() << "Object claims to have child that we cannot navigate to. FIX IT!" << parentInterface->object();

            qDebug() << "Original interface: " << interface->object() << index;
            qDebug() << "Parent interface: " << parentInterface->object() << " childcount:" << parentInterface->childCount();
            QObject* p = parentInterface->object();
            qDebug() << p->children();

            QAccessibleInterface* tttt;
            int id = parentInterface->navigate(QAccessible::Child, 1, &tttt);
            qDebug() << "Nav child: " << id << tttt->object();
        }
        path.prepend('/' + QString::number(index));
        interfaceWithObject = parentInterface;
    }
    path.prepend(QSPI_OBJECT_PATH_PREFIX + QString::number(reinterpret_cast<quintptr>(interfaceWithObject->object())));

    if (childIndex > 0) {
        path.append('/' + QString::number(childIndex));
    }
    delete childInterface;
    return path;
}

// Component

static QAccessibleInterface *getWindow(QAccessibleInterface* interface)
{
    QAccessibleInterface *current=NULL, *tmp=NULL;

    interface->navigate(QAccessible::Ancestor, 0, &current);

    while (current &&
           current->role(0) != QAccessible::Window &&
           current->role(0) != QAccessible::Application)
    {
        tmp = NULL;
        current->navigate (QAccessible::Ancestor, 1, &tmp);
        current = tmp;
    }

    if (current) {
        return current;
    } else {
        return NULL;
    }
}

static QRect getRelativeRect(QAccessibleInterface* interface, int child)
{
    QAccessibleInterface *window;
    QRect wr, cr;

    cr = interface->rect(child);

    window = getWindow(interface);
    if (window)
    {
        wr = window->rect(child);

        cr.setX(cr.x() - wr.x());
        cr.setY(cr.x() - wr.y());
    }
    return cr;
}

bool QSpiAdaptorV2::componentInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection)
{
    Q_ASSERT(child >= 0);

    if (function == "Contains") {
        bool ret = false;
        int x = message.arguments().at(0).toInt();
        int y = message.arguments().at(1).toInt();
        uint coordType = message.arguments().at(2).toUInt();
        if (coordType == ATSPI_COORD_TYPE_SCREEN)
            ret = interface->rect(child).contains(x, y);
        else
            ret = getRelativeRect(interface, child).contains(x, y);
        sendReply(connection, message, ret);
    } else if (function == "GetAccessibleAtPoint") {
        int x = message.arguments().at(0).toInt();
        int y = message.arguments().at(1).toInt();
        uint coordType = message.arguments().at(2).toUInt();
        Q_UNUSED (coordType) // FIXME

        // Grab the top level widget. For complex widgets we want to return a child
        // at the right position instead.
        QWidget* w = qApp->widgetAt(x, y);
        if (w) {
            QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(w);
            if (!iface) {
                return false;
            }
            int childIndex = iface->childAt(x, y);
            QString path = pathForInterface(iface, childIndex);
            sendReply(connection, message, QVariant::fromValue(
                          QSpiObjectReference(connection, QDBusObjectPath(path))));
        } else {
            sendReply(connection, message, QVariant::fromValue(
                          QSpiObjectReference(connection, QDBusObjectPath(QSPI_OBJECT_PATH_NULL))));
        }
    } else if (function == "GetAlpha") {
        sendReply(connection, message, (double) 1.0);
    } else if (function == "GetExtents") {
        uint coordType = message.arguments().at(0).toUInt();
        sendReply(connection, message, QVariant::fromValue(getExtents(interface, child, coordType)));
    } else if (function == "GetLayer") {
        sendReply(connection, message, QVariant::fromValue((uint)1));
    } else if (function == "GetMDIZOrder") {
        sendReply(connection, message, QVariant::fromValue((short)0));
    } else if (function == "GetPosition") {
        uint coordType = message.arguments().at(0).toUInt();
        QRect rect;
        if (coordType == ATSPI_COORD_TYPE_SCREEN)
            rect = interface->rect(child);
        else
            rect = getRelativeRect(interface, child);
        QVariantList pos;
        pos << rect.x() << rect.y();
        connection.send(message.createReply(pos));
    } else if (function == "GetSize") {
        QRect rect = interface->rect(child);
        QVariantList size;
        size << rect.width() << rect.height();
        connection.send(message.createReply(size));
    } else if (function == "GrabFocus") {
        if (interface->object() && interface->object()->isWidgetType()) {
            QWidget* w = static_cast<QWidget*>(interface->object());
            w->setFocus(Qt::OtherFocusReason);
            sendReply(connection, message, true);
        }
        sendReply(connection, message, false);
    } else if (function == "SetExtents") {
//        int x = message.arguments().at(0).toInt();
//        int y = message.arguments().at(1).toInt();
//        int width = message.arguments().at(2).toInt();
//        int height = message.arguments().at(3).toInt();
//        uint coordinateType = message.arguments().at(4).toUInt();
        qWarning() << "SetExtents is not implemented.";
        sendReply(connection, message, false);
    } else if (function == "SetPosition") {
//        int x = message.arguments().at(0).toInt();
//        int y = message.arguments().at(1).toInt();
//        uint coordinateType = message.arguments().at(2).toUInt();
        qWarning() << "SetPosition is not implemented.";
        sendReply(connection, message, false);
    } else if (function == "SetSize") {
//        int width = message.arguments().at(0).toInt();
//        int height = message.arguments().at(1).toInt();
        qWarning() << "SetSize is not implemented.";
        sendReply(connection, message, false);
    } else {
        qWarning() << "WARNING: QSpiAdaptorV2::handleMessage does not implement " << function << message.path();
        return false;
    }
    return true;
}

QSpiRect QSpiAdaptorV2::getExtents(QAccessibleInterface *interface, int child, uint coordType)
{
    QSpiRect val;
    QRect rect;
    if (coordType == 0)
        rect = interface->rect(child);
    else
        rect = getRelativeRect(interface, child);

    val.x = rect.x();
    val.y = rect.y();
    val.width = rect.width();
    val.height = rect.height();
    return val;
}
