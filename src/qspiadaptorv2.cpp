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
#include <qaccessible2.h>
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

void QSpiAdaptorV2::windowActivated(QObject* window)
{
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(window);
    QString windowTitle = iface->text(QAccessible::Name, 0);
    delete iface;

    QDBusVariant data;
    data.setVariant(windowTitle);
//    emit Create("", 0, 0, data, spiBridge->getRootReference());
//    emit Restore("", 0, 0, data, spiBridge->getRootReference());
//    emit Activate("", 0, 0, data, spiBridge->getRootReference());

    QString path = pathForObject(window);
    QString interface = "org.a11y.atspi.Event.Window";
    QString name = "Activate";

    QDBusMessage message = QDBusMessage::createSignal(path, interface, name);
    QVariantList arguments;
    arguments << QString() << 0 << 0 << QVariant::fromValue(data)
              << QVariant::fromValue(QSpiObjectReference(m_dbus->connection().baseService(), QDBusObjectPath(QSPI_OBJECT_PATH_ROOT)));
    message.setArguments(arguments);
    bool ret = m_dbus->connection().send(message);

    qDebug() << "window message sent: " << ret;
}

QPair<QAccessibleInterface*, int> QSpiAdaptorV2::interfaceFromPath(const QString& dbusPath) const
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
    } else if (interface == QSPI_INTERFACE_ACTION) {
        return actionInterface(accessible.first, accessible.second, function, message, connection);
    } else if (interface == QSPI_INTERFACE_TEXT) {
        return textInterface(accessible.first, accessible.second, function, message, connection);
    } else if (interface == QSPI_INTERFACE_EDITABLE_TEXT) {
        return editableTextInterface(accessible.first, accessible.second, function, message, connection);
    } else if (interface == QSPI_INTERFACE_VALUE) {
        return valueInterface(accessible.first, accessible.second, function, message, connection);
    } else if (interface == QSPI_INTERFACE_TABLE) {
        return tableInterface(accessible.first, accessible.second, function, message, connection);
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
    } else if (function == "GetName") {
        sendReply(connection, message, QVariant::fromValue(QDBusVariant(interface->text(QAccessible::Name, child))));
    } else if (function == "GetRoleName") {
        sendReply(connection, message, qSpiRoleMapping[interface->role(child)].name());
    } else if (function == "GetLocalizedRoleName") {
        sendReply(connection, message, QVariant::fromValue(qSpiRoleMapping[interface->role(child)].localizedName()));
    } else if (function == "GetChildCount") {
        int childCount = child ? 0 : interface->childCount();
        sendReply(connection, message, QVariant::fromValue(QDBusVariant(childCount)));
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

        // Parent is a property, so it needs to be wrapped inside an extra variant.
        sendReply(connection, message, QVariant::fromValue(
                      QDBusVariant(QVariant::fromValue(QSpiObjectReference(connection, QDBusObjectPath(path))))));
    } else if (function == "GetChildAtIndex") {
        Q_ASSERT(child == 0); // Don't support child of virtual child
        int index = message.arguments().first().toInt() + 1;
        sendReply(connection, message, QVariant::fromValue(
                      QSpiObjectReference(connection, QDBusObjectPath(pathForInterface(interface, index)))));
    } else if (function == "GetInterfaces") {
        sendReply(connection, message, accessibleInterfaces(interface, child));
    } else if (function == "GetDescription") {
        sendReply(connection, message, QVariant::fromValue(QDBusVariant(interface->text(QAccessible::Description, child))));
    } else if (function == "GetState") {
        quint64 spiState = spiStatesFromQState(interface->state(child));
        if (interface->table2Interface()) {
            setSpiStateBit(&spiState, ATSPI_STATE_MANAGES_DESCENDANTS);
        }
        sendReply(connection, message,
                  QVariant::fromValue(spiStateSetFromSpiStates(spiState)));
    } else if (function == "GetAttributes") {
        sendReply(connection, message, QVariant::fromValue(QSpiAttributeSet()));
    } else if (function == "GetRelationSet") {
        sendReply(connection, message, QVariant::fromValue(relationSet(interface, child, connection)));
    } else if (function == "GetApplication") {
        sendReply(connection, message, QVariant::fromValue(
                      QSpiObjectReference(connection, QDBusObjectPath(QSPI_OBJECT_PATH_ROOT))));
    } else if (function == "GetChildren") {
        QSpiObjectReferenceArray children;
        for (int i = 0; i < interface->childCount(); ++i) {
            QSpiObjectReference ref(connection, QDBusObjectPath(pathForInterface(interface, i + 1)));
            children << ref;
        }
        connection.send(message.createReply(QVariant::fromValue(children)));
    } else {
        qWarning() << "WARNING: QSpiAdaptorV2::handleMessage does not implement " << function << message.path();
        return false;
    }
    return true;
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

