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
        if (!parent || parent->role(0) == QAccessible::Application) {
            QVariant ref;
            QSpiObjectReference v(connection, QDBusObjectPath(QSPI_OBJECT_PATH_ROOT));
            ref.setValue(v);
            sendReply(connection, message, ref);
            return true;
        }

        return false;

//        QString path = pathForInterface(parent, 0);
//        QVariantList v;
//        v.append(connection.baseService());
//        v.append(QVariant::fromValue(QDBusObjectPath(path)));
//        sendReply(connection, message, v);
//        if (parent != interface)
//            delete parent;
//        return true;
    } else if (function == "GetChildAtIndex") {
        QString path;
        if (child) {
            path = pathForInterface(interface, child + 1);
        } else {
            QAccessibleInterface *childInterface = 0;
            int childIndex = interface->navigate(QAccessible::Child, child + 1, &childInterface);
            if (childIndex < 0)
                return false;
            path = pathForInterface(childInterface, childIndex);
            delete childInterface;
        }

        QVariant ref;
        QSpiObjectReference v(connection, QDBusObjectPath(path));
        ref.setValue(v);

        QDBusMessage reply = message.createReply(ref);
        connection.send(reply);

        return true;
    } else if (function == "GetInterfaces") {
        QStringList ifaces;
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

        return true;
    } else if (function == "GetRelationSet") {
        return true;
    } else {
        qWarning() << "WARNING: QSpiAccessibleInterface::handleMessage does not implement " << function << message.path();
    }
    return false;
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

