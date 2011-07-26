/*
 * D-Bus AT-SPI, Qt Adaptor
 *
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

#include "qspiaccessibleinterface.h"

#include <qdebug.h>
#include <qdbusmessage.h>
#include <dbusconnection.h>
#include <qaccessible.h>

#include "constant_mappings.h"


QSpiAccessibleInterface::QSpiAccessibleInterface()
{
}

bool QSpiAccessibleInterface::handleMessage(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection)
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
        qWarning() << "WARNING: QSpiAccessibleInterface::handleMessage does not implement " << function << message.path();
    }
    return false;
}

QStringList QSpiAccessibleInterface::accessibleInterfaces(QAccessibleInterface */*interface*/, int /*child*/) const
{
    QStringList ifaces;
    // FIXME
    ifaces << QSPI_INTERFACE_ACCESSIBLE;
    return ifaces;
}

QSpiRelationArray QSpiAccessibleInterface::relationSet(QAccessibleInterface *interface, int child, const QDBusConnection &connection) const
{
    QSpiRelationArray relations;
    if (child == 0) {
        qDebug() << "QSpiAccessibleInterface::relationSet currently has a problem with child ids.";
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

void QSpiAccessibleInterface::sendReply(const QDBusConnection &connection, const QDBusMessage &message, const QVariant &argument)
{
    QDBusMessage reply = message.createReply(argument);
    connection.send(reply);
}

QAccessibleInterface *QSpiAccessibleInterface::accessibleParent(QAccessibleInterface *iface, int child)
{
    if (child)
        return iface;
    QAccessibleInterface *parent = 0;
    iface->navigate(QAccessible::Ancestor, 1, &parent);
    return parent;
}

QString QSpiAccessibleInterface::pathForObject(QObject *object)
{
    Q_ASSERT(object);
    return QSPI_OBJECT_PATH_PREFIX + QString::number(reinterpret_cast<size_t>(object));
}

QString QSpiAccessibleInterface::pathForInterface(QAccessibleInterface *interface, int childIndex)
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

