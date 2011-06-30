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

#include "adaptor.h"
#include "accessible.h"
#include "application.h"
#include "cache.h"
#include "constant_mappings.h"
#include "dbusconnection.h"
#include "struct_marshallers.h"

#include "generated/dec_proxy.h"
#include "generated/event_adaptor.h"

#include <QEvent>
#include <QKeyEvent>


#define QSPI_DEC_NAME        "/org/a11y/atspi/Registry"
#define QSPI_DEC_OBJECT_PATH "/org/a11y/atspi/registry/deviceeventcontroller"

QSpiAccessibleBridge* QSpiAccessibleBridge::self = 0;

QSpiAccessibleBridge::QSpiAccessibleBridge()
    : cache(0), initialized(false)
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

    QAccessibleInterface* i = QAccessible::queryAccessibleInterface(qApp);
    QSpiAdaptor* applicationAccessible = new QSpiApplication(dbusConnection->connection(), i);
    adaptors.insert(QString(QSPI_OBJECT_PATH_ROOT), applicationAccessible);
    connect(applicationAccessible, SIGNAL(windowActivated(QObject*)), this, SLOT(windowActivated(QObject*)));
}

void QSpiAccessibleBridge::windowActivated(QObject* window)
{
    QSpiAdaptor* a = spiBridge->objectToAccessible(window);
    QSpiAccessible* acc = static_cast<QSpiAccessible*>(a);
    acc->windowActivated();
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
    initialized = true;

    // the interface we get will be for the QApplication object.
    // we already cache it in the constructor.
    Q_ASSERT(interface->object() == qApp);
}

QSpiObjectReference QSpiAccessibleBridge::getRootReference() const
{
    return QSpiObjectReference(dBusConnection(), QDBusObjectPath(QSPI_OBJECT_PATH_ROOT));
}

void QSpiAccessibleBridge::notifyAccessibilityUpdate(int reason, QAccessibleInterface *interface, int index)
{
    Q_ASSERT(interface && interface->isValid());

    if (!initialized)
        return;

    // this gets deleted, so create one if we don't have it yet
    QSpiAdaptor* accessible = interfaceToAccessible(interface, index, false);
    //Q_ASSERT(accessible->associatedInterface()->object() == interface->object());
    if (accessible->associatedInterface()->object() == interface->object()) {
        qWarning() << "Creating accessible with different object than the original interface!";
    }

    switch (reason) {
    case QAccessible::ObjectCreated:
        qDebug() << "created" << interface->object();
        // make sure we don't duplicate this. seems to work for qml loaders.
        notifyAboutCreation(accessible);
        break;
    case QAccessible::ObjectShow:
        qDebug() << "show" << interface->object();
        break;
    case QAccessible::Focus: {
        static QSpiAccessible *lastFocused = 0;
        if (lastFocused) {
            QDBusVariant data;
            data.setVariant(QVariant::fromValue(lastFocused->getReference()));
            emit lastFocused->StateChanged("focused", 0, 0, data, getRootReference());
        }
        lastFocused = qobject_cast<QSpiAccessible*>(accessible);
    }
    }
//    qDebug() << "QSpiAccessibleBridge::notifyAccessibilityUpdate" << QString::number(reason, 16)
//             << " obj: " << interface->object()
//             << (interface->isValid() ? interface->object()->objectName() : " invalid interface!")
//             << accessible->interface;
    accessible->accessibleEvent((QAccessible::Event)reason);
}

QSpiAdaptor* QSpiAccessibleBridge::objectToAccessible(QObject *object)
{
    Q_ASSERT(object);

    QString path = QSpiAccessible::pathForObject(object);
    if (adaptors.contains(path))
        return adaptors.value(path);

    QAccessibleInterface* interface = QAccessible::queryAccessibleInterface(object);
    if (!interface) {
        qWarning() << "Create accessible for object which cannot create an accessible interface." << object;
        return 0;
    }

    return interfaceToAccessible(interface, 0, true);
}

QSpiAdaptor* QSpiAccessibleBridge::interfaceToAccessible(QAccessibleInterface* interface, int index, bool takeOwnershipOfInterface)
{
    Q_ASSERT(interface && interface->isValid());

    if (interface->object() == qApp) {
        return adaptors.value(QSPI_OBJECT_PATH_ROOT);
    }

    QString path = QSpiAccessible::pathForInterface(interface, index);
    // optimize?
    if (adaptors.contains(path)) {
        if (adaptors.value(path)->associatedInterface()->object() != interface->object()) {

            QSpiAdaptor* originalAdaptor = adaptors.take(path);
            qDebug() << "not the same: " << originalAdaptor->associatedInterface()->object() << interface->object()
                     << " at path: " << path;


            // ItemViews create qobjects for rows/cells later as needed.
            // Those may initially be 0.

            // remove object
            // add new interface
            cache->emitRemoveAccessible(originalAdaptor->getReference());
            delete originalAdaptor;
//            Q_ASSERT(0);
        } else {
            return adaptors.value(path);
        }
    }

// FIXME if this works, we can save code below...
//    QAccessibleInterface* copy(QAccessibleInterface(*interface));

    // if we cannot keep the interface around (notifyAccessibility will delete interfaces)
    // we need to ask for one that we can keep
    if (!takeOwnershipOfInterface) {
        QAccessibleInterface* ownedInterface = QAccessible::queryAccessibleInterface(interface->object());
        if (!ownedInterface) {
            QAccessibleInterface* parentInterface;
            interface->navigate(QAccessible::Ancestor, 1, &parentInterface);
            Q_ASSERT(parentInterface);
            int index = parentInterface->indexOfChild(interface);
            parentInterface->navigate(QAccessible::Child, index, &ownedInterface);
            delete parentInterface;
        }
        Q_ASSERT(ownedInterface);
        Q_ASSERT(interface->object() == ownedInterface->object());
        interface = ownedInterface;
    }
    QSpiAdaptor *accessible = new QSpiAccessible(interface, index);

    // put ourself in the list of accessibles
    adaptors.insert(path, accessible);

    notifyAboutCreation(accessible);
    return accessible;
}

void QSpiAccessibleBridge::notifyAboutCreation(QSpiAdaptor* accessible)
{
    // say hello to d-bus
    cache->emitAddAccessible(accessible->getCacheItem());

    // notify about the new child of our parent
    int childCount = 0;
    QSpiAdaptor* parentAdaptor = 0;
    if (accessible->childIndex() == 0) {
        QAccessibleInterface *parent = 0;
        accessible->associatedInterface()->navigate(QAccessible::Ancestor, 1, &parent);
        if (parent) {
            parentAdaptor = interfaceToAccessible(parent, 0, true);
            childCount = parent->childCount();
        }
    } else {
        parentAdaptor = interfaceToAccessible(accessible->associatedInterface(), 0, true);
        childCount = accessible->associatedInterface()->childCount();
    }

    if (parentAdaptor) {
        QSpiObjectReference r = accessible->getReference();
        QDBusVariant data;
        data.setVariant(QVariant::fromValue(r));
        parentAdaptor->signalChildrenChanged("add", childCount, 0, data);
    }
}

void QSpiAccessibleBridge::objectDestroyed(QObject* o)
{
    QString path = QSpiAccessible::pathForObject(o);
    adaptors.remove(path);
}

void QSpiAccessibleBridge::removeAdaptor(QSpiAdaptor *adaptor)
{
    adaptors.remove(adaptor->getReference().path.path());
}