void QSpiAdaptorV2::sendReply(const QDBusConnection &connection, const QDBusMessage &message, const QVariant &argument) const
{
    QDBusMessage reply = message.createReply(argument);
    connection.send(reply);
}

QAccessibleInterface *QSpiAdaptorV2::accessibleParent(QAccessibleInterface *iface, int child) const
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
            if (childIndex < 0)
                childIndex = 0;
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

// Action interface
bool QSpiAdaptorV2::actionInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection)
{
    if (function == "GetNActions") {
        sendReply(connection, message, QVariant::fromValue(QDBusVariant(QVariant::fromValue(interface->actionInterface()->actionCount()))));
    } else if (function == "DoAction") {
        int index = message.arguments().at(0).toInt();
        interface->actionInterface()->doAction(index);
        sendReply(connection, message, true);
    } else if (function == "GetActions") {
        if (child) {
            qWarning() << "QSpiAdaptorV2::actionInterface: Requesting action interface for child";
            return false;
        }
        sendReply(connection, message, QVariant::fromValue(getActions(interface)));
    } else if (function == "GetName") {
        int index = message.arguments().at(0).toInt();
        sendReply(connection, message, interface->actionInterface()->name(index));
    } else if (function == "GetDescription") {
        int index = message.arguments().at(0).toInt();
        sendReply(connection, message, interface->actionInterface()->description(index));
    } else if (function == "GetKeyBinding") {
        int index = message.arguments().at(0).toInt();
        QStringList keyBindings;
        keyBindings = interface->actionInterface()->keyBindings(index);
        /* Might as well return the first key binding, what are the other options? */
        if (keyBindings.length() > 0)
            sendReply(connection, message, keyBindings.at(0));
        else
            sendReply(connection, message, QString());
    } else {
        qWarning() << "WARNING: QSpiAdaptorV2::handleMessage does not implement " << function << message.path();
        return false;
    }
    return true;
}

QSpiActionArray QSpiAdaptorV2::getActions(QAccessibleInterface *interface) const
{
    QSpiActionArray actions;
    for (int i = 0; i < interface->actionInterface()->actionCount(); i++)
    {
        QSpiAction action;
        QStringList keyBindings;

        action.name = interface->actionInterface()->name(i);
        action.description = interface->actionInterface()->description(i);

        keyBindings = interface->actionInterface()->keyBindings(i);

        if (keyBindings.length() > 0)
                action.keyBinding = keyBindings[0];
        else
                action.keyBinding = "";

        actions << action;
    }
    return actions;
}

