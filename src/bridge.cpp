/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2008-2011 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright 2008, 2009      Codethink Ltd.
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

#include "bridge.h"

#include <qaccessible2.h>

#include "atspiadaptor.h"

#include "cache.h"
#include "constant_mappings.h"
#include "dbusconnection.h"
#include "struct_marshallers.h"

#include "generated/dec_proxy.h"

QSpiAccessibleBridge* QSpiAccessibleBridge::self = 0;

/*!
    \class QSpiAccessibleBridge

    \brief QSpiAccessibleBridge

    QSpiAccessibleBridge
*/

QSpiAccessibleBridge::QSpiAccessibleBridge()
    : cache(0)
    , dec(0)
    , dbusAdaptor(0)
    , rootObjectInitialized(false)
    , dbusConnection(0)
{
    Q_ASSERT(self == 0);
    self = this;

    dbusConnection = new DBusConnection();
    // Delay setup till DBusConnection::connectionFetched() was emitted.
    connect(dbusConnection, SIGNAL(connectionFetched()), this, SLOT(initDBusConnection()));
}

QSpiAccessibleBridge::~QSpiAccessibleBridge()
{
    delete dbusConnection;
} // Qt currently doesn't delete plugins.

void QSpiAccessibleBridge::initDBusConnection()
{
    // Initial DBusConnection setup work was done asynchronously and either succeeded
    // or not. If not we do not need to do anything future. The whole Qt-application
    // is just not accessible using atspi and our bridge plugins job is done.
    if (!dbusConnection->isConnected())
        return;

    // When dbus connection to atspi-registryd daemon was success established register
    // all the dbus types, adaptors and caches for the Qt-application. Once done our
    // bridge plugin is fully setup and ready.
    qSpiInitializeStructTypes();
    qSpiInitializeConstantMappings();

    /* Create the cache of accessible objects */
    cache = new QSpiDBusCache(dbusConnection->connection(), this);
    dec = new DeviceEventControllerProxy(this);

    bool reg = dbusConnection->connection().registerObject(ATSPI_DBUS_PATH_DEC, this, QDBusConnection::ExportAdaptors);
    qDebug() << "Registered DEC: " << reg;

    dbusAdaptor = new AtSpiAdaptor(dbusConnection, this);
    dbusConnection->connection().registerVirtualObject(QSPI_OBJECT_PATH_ACCESSIBLE, dbusAdaptor, QDBusConnection::SubPath);
    dbusAdaptor->registerApplication();
    if (rootObjectInitialized)
        dbusAdaptor->setInitialized(true);
}

void QSpiAccessibleBridge::setRootObject(QAccessibleInterface *interface)
{
    // the interface we get will be for the QApplication object.
    // we already cache it in the constructor.
    Q_ASSERT(interface->object() == qApp);
    rootObjectInitialized = true;
    if (dbusAdaptor)
        dbusAdaptor->setInitialized(true);
}

void QSpiAccessibleBridge::notifyAccessibilityUpdate(int reason, QAccessibleInterface *interface, int index)
{
    qDebug() << Q_FUNC_INFO << "QAccessible::Event=" << QString::number(reason, 16) << "dbusAdaptor=" << dbusAdaptor;

    if(!dbusAdaptor) {
        // When still asynchronously calling the org.a11y.Status.IsEnabled dbus method
        //(isFetchingConnection() returns true) then not call expensive (cause
        // blocking) dbusConnection->isConnected() or dbusConnection->connected()
        // when the object is not visible yet.
        if (reason != QAccessible::ObjectShow)
            if (dbusConnection->isFetchingConnection())
                return;

        // Following call would block till the connection was either successful established
        // or was rejected cause 1) The org.a11y dbus registryd daemon is not running or
        // 2) it is running but disabled (org.a11y.Status.IsEnabled return false).
        bool connected = dbusConnection->isConnected();
        if (!connected)
            return;
        Q_ASSERT(dbusAdaptor);
        if (!dbusAdaptor)
            return;
    }

    dbusAdaptor->notify(reason, interface, index);
}
