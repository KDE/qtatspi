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

    Q_ASSERT(parts.size() > 5);

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
        function = message.arguments().at(1).toString();
    }

    // switch interface to call
    if (interface == QSPI_INTERFACE_ACCESSIBLE) {
        return m_accessibleInterface->handleMessage(accessible.first, accessible.second, function, message, connection);
    } else {
        qDebug() << "QSpiAdaptorV2::handleMessage " << message.path() << interface << function;
    }
    return false;
}

//bool QSpiAdaptorV2::handleMessageForRoot(const QDBusMessage &message, const QDBusConnection &connection)
//{
//    if (message.interface() == "org.freedesktop.DBus.Properties") {
//        if (message.member() == "Set") {
//            Q_ASSERT(message.signature() == "ssv");
//            QString interface = message.arguments().at(0).toString();
//            Q_ASSERT(interface == "org.a11y.atspi.Application");
//            QString property = message.arguments().at(1).toString();
//            QVariant value = qvariant_cast<QDBusVariant>(message.arguments().at(2)).variant();
//            if (property == "Id")
//                m_applicationId = value.toInt();

//        } else if (message.member() == QLatin1String("Get") && message.signature() == QLatin1String("ss")) {
//            Q_ASSERT(message.signature() == "ss");
//            QString interface = message.arguments().at(0).toString();
//            QString property = message.arguments().at(1).toString();
//            if (interface == "org.a11y.atspi.Application") {
//                if (property == "Id") {
//                    QDBusMessage reply = message.createReply(QVariant::fromValue(QDBusVariant(m_applicationId)));
//                    return connection.send(reply);
//                }
//            } else if (interface == "org.a11y.atspi.Accessible") {
//                if (property == "Name") {
//                    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(qApp);
//                    QString name = iface->text(QAccessible::Name, 0);
//                    QDBusMessage reply = message.createReply(QVariant::fromValue(QDBusVariant(name)));
//                    return connection.send(reply);

//                }
//            }

//        }
//    }
//    return false;
//}

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