// Text interface
bool QSpiAdaptorV2::textInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection)
{
    Q_ASSERT(child == 0); // We should never claim to have a text interface on a virtual child
    // properties
    if (function == "GetCaretOffset") {
        sendReply(connection, message, QVariant::fromValue(QDBusVariant(QVariant::fromValue(interface->textInterface()->cursorPosition()))));
    } else if (function == "GetCharacterCount") {
        sendReply(connection, message, QVariant::fromValue(QDBusVariant(QVariant::fromValue(interface->textInterface()->characterCount()))));

    // functions
    } else if (function == "AddSelection") {
        int startOffset = message.arguments().at(0).toInt();
        int endOffset = message.arguments().at(1).toInt();
        int lastSelection = interface->textInterface()->selectionCount();
        interface->textInterface()->setSelection(lastSelection, startOffset, endOffset);
        sendReply(connection, message, (interface->textInterface()->selectionCount() > lastSelection));
    } else if (function == "GetAttributeRun") {
        int offset = message.arguments().at(0).toInt();
        bool includeDefaults = message.arguments().at(1).toBool();
        Q_UNUSED(includeDefaults)
        connection.send(message.createReply(getAttributes(interface, offset, includeDefaults)));
    } else if (function == "GetAttributeValue") {
        int offset = message.arguments().at(0).toInt();
        QString attributeName = message.arguments().at(1).toString();
        connection.send(message.createReply(getAttributeValue(interface, offset, attributeName)));
    } else if (function == "GetAttributes") {
        int offset = message.arguments().at(0).toInt();
        connection.send(message.createReply(getAttributes(interface, offset, true)));
    } else if (function == "GetBoundedRanges") {
        int x = message.arguments().at(0).toInt();
        int y = message.arguments().at(1).toInt();
        int width = message.arguments().at(2).toInt();
        int height = message.arguments().at(3).toInt();
        uint coordType = message.arguments().at(4).toUInt();
        uint xClipType = message.arguments().at(5).toUInt();
        uint yClipType = message.arguments().at(6).toUInt();
        Q_UNUSED(x) Q_UNUSED (y) Q_UNUSED(width)
        Q_UNUSED(height) Q_UNUSED(coordType)
        Q_UNUSED(xClipType) Q_UNUSED(yClipType)
        qWarning("Not implemented: QSpiAdaptor::GetBoundedRanges");
        sendReply(connection, message, QVariant::fromValue(QSpiTextRangeList()));
    } else if (function == "GetCharacterAtOffset") {
        int offset = message.arguments().at(0).toInt();
        int start;
        int end;
        QString result = interface->textInterface()->textAtOffset(offset, QAccessible2::CharBoundary, &start, &end);
        sendReply(connection, message, (int) *(qPrintable (result)));
        Q_ASSERT(0); // FIXME wtf is this!!!???!!! at least test if it produces the desired result bah

    } else if (function == "GetCharacterExtents") {
        int offset = message.arguments().at(0).toInt();
        int coordType = message.arguments().at(1).toUInt();
        connection.send(message.createReply(getCharacterExtents(interface, offset, coordType)));
    } else if (function == "GetDefaultAttributeSet" || function == "GetDefaultAttributes") {
        // GetDefaultAttributes is deprecated in favour of GetDefaultAttributeSet.
        // Empty set seems reasonable. There is no default attribute set.
        sendReply(connection, message, QVariant::fromValue(QSpiAttributeSet()));
    } else if (function == "GetNSelections") {
        sendReply(connection, message, interface->textInterface()->selectionCount());
    } else if (function == "GetOffsetAtPoint") {
        int x = message.arguments().at(0).toInt();
        int y = message.arguments().at(1).toInt();
        uint coordType = message.arguments().at(2).toUInt();
        //            int (int x, int y, uint coordType);
        // FIXME: relative to screen and relative to parent is not the same
        int offset = interface->textInterface()->offsetAtPoint(QPoint (x, y), static_cast <QAccessible2::CoordinateType> (coordType));
        sendReply(connection, message, offset);
    } else if (function == "GetRangeExtents") {
        int startOffset = message.arguments().at(0).toInt();
        int endOffset = message.arguments().at(1).toInt();
        uint coordType = message.arguments().at(2).toUInt();
        connection.send(message.createReply(getRangeExtents(interface, startOffset, endOffset, coordType)));
    } else if (function == "GetSelection") {
        int selectionNum = message.arguments().at(0).toInt();
        int start, end;
        interface->textInterface()->selection(selectionNum, &start, &end);
            if (start < 0) {
            // FIXME: what is this for?
            end = interface->textInterface()->cursorPosition();
        }
        QVariantList sel;
        sel << start << end;
        connection.send(message.createReply(sel));
    } else if (function == "GetText") {
        int startOffset = message.arguments().at(0).toInt();
        int endOffset = message.arguments().at(1).toInt();
        if (endOffset == -1) // AT-SPI uses -1 to signal all characters
            endOffset = interface->textInterface()->characterCount();
        sendReply(connection, message, interface->textInterface()->text(startOffset, endOffset));
    } else if (function == "GetTextAfterOffset") {
        int offset = message.arguments().at(0).toInt();
        int type = message.arguments().at(1).toUInt();
        int startOffset, endOffset;
        QString text = interface->textInterface()->textAfterOffset(offset, qAccessibleBoundaryType(type), &startOffset, &endOffset);
        QVariantList ret;
        ret << text << startOffset << endOffset;
        connection.send(message.createReply(ret));
    } else if (function == "GetTextAtOffset") {
        int offset = message.arguments().at(0).toInt();
        int type = message.arguments().at(1).toUInt();
        int startOffset, endOffset;
        QString text = interface->textInterface()->textAtOffset(offset, qAccessibleBoundaryType(type), &startOffset, &endOffset);
        QVariantList ret;
        ret << text << startOffset << endOffset;
        connection.send(message.createReply(ret));
    } else if (function == "GetTextBeforeOffset") {
        int offset = message.arguments().at(0).toInt();
        int type = message.arguments().at(1).toUInt();
        int startOffset, endOffset;
        QString text = interface->textInterface()->textBeforeOffset(offset, qAccessibleBoundaryType(type), &startOffset, &endOffset);
        QVariantList ret;
        ret << text << startOffset << endOffset;
        connection.send(message.createReply(ret));
    } else if (function == "RemoveSelection") {
        int selectionNum = message.arguments().at(0).toInt();
        interface->textInterface()->removeSelection(selectionNum);
        sendReply(connection, message, true);
    } else if (function == "SetCaretOffset") {
        int offset = message.arguments().at(0).toInt();
        interface->textInterface()->setCursorPosition(offset);
        sendReply(connection, message, true);
    } else if (function == "SetSelection") {
        int selectionNum = message.arguments().at(0).toInt();
        int startOffset = message.arguments().at(1).toInt();
        int endOffset = message.arguments().at(2).toInt();
        interface->textInterface()->setSelection(selectionNum, startOffset, endOffset);
        sendReply(connection, message, true);
    } else {
        qWarning() << "WARNING: QSpiAdaptorV2::handleMessage does not implement " << function << message.path();
        return false;
    }
    return true;
}

