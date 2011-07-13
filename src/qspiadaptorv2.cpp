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

#include "generated/socket_proxy.h"

#define QSPI_INTERFACE_PREFIX "org.a11y.atspi"

#define QSPI_INTERFACE_ACCESSIBLE            QSPI_INTERFACE_PREFIX ".Accessible"
#define QSPI_INTERFACE_ACTION                QSPI_INTERFACE_PREFIX ".Action"
#define QSPI_INTERFACE_APPLICATION           QSPI_INTERFACE_PREFIX ".Application"
#define QSPI_INTERFACE_COLLECTION            QSPI_INTERFACE_PREFIX ".Collection"
#define QSPI_INTERFACE_COMPONENT             QSPI_INTERFACE_PREFIX ".Component"
#define QSPI_INTERFACE_DOCUMENT              QSPI_INTERFACE_PREFIX ".Document"
#define QSPI_INTERFACE_EDITABLE_TEXT         QSPI_INTERFACE_PREFIX ".EditableText"
#define QSPI_INTERFACE_HYPERLINK             QSPI_INTERFACE_PREFIX ".Hyperlink"
#define QSPI_INTERFACE_HYPERTEXT             QSPI_INTERFACE_PREFIX ".Hypertext"
#define QSPI_INTERFACE_IMAGE                 QSPI_INTERFACE_PREFIX ".Image"
#define QSPI_INTERFACE_REGISTRY              QSPI_INTERFACE_PREFIX ".Registry"
#define QSPI_INTERFACE_SELECTION             QSPI_INTERFACE_PREFIX ".Selection"
#define QSPI_INTERFACE_TABLE                 QSPI_INTERFACE_PREFIX ".Table"
#define QSPI_INTERFACE_TEXT                  QSPI_INTERFACE_PREFIX ".Text"
#define QSPI_INTERFACE_TREE                  QSPI_INTERFACE_PREFIX ".Tree"
#define QSPI_INTERFACE_VALUE                 QSPI_INTERFACE_PREFIX ".Value"

#define QSPI_OBJECT_PATH_PREFIX  "/org/a11y/atspi/accessible/"
#define QSPI_OBJECT_PATH_NULL    QSPI_OBJECT_PATH_PREFIX "null"
#define QSPI_OBJECT_PATH_ROOT    QSPI_OBJECT_PATH_PREFIX "root"

#define QSPI_REGISTRY_NAME "org.a11y.atspi.Registry"

QSpiAdaptorV2::QSpiAdaptorV2(DBusConnection *connection, QObject *parent)
    :QDBusVirtualObject(parent), m_dbus(connection)
{
    callAccessibilityRegistry();
}

QString QSpiAdaptorV2::introspect(const QString &path) const
{
    qWarning() << "QSpiAdaptorV2::introspect on " << path;
    return QString();
}

bool QSpiAdaptorV2::handleMessage(const QDBusMessage &message, const QDBusConnection &connection)
{
    Q_UNUSED(connection)
    qDebug() << "QSpiAdaptorV2::handleMessage" << message.arguments() << message.member() << message.path();
    return false;
}

void QSpiAdaptorV2::callAccessibilityRegistry()
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
