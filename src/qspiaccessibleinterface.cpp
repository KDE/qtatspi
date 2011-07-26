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
    if (function == "GetRole") {
        QVariant v;
        v.setValue((uint) qSpiRoleMapping[interface->role(child)].spiRole());
        QDBusMessage reply = message.createReply(v);
        connection.send(reply);
        return true;
    } else if (function == "GetName") {
        sendReply(connection, message, interface->text(QAccessible::Name, child));
        return true;
    } else if (function == "GetRoleName") {
        QVariant v;
        v.setValue(qSpiRoleMapping[interface->role(child)].name());
        QDBusMessage reply = message.createReply(v);
        connection.send(reply);
        return true;
    } else if (function == "GetLocalizedRoleName") {
        QVariant v;
        v.setValue(qSpiRoleMapping[interface->role(child)].localizedName());
        QDBusMessage reply = message.createReply(v);
        connection.send(reply);
        return true;
    } else if (function == "GetChildCount") {
        int childCount = child ? 0 : interface->childCount();
        sendReply(connection, message, childCount);
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
        QAccessibleInterface *parent = accessibleParent(interface, child);

        QString path;
        if (!parent || parent->role(0) == QAccessible::Application) {
            path = QSPI_OBJECT_PATH_ROOT;
        } else {
            path = pathForInterface(parent, 0);
        }
        if (parent != interface)
            delete parent;

        QVariant ref;
        QSpiObjectReference v(connection, QDBusObjectPath(path));
        ref.setValue(v);
        sendReply(connection, message, ref);
        return true;
    } else if (function == "GetChildAtIndex") {
        int index = message.arguments().first().toInt();
        QString path;
        Q_ASSERT(child == 0);
        QAccessibleInterface *childInterface = 0;
        int childIndex = interface->navigate(QAccessible::Child, index+1, &childInterface);
        if (childIndex < 0)
            return false;
        if (childIndex == 0) {
            Q_ASSERT(childInterface);
            path = pathForInterface(childInterface, childIndex);
            delete childInterface;
        } else {
            path = pathForInterface(interface, childIndex);
        }

        QVariant ref;
        QSpiObjectReference v(connection, QDBusObjectPath(path));
        ref.setValue(v);
        QDBusMessage reply = message.createReply(ref);
        connection.send(reply);
        return true;
    } else if (function == "GetInterfaces") {
        QStringList ifaces;
        // FIXME
        ifaces << QSPI_INTERFACE_ACCESSIBLE;
        QVariant v;
        v.setValue(ifaces);
        QDBusMessage reply = message.createReply(v);
        connection.send(reply);
        return true;
    } else if (function == "GetDescription") {
        sendReply(connection, message, interface->text(QAccessible::Description, child));
        return true;
    } else if (function == "GetState") {
        quint64 spiState = spiStatesFromQState(interface->state(child));
        if (interface->table2Interface()) {
            setSpiStateBit(&spiState, ATSPI_STATE_MANAGES_DESCENDANTS);
        }
        QVariant v;
        QSpiUIntList l = spiStateSetFromSpiStates(spiState);
        v.setValue(l);
        QDBusMessage reply = message.createReply(v);
        connection.send(reply);
        return true;
    } else if (function == "GetAttributes") {
        QVariant attributes;
        attributes.setValue(QSpiAttributeSet());
        QDBusMessage reply = message.createReply(attributes);
        connection.send(reply);
        return true;
    } else if (function == "GetRelationSet") {
        QVariant relations;
        relations.setValue(relationSet(interface, child, connection));
        QDBusMessage reply = message.createReply(relations);
        connection.send(reply);
        return true;
    } else if (function == "GetApplication") {
        QVariant ref;
        QSpiObjectReference v(connection, QDBusObjectPath(QSPI_OBJECT_PATH_ROOT));
        ref.setValue(v);
        QDBusMessage reply = message.createReply(ref);
        connection.send(reply);
        return true;
    } else if (function == "GetChildren") {
        qDebug() << "IMPLEMENT GETCHILDREN";
        //    QSpiObjectReferenceArray ();
        return true;
    } else {
        qWarning() << "WARNING: QSpiAccessibleInterface::handleMessage does not implement " << function << message.path();
    }
    return false;
}

QSpiRelationArray QSpiAccessibleInterface::relationSet(QAccessibleInterface *interface, int child, const QDBusConnection &connection) const
{
    Q_ASSERT(child == 0);
    const QAccessible::RelationFlag relationsToCheck[] = {QAccessible::Label, QAccessible::Labelled, QAccessible::Controller, QAccessible::Controlled, static_cast<QAccessible::RelationFlag>(-1)};
    const AtspiRelationType relationTypes[] = {ATSPI_RELATION_LABELLED_BY, ATSPI_RELATION_LABEL_FOR, ATSPI_RELATION_CONTROLLED_BY, ATSPI_RELATION_CONTROLLER_FOR};

    QSpiRelationArray relations;
    QAccessibleInterface *target;

    for (int i = 0; relationsToCheck[i] >= 0; i++) {
        QList<QSpiObjectReference> related;
        int navigateResult = 1;

        for (int j = 1; navigateResult >= 0; j++) {
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
    sendReply(connection, message, QDBusVariant(argument));
}

void QSpiAccessibleInterface::sendReply(const QDBusConnection &connection, const QDBusMessage &message, const QDBusVariant &argument)
{
    QDBusMessage reply = message.createReply(QVariant::fromValue(argument));
    connection.send(reply);
//    qDebug() << "SIGNATURE " << message.member() << reply.signature() << message.signature();
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
    return path;
}