QAccessible2::BoundaryType QSpiAdaptorV2::qAccessibleBoundaryType(int atspiTextBoundaryType) const
{
    switch (atspiTextBoundaryType) {
    case ATSPI_TEXT_BOUNDARY_CHAR:
        return QAccessible2::CharBoundary;
    case ATSPI_TEXT_BOUNDARY_WORD_START:
    case ATSPI_TEXT_BOUNDARY_WORD_END:
        return QAccessible2::WordBoundary;
    case ATSPI_TEXT_BOUNDARY_SENTENCE_START:
    case ATSPI_TEXT_BOUNDARY_SENTENCE_END:
        return QAccessible2::SentenceBoundary;
    case ATSPI_TEXT_BOUNDARY_LINE_START:
    case ATSPI_TEXT_BOUNDARY_LINE_END:
        return QAccessible2::LineBoundary;
    }
    Q_ASSERT_X(0, "", "Requested invalid boundary type.");
    return QAccessible2::CharBoundary;
}

// FIXME all attribute methods below should share code
QVariantList QSpiAdaptorV2::getAttributes(QAccessibleInterface *interface, int offset, bool includeDefaults) const
{
    Q_UNUSED(includeDefaults);

    QSpiAttributeSet set;
    int startOffset;
    int endOffset;

    QString joined = interface->textInterface()->attributes(offset, &startOffset, &endOffset);
    QStringList attributes = joined.split (';', QString::SkipEmptyParts, Qt::CaseSensitive);
    foreach (const QString &attr, attributes) {
        QStringList items;
        items = attr.split(':', QString::SkipEmptyParts, Qt::CaseSensitive);
        set[items[0]] = items[1];
    }

    QVariantList list;
    list << QVariant::fromValue(set) << startOffset << endOffset;

    return list;
}

