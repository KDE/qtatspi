#include "qspiaccessibleinterface.h"

#include <qdebug.h>
#include <qdbusmessage.h>
#include <dbusconnection.h>
#include <qaccessible.h>

#include "constant_mappings.h"

#define QSPI_OBJECT_PATH_PREFIX  "/org/a11y/atspi/accessible/"

QSpiAccessibleInterface::QSpiAccessibleInterface()
{
}

bool QSpiAccessibleInterface::handleMessage(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection)
{
    if (function == "GetRole") {
        sendReply(connection, message, (int) qSpiRoleMapping[interface->role(child)].spiRole());
        return true;
    } else if (function == "Name") {
        sendReply(connection, message, interface->text(QAccessible::Name, child));
        return true;
    } else if (function == "GetRoleName") {
        sendReply(connection, message, qSpiRoleMapping[interface->role(child)].name());
        return true;
    } else if (function == "GetLocalizedRoleName") {
        sendReply(connection, message, qSpiRoleMapping[interface->role(child)].localizedName());
        return true;
    } else if (function == "ChildCount") {
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
    } else if (function == "Parent") {
        QAccessibleInterface *parent = accessibleParent(interface, child);
        if (!parent) {
            // FIXME
            // sendReply(connection, message, ref);
            return false;
        }
        QString path = pathForInterface(parent, 0);
        QVariantList v;
        v.append(connection.baseService());
        v.append(QVariant::fromValue(QDBusObjectPath(path)));
        sendReply(connection, message, v);
        if (parent != interface)
            delete parent;
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

