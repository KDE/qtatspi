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

#include "qspiadaptorv2.h"

#include "application.h"
#include "cache.h"
#include "constant_mappings.h"
#include "dbusconnection.h"
#include "struct_marshallers.h"

#include "generated/dec_proxy.h"
#include "generated/event_adaptor.h"


#define QSPI_DEC_NAME        "/org/a11y/atspi/Registry"
#define QSPI_DEC_OBJECT_PATH "/org/a11y/atspi/registry/deviceeventcontroller"

#define QSPI_OBJECT_PATH_ACCESSIBLE  "/org/a11y/atspi/accessible"

QSpiAccessibleBridge* QSpiAccessibleBridge::self = 0;

QSpiAccessibleBridge::QSpiAccessibleBridge()
    : cache(0)
{
    Q_ASSERT(self == 0);
    self = this;

    dbusConnection = new DBusConnection();
    if (!dBusConnection().isConnected()) {
        qWarning() << "Could not connect to dbus.";
    }

    qSpiInitializeStructTypes();
    qSpiInitializeConstantMappings();

    /* Create the cache of accessible objects */
    cache = new QSpiDBusCache(dBusConnection(), this);
    dec = new DeviceEventControllerProxy(this);

    bool reg = dBusConnection().registerObject(QSPI_DEC_OBJECT_PATH, this, QDBusConnection::ExportAdaptors);
    qDebug() << "Registered DEC: " << reg;

    dbusAdaptor = new QSpiAdaptorV2(dbusConnection, this);
    dBusConnection().registerVirtualObject(QSPI_OBJECT_PATH_ACCESSIBLE, dbusAdaptor, QDBusConnection::SubPath);
    dbusAdaptor->registerApplication();

    QSpiApplicationAdaptor *applicationAdaptor = new QSpiApplicationAdaptor(dbusConnection->connection(), this);
    connect(applicationAdaptor, SIGNAL(windowActivated(QObject*)), dbusAdaptor, SLOT(windowActivated(QObject*)));
}

QSpiAccessibleBridge::~QSpiAccessibleBridge()
{
    delete dbusConnection;
} // Qt currently doesn't delete plugins.

QDBusConnection QSpiAccessibleBridge::dBusConnection() const
{
    return dbusConnection->connection();
}

void QSpiAccessibleBridge::setRootObject(QAccessibleInterface *interface)
{
    // the interface we get will be for the QApplication object.
    // we already cache it in the constructor.
    Q_ASSERT(interface->object() == qApp);
    dbusAdaptor->setInitialized(true);
}

void QSpiAccessibleBridge::notifyAccessibilityUpdate(int reason, QAccessibleInterface *interface, int index)
{
    dbusAdaptor->notify(reason, interface, index);
}

//QSpiAdaptor* QSpiAccessibleBridge::interfaceToAccessible(QAccessibleInterface* interface, int index, bool takeOwnershipOfInterface)
//{
//    Q_ASSERT(interface && interface->isValid());

//    if (interface->object() == qApp) {
//        return adaptors.value(QSPI_OBJECT_PATH_ROOT);
//    }

//    QString path = QSpiAccessible::pathForInterface(interface, index);
//    // optimize?
//    if (adaptors.contains(path)) {
//        if (adaptors.value(path)->associatedInterface()->object() != interface->object()) {

//            QSpiAdaptor* originalAdaptor = adaptors.take(path);
//            qDebug() << "not the same: " << originalAdaptor->associatedInterface()->object() << interface->object()
//                     << " at path: " << path;

//            // ItemViews create qobjects for rows/cells later as needed.
//            // Those may initially be 0.

//            // remove object
//            // add new interface
//            cache->emitRemoveAccessible(originalAdaptor->getReference());
//            delete originalAdaptor;
////            Q_ASSERT(0);
//        } else {
//            return adaptors.value(path);
//        }
//    }

//// FIXME if this works, we can save code below...
////    QAccessibleInterface* copy(QAccessibleInterface(*interface));

//    // if we cannot keep the interface around (notifyAccessibility will delete interfaces)
//    // we need to ask for one that we can keep
//    if (!takeOwnershipOfInterface) {
//        QAccessibleInterface* ownedInterface = QAccessible::queryAccessibleInterface(interface->object());
//        if (!ownedInterface) {
//            QAccessibleInterface* parentInterface;
//            interface->navigate(QAccessible::Ancestor, 1, &parentInterface);
//            Q_ASSERT(parentInterface);
//            int index = parentInterface->indexOfChild(interface);
//            parentInterface->navigate(QAccessible::Child, index, &ownedInterface);
//            delete parentInterface;
//        }
//        Q_ASSERT(ownedInterface);
//        Q_ASSERT(interface->object() == ownedInterface->object());
//        interface = ownedInterface;
//    }
//    QSpiAdaptor *accessible = new QSpiAccessible(interface, index);

//    // put ourself in the list of accessibles
//    adaptors.insert(path, accessible);

//    notifyAboutCreation(accessible);
//    return accessible;
//}