QVariantList QSpiAdaptorV2::getAttributeValue(QAccessibleInterface *interface, int offset, const QString &attributeName) const
{
    QString     mapped;
    QString     joined;
    QStringList attributes;
    QSpiAttributeSet map;
    int startOffset;
    int endOffset;
    bool defined;

    joined = interface->textInterface()->attributes(offset, &startOffset, &endOffset);
    attributes = joined.split (';', QString::SkipEmptyParts, Qt::CaseSensitive);
    foreach (const QString& attr, attributes) {
        QStringList items;
        items = attr.split(':', QString::SkipEmptyParts, Qt::CaseSensitive);
        map[items[0]] = items[1];
    }
    mapped = map[attributeName];
    if (mapped == QString())
       defined = true;
    else
       defined = false;
    QVariantList list;
    list << mapped << startOffset << endOffset << defined;
    return list;
}

QVariantList QSpiAdaptorV2::getCharacterExtents(QAccessibleInterface *interface, int offset, uint coordType) const
{
    int x;
    int y;
    int width;
    int height;

    // QAccessible2 has RelativeToParent as a coordinate type instead of relative
    // to top-level window, which is an AT-SPI coordinate type.
    if (static_cast<QAccessible2::CoordinateType>(coordType) != QAccessible2::RelativeToScreen) {
        const QWidget *widget = qobject_cast<const QWidget*>(interface->object());
        if (!widget) {
            return QVariantList() << 0 << 0 << 0 << 0;
        }
        const QWidget *parent = widget->parentWidget();
        while (parent) {
            widget = parent;
            parent = widget->parentWidget();
        }
        x = -widget->x();
        y = -widget->y();
    } else {
        x = 0;
        y = 0;
    }

    QRect rect = interface->textInterface()->characterRect(offset, QAccessible2::RelativeToScreen);
    width = rect.width();
    height = rect.height();
    x += rect.x();
    y += rect.y();

    QVariantList list;
    list << x << y << width << height;
    return list;
}

QVariantList QSpiAdaptorV2::getRangeExtents(QAccessibleInterface *interface,
                                            int startOffset, int endOffset, uint coordType) const
{
    int x;
    int y;
    int width;
    int height;

    if (endOffset == -1)
        endOffset = interface->textInterface()->characterCount();

    if (endOffset <= startOffset) {
        return QVariantList() << 0 << 0 << 0 << 0;
    }

    int xOffset = 0, yOffset = 0;
    QAccessibleTextInterface *textInterface = interface->textInterface();

    // QAccessible2 has RelativeToParent as a coordinate type instead of relative
    // to top-level window, which is an AT-SPI coordinate type.
    if (static_cast<QAccessible2::CoordinateType>(coordType) != QAccessible2::RelativeToScreen) {
        const QWidget *widget = qobject_cast<const QWidget*>(interface->object());
        if (!widget) {
            return QVariantList() << 0 << 0 << 0 << 0;
        }
        const QWidget *parent = widget->parentWidget();
        while (parent) {
            widget = parent;
            parent = widget->parentWidget();
        }
        xOffset = -widget->x();
        yOffset = -widget->y();
    }

    int minX=INT_MAX, minY=INT_MAX, maxX=0, maxY=0;

    for (int i=startOffset; i<endOffset; i++) {
        QRect rect = textInterface->characterRect(i, QAccessible2::RelativeToScreen);
        if (rect.x() < minX) {
            minX = rect.x();
        }
        if (rect.y() < minY) {
            minY = rect.y();
        }
        if ((rect.x() + rect.width()) > maxX) {
            maxX = (rect.x() + rect.width());
        }
        if ((rect.y() + rect.height()) > maxY) {
            maxY = (rect.y() + rect.height());
        }
    }

    width = maxX - minX;
    height = maxY - minY;
    y = minY + yOffset;
    x = minX + xOffset;
    QVariantList list;
    list << x << y << width << height;
    return list;
}



