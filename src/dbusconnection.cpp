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


#include "dbusconnection.h"

#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusPendingReply>
#include <QtCore/QDebug>

#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

/*!
    \class DBusConnection

    \brief DBusConnection to the atspi accessibility dbus.

    The atspi registry daemon provides dbus services accessible with this
    class. Usually the org.a11y dbus service offers two central methods:
    1) org.a11y.Status.IsEnabled is a read/write boolean to enable/disable
    accessibility system wide.
    2) org.a11y.Bus.GetAddress provides the way to create an own private
    connection, to the atspi registryd dbus daemon. All communication will
    be done via that dbus connection.
*/


/*!
  Connects to the accessibility dbus.
*/
DBusConnection::DBusConnection()
    : QObject()
    , dbusConnection(QDBusConnection::sessionBus())
    , initWatcher(0)
    , connected(false)
{
    init();
}

void DBusConnection::init()
{
    if (!dbusConnection.isConnected()) {
        qWarning("Could not connect to DBus session bus.");
        return;
    }

    QDBusMessage m = QDBusMessage::createMethodCall(QLatin1String("org.a11y.Bus"), QLatin1String("/org/a11y/bus"), QLatin1String("org.freedesktop.DBus.Properties"), QLatin1String("Get"));
    m.setArguments(QVariantList() << QLatin1String("org.a11y.Status") << QLatin1String("IsEnabled"));
    QDBusPendingCall async = dbusConnection.asyncCall(m);
    initWatcher = new QDBusPendingCallWatcher(async, this);
    connect(initWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(initFinished()));
}

/*!
  Called when the connection was fetched.

  This will happen exactly one time during the lifetime of a DBusConnection instance
  at the very beginning when the instance is created.
  This method emits the \a connectionFetched() signal once the DBus reply from the
  atspi accessibility dbus was received and processed.
*/
void DBusConnection::initFinished()
{
    if (!initWatcher)
        return;

    QDBusPendingReply<QVariant> reply = *initWatcher;
    bool enabled = qdbus_cast< QVariant >(reply).toBool();

    if (enabled) {
        dbusConnection = connectDBus();
        connected = dbusConnection.isConnected();
        if (!connected) {
            qWarning("Could not connect to Accessibility DBus.");
        }
    }

    initWatcher->deleteLater();
    initWatcher = 0;

    emit connectionFetched();
}

QDBusConnection DBusConnection::connectDBus()
{
    QString address = getAccessibilityBusAddress();

    if (!address.isEmpty()) {
        QDBusConnection c = QDBusConnection::connectToBus(address, "a11y");
        if (c.isConnected()) {
            qDebug() << "Connected to accessibility bus at: " << address;
            return c;
        }
        qWarning("Found Accessibility DBus address but cannot connect. Falling back to session bus.");
    } else {
        qWarning("Accessibility DBus not found. Falling back to session bus.");
    }

    return QDBusConnection::sessionBus();
}

QString DBusConnection::getAccessibilityBusAddress() const
{
    QString address = getAccessibilityBusAddressDBus();
    if (address.isEmpty()) {
        address = getAccessibilityBusAddressXAtom();
    }
    return address;
}

QString DBusConnection::getAccessibilityBusAddressDBus() const
{
    QDBusConnection c = QDBusConnection::sessionBus();

    QDBusMessage m = QDBusMessage::createMethodCall("org.a11y.Bus",
                                                    "/org/a11y/bus",
                                                    "org.a11y.Bus", "GetAddress");
    QDBusMessage reply = c.call(m);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "Qt at-spi: error getting the accessibility dbus address: " << reply.errorMessage();
        return QString();
    }

    QString busAddress = reply.arguments().at(0).toString();
    qDebug() << "Got bus address: " << busAddress;
    return busAddress;
}

QString DBusConnection::getAccessibilityBusAddressXAtom() const
{
    Display* bridge_display = QX11Info::display();

    Atom actualType;
    int actualFormat;
    char *propData = 0;
    unsigned long nItems;
    unsigned long leftOver;
    Atom AT_SPI_BUS = XInternAtom(bridge_display, "AT_SPI_BUS", False);
    XGetWindowProperty (bridge_display,
                        XDefaultRootWindow (bridge_display),
                        AT_SPI_BUS, 0L,
                        (long) BUFSIZ, False,
                        (Atom) 31, &actualType, &actualFormat,
                        &nItems, &leftOver,
                        (unsigned char **) (void *) &propData);

    QString busAddress = QString::fromLocal8Bit(propData);

    XFree(propData);
    return busAddress;
}

/*!
  Returns if the connection to the accessibility dbus is still in
  progress and not done yet.

  The initial fetch of the comnnection happens over dbus and as such can block for
  a longer time means may need longer. To make it possible that users of this class
  do not block while that happens it is possible to use this method to determinate
  if fetching the connection is currently work in progress and if so connect with
  the \a connectionFetched signal to be called back when the connection is ready.
*/
bool DBusConnection::isFetchingConnection() const
{
    return initWatcher;
}

/*!
  Returns if the connection to the accessibility dbus was successful established.
*/
bool DBusConnection::isConnected() const
{
    if (initWatcher) {
        initWatcher->waitForFinished(); // block
        const_cast<DBusConnection*>(this)->initFinished();
    }
    return connected;
}

/*!
  Returns the DBus connection that got established.

  This may either be the session bus or a referenced accessibility bus. If the
  connection was not fetched yet, means \a isFetchingConnection returns true, then
  calling this method will block till the connection was fetched.
*/
QDBusConnection DBusConnection::connection() const
{
    if (initWatcher) {
        initWatcher->waitForFinished(); // block
        const_cast<DBusConnection*>(this)->initFinished();
    }
    return dbusConnection;
}
