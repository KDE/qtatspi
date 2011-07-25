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

#include <QDBusMessage>
#include <QDebug>
#include <qaccessible.h>

#include "generated/socket_proxy.h"
#include "qspiaccessibleinterface.h"

#include "constant_mappings.h"


QSpiAdaptorV2::QSpiAdaptorV2(DBusConnection *connection, QObject *parent)
    :QDBusVirtualObject(parent), m_dbus(connection)
{
    m_accessibleInterface = new QSpiAccessibleInterface();
}

QSpiAdaptorV2::~QSpiAdaptorV2()
{
    delete m_accessibleInterface;
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
        return m_accessibleInterface->handleMessage(accessible.first, accessible.second, function, message, connection);
    } else if (interface == QSPI_INTERFACE_APPLICATION) {
        return handleApplicationMessage(accessible.first, accessible.second, function, message, connection);
    } else {
        qDebug() << "QSpiAdaptorV2::handleMessage " << message.path() << interface << function;
    }
    return false;
}


bool QSpiAdaptorV2::handleApplicationMessage(QAccessibleInterface *, int, const QString &function, const QDBusMessage &message, const QDBusConnection &connection)
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