// Editable Text interface
bool QSpiAdaptorV2::editableTextInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection)
{
    Q_ASSERT(child == 0); // We should never claim to have a text interface on a virtual child
    if (function == "CopyText") {
        int startPos = message.arguments().at(0).toInt();
        int endPos = message.arguments().at(1).toInt();
        interface->editableTextInterface()->copyText(startPos, endPos);
        connection.send(message.createReply(true));
    } else if (function == "CutText") {
        int startPos = message.arguments().at(0).toInt();
        int endPos = message.arguments().at(1).toInt();
        interface->editableTextInterface()->cutText(startPos, endPos);
        connection.send(message.createReply(true));
    } else if (function == "DeleteText") {
        int startPos = message.arguments().at(0).toInt();
        int endPos = message.arguments().at(1).toInt();
        interface->editableTextInterface()->deleteText(startPos, endPos);
        connection.send(message.createReply(true));
    } else if (function == "InsertText") {
        int position = message.arguments().at(0).toInt();
        QString text = message.arguments().at(1).toString();
        int length = message.arguments().at(2).toInt();
        QString resized (text);
        resized.resize(length);
        interface->editableTextInterface()->insertText(position, resized);
        connection.send(message.createReply(true));
    } else if (function == "PasteText") {
        int position = message.arguments().at(0).toInt();
        interface->editableTextInterface()->pasteText(position);
        connection.send(message.createReply(true));
    } else if (function == "SetTextContents") {
        QString newContents = message.arguments().at(0).toString();
        interface->editableTextInterface()->replaceText(0, interface->textInterface()->characterCount(), newContents);
        connection.send(message.createReply(true));
    } else if (function == "") {
        connection.send(message.createReply());
    } else {
        qWarning() << "WARNING: QSpiAdaptorV2::handleMessage does not implement " << function << message.path();
        return false;
    }
    return true;
}

// Value interface
bool QSpiAdaptorV2::valueInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection)
{
    Q_ASSERT(child == 0);
    if (0) {
    } else if (function == "SetCurrentValue") {
        double value = message.arguments().at(0).toInt();
        //Temporal fix
        //See https://bugzilla.gnome.org/show_bug.cgi?id=652596
        interface->valueInterface()->setCurrentValue(value);
        connection.send(message.createReply()); // FIXME is the reply needed?
    } else if (function == "GetCurrentValue") {
        bool success;
        double val = interface->valueInterface()->currentValue().toDouble(&success);
        if (!success) {
            qWarning ("QSpiAdaptorV2::valueInterface: Could not convert current value to double.");
        }
        connection.send(message.createReply(
                            QVariant::fromValue(QDBusVariant(QVariant::fromValue(val)))));
    } else if (function == "GetMaximumValue") {
        bool success;
        double val = interface->valueInterface()->maximumValue().toDouble(&success);
        if (!success) {
            qWarning ("QSpiAdaptorV2::valueInterface: Could not convert current value to double.");
        }
        connection.send(message.createReply(
                            QVariant::fromValue(QDBusVariant(QVariant::fromValue(val)))));
    } else if (function == "GetMinimumIncrement") {
        connection.send(message.createReply(
                            QVariant::fromValue(QDBusVariant(QVariant::fromValue(0.0)))));
    } else if (function == "GetMinimumValue") {
        bool success;
        double val = interface->valueInterface()->minimumValue().toDouble(&success);
        if (!success) {
            qWarning ("QSpiAdaptorV2::valueInterface: Could not convert current value to double.");
        }
        connection.send(message.createReply(
                            QVariant::fromValue(QDBusVariant(QVariant::fromValue(val)))));
    } else {
        qWarning() << "WARNING: QSpiAdaptorV2::handleMessage does not implement " << function << message.path();
        return false;
    }
    return true;
}

// Table interface
bool QSpiAdaptorV2::tableInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection)
{
    Q_ASSERT(child == 0);
    if (0) {
    // properties
    } else if (function == "GetCaption") {
        // fixme: leak of QAI
        QObject *object = interface->tableInterface()->caption()->object();
        if (!object) {
            connection.send(message.createReply(QVariant::fromValue(QDBusVariant(QVariant::fromValue(QSpiObjectReference())))));
        } else {
            QDBusObjectPath path(pathForObject(object));
            QSpiObjectReference ref(connection, path);
            connection.send(message.createReply(QVariant::fromValue(QDBusVariant(QVariant::fromValue(ref)))));
        }
    } else if (function == "GetNColumns") {
        connection.send(message.createReply(QVariant::fromValue(QDBusVariant(
            QVariant::fromValue(interface->table2Interface()->columnCount())))));
    } else if (function == "GetNRows") {
        connection.send(message.createReply(QVariant::fromValue(QDBusVariant(
            QVariant::fromValue(interface->table2Interface()->rowCount())))));
    } else if (function == "GetNSelectedColumns") {
        connection.send(message.createReply(QVariant::fromValue(QDBusVariant(
            QVariant::fromValue(interface->table2Interface()->selectedColumnCount())))));
    } else if (function == "GetNSelectedRows") {
        connection.send(message.createReply(QVariant::fromValue(QDBusVariant(
            QVariant::fromValue(interface->table2Interface()->selectedRowCount())))));
    } else if (function == "GetSummary") {
        // fixme: leak of QAI
        QObject *object = interface->tableInterface()->summary()->object();
        if (!object) {
            connection.send(message.createReply(QVariant::fromValue(QDBusVariant(QVariant::fromValue(QSpiObjectReference())))));
        } else {
            QDBusObjectPath path(pathForObject(object));
            QSpiObjectReference ref(connection, path);
            connection.send(message.createReply(QVariant::fromValue(QDBusVariant(QVariant::fromValue(ref)))));
        }

    // methods
    } else if (function == "GetAccessibleAt") {
        int row = message.arguments().at(0).toInt();
        int column = message.arguments().at(1).toInt();
        Q_ASSERT(interface->table2Interface());
        Q_ASSERT(row >= 0);
        Q_ASSERT(column >= 0);
        Q_ASSERT(row < interface->table2Interface()->rowCount());
        Q_ASSERT(column < interface->table2Interface()->columnCount());

        QSpiObjectReference ref;
        QAccessibleInterface* cell = interface->table2Interface()->cellAt(row, column);
        if (cell) {
            ref = QSpiObjectReference(connection, QDBusObjectPath(pathForInterface(cell, 0)));
            delete cell;
        } else {
            qWarning() << "WARNING: no cell interface returned for " << interface->object() << row << column;
            ref = QSpiObjectReference();
        }
        connection.send(message.createReply(QVariant::fromValue(ref)));

    } else if (function == "GetIndexAt") {
        int row = message.arguments().at(0).toInt();
        int column = message.arguments().at(1).toInt();
        QAccessibleInterface *cell = interface->table2Interface()->cellAt(row, column);
        int index = interface->indexOfChild(cell);
        qDebug() << "QSpiAdaptor::GetIndexAt" << row << column << index;
        Q_ASSERT(index > 0);
        delete cell;
        connection.send(message.createReply(index));

    } else if (function == "GetColumnAtIndex") {
        int index = message.arguments().at(0).toInt();
        int ret = 0;
        if (index > 1) {
            QAccessibleInterface *iface;
            interface->navigate(QAccessible::Child, index, &iface);
            if (iface) {
                qDebug() << "iface: " << iface->text(QAccessible::Name, 0);

                QAccessibleTable2CellInterface *cell = static_cast<QAccessibleTable2CellInterface*>(iface);
                ret = cell->columnIndex();
                delete cell;
            }
        }
        connection.send(message.createReply(ret));
    } else if (function == "GetRowAtIndex") {
        // FIXME merge with GetColumnAtIndex
        int index = message.arguments().at(0).toInt();
        int ret = 0;
        qDebug() << "QSpiAdaptor::GetRowAtIndex" << index;
        if (index > 1) {
            QAccessibleInterface *iface;
            interface->navigate(QAccessible::Child, index, &iface);
            if (iface) {
                qDebug() << "iface: " << iface->text(QAccessible::Name, 0);

                QAccessibleTable2CellInterface *cell = static_cast<QAccessibleTable2CellInterface*>(iface);
                ret = cell->rowIndex();
                delete cell;
            }
        }
        connection.send(message.createReply(ret));

    } else if (function == "GetColumnDescription") {
        int column = message.arguments().at(0).toInt();
        connection.send(message.createReply(interface->table2Interface()->columnDescription(column)));
    } else if (function == "GetRowDescription") {
        int row = message.arguments().at(0).toInt();
        connection.send(message.createReply(interface->table2Interface()->rowDescription(row)));



    } else if (function == "GetRowColumnExtentsAtIndex") {
        int index = message.arguments().at(0).toInt();
        bool success = false;

        int row, col, rowExtents, colExtents;
        bool isSelected;

        int cols = interface->table2Interface()->columnCount();
        row = index/cols;
        col = index%cols;
        QAccessibleTable2CellInterface *cell = interface->table2Interface()->cellAt(row, col);
        if (cell) {
            cell->rowColumnExtents(&row, &col, &rowExtents, &colExtents, &isSelected);
            success = true;
            delete cell;
        }

        QVariantList list;
        list << success << row << col << rowExtents << colExtents << isSelected;
        connection.send(message.createReply(list));

    } else if (function == "GetColumnExtentAt") {
        int row = message.arguments().at(0).toInt();
        int column = message.arguments().at(1).toInt();
        connection.send(message.createReply(interface->table2Interface()->cellAt(row, column)->columnExtent()));

    } else if (function == "GetRowExtentAt") {
        int row = message.arguments().at(0).toInt();
        int column = message.arguments().at(1).toInt();
        connection.send(message.createReply(interface->table2Interface()->cellAt(row, column)->rowExtent()));

    } else if (function == "GetColumnHeader") {
        int column = message.arguments().at(0).toInt();
        QSpiObjectReference ref;

        QAccessibleTable2CellInterface *cell = interface->table2Interface()->cellAt(0, column);
        if (cell) {
            QList<QAccessibleInterface*> header = cell->columnHeaderCells();
            delete cell;
            if (header.size() > 0) {
                ref = QSpiObjectReference(connection, QDBusObjectPath(pathForInterface(header.at(0), 0)));
                qDeleteAll(header);
            }
        }
        connection.send(message.createReply(QVariant::fromValue(ref)));

    } else if (function == "GetRowHeader") {
        int row = message.arguments().at(0).toInt();
        QSpiObjectReference ref;
        QAccessibleTable2CellInterface *cell = interface->table2Interface()->cellAt(row, 0);
        if (cell) {
            QList<QAccessibleInterface*> header = cell->rowHeaderCells();
            delete cell;
            if (header.size() > 0) {
                ref = QSpiObjectReference(connection, QDBusObjectPath(pathForInterface(header.at(0), 0)));
                qDeleteAll(header);
            }
        }
        connection.send(message.createReply(QVariant::fromValue(ref)));

    } else if (function == "GetSelectedColumns") {
        connection.send(message.createReply(QVariant::fromValue(interface->table2Interface()->selectedColumns())));
    } else if (function == "GetSelectedRows") {
        connection.send(message.createReply(QVariant::fromValue(interface->table2Interface()->selectedRows())));
    } else if (function == "IsColumnSelected") {
        int column = message.arguments().at(0).toInt();
        connection.send(message.createReply(interface->table2Interface()->isColumnSelected(column)));
    } else if (function == "IsRowSelected") {
        int row = message.arguments().at(0).toInt();
        connection.send(message.createReply(interface->table2Interface()->isRowSelected(row)));
    } else if (function == "IsSelected") {
        int row = message.arguments().at(0).toInt();
        int column = message.arguments().at(1).toInt();
        QAccessibleTable2CellInterface* cell = interface->table2Interface()->cellAt(row, column);
        connection.send(message.createReply(cell->isSelected()));
        delete cell;
    } else if (function == "AddColumnSelection") {
        int column = message.arguments().at(0).toInt();
        connection.send(message.createReply(interface->table2Interface()->selectColumn(column)));
    } else if (function == "AddRowSelection") {
        int row = message.arguments().at(0).toInt();
        connection.send(message.createReply(interface->table2Interface()->selectRow(row)));
    } else if (function == "RemoveColumnSelection") {
        int column = message.arguments().at(0).toInt();
        connection.send(message.createReply(interface->table2Interface()->unselectColumn(column)));
    } else if (function == "RemoveRowSelection") {
        int row = message.arguments().at(0).toInt();
        connection.send(message.createReply(interface->table2Interface()->unselectRow(row)));
    } else {
        qWarning() << "WARNING: QSpiAdaptorV2::handleMessage does not implement " << function << message.path();
        return false;
    }
    return true;
}
