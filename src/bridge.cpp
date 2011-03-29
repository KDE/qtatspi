/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2008, 2009      Nokia.
 * Copyright 2008, 2009      Codethink Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
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
    cache = new QSpiDBusCache(this);
    dec = new DeviceEventControllerProxy(this);

    bool reg = dBusConnection().registerObject(QSPI_DEC_OBJECT_PATH, this, QDBusConnection::ExportAdaptors);
    qDebug() << "Registered DEC: " << reg;

    QAccessibleInterface* i = QAccessible::queryAccessibleInterface(qApp);
    QSpiApplication* accessible = new QSpiApplication(i);
    adaptorWithObjectMap.insert(qApp, accessible);
    allAdaptors.append(accessible);
}

QSpiAccessibleBridge::~QSpiAccessibleBridge ()
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
}

QSpiObjectReference QSpiAccessibleBridge::getRootReference() const
{
    return QSpiObjectReference(dBusConnection(), QDBusObjectPath(QSPI_OBJECT_PATH_ROOT));
}

void QSpiAccessibleBridge::notifyAccessibilityUpdate(int reason, QAccessibleInterface *interface, int index)
{
    Q_ASSERT(interface && interface->isValid());

    // this gets deleted, so create one if we don't have it yet
    QSpiAdaptor* accessible = interfaceToAccessible(interface, index, false);

    if (reason == QAccessible::Focus) {
        static QSpiAccessible *lastFocused = 0;
        if (lastFocused) {
            QDBusVariant data;
            data.setVariant(QVariant::fromValue(lastFocused->getReference()));
            qDebug() << "last focus: " << lastFocused;
            emit lastFocused->StateChanged("focused", 0, 0, data, getRootReference());
        }
        lastFocused = qobject_cast<QSpiAccessible*>(accessible);
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
    if (adaptorWithObjectMap.contains(object))
        return adaptorWithObjectMap.value(object);

    QAccessibleInterface* interface = QAccessible::queryAccessibleInterface(object);
    if (!interface) {
        qWarning() << "Create accessible for object which cannot create an accessible interface." << object;
        Q_ASSERT(interface);
        return 0;
    }

    return interfaceToAccessible(interface, 0, true);
}

QSpiAdaptor* QSpiAccessibleBridge::interfaceToAccessible(QAccessibleInterface* interface, int index, bool takeOwnershipOfInterface)
{
    Q_ASSERT(interface && interface->isValid());
    if (interface->object()) {
        QHash<QObject*, QSpiAdaptor*>::const_iterator i = adaptorWithObjectMap.constFind(interface->object());
        while (i != adaptorWithObjectMap.constEnd()) {
            if (i.value()->childIndex() == index)
                return i.value();
            ++i;
        }
    }

    // FIXME maybe cache (QHash<QAccessibleInterface*, QSpiAdaptor*> for things that don't have an object associated?
    if (interface->object() == 0) {
        foreach (QSpiAdaptor* a, adaptorWithoutObjectList) {
            if (a->associatedInterface() == interface && a->childIndex() == index)
                return a;
        }
        qDebug() << "      QSpiAccessibleBridge::interfaceToAccessible new with no object";
    } else {
        qDebug() << "      QSpiAccessibleBridge::interfaceToAccessible new: " << interface->object()
                 << interface->text(QAccessible::Name, index) << qSpiRoleMapping.value(interface->role(index)).name();
    }

    // FIXME look for interfaces with no object or index > 0

    // if we cannot keep the interface around (notifyAccessibility will delete interfaces)
    // we need to ask for one that we can keep
    if (!takeOwnershipOfInterface) {
        interface = QAccessible::queryAccessibleInterface(interface->object());
    }
    QSpiAdaptor *accessible = new QSpiAccessible(interface, index);

    // put ourself in the list of accessibles
    if (interface->object()) {
        adaptorWithObjectMap.insertMulti(interface->object(), accessible);
        connect(interface->object(), SIGNAL(destroyed(QObject*)), this, SLOT(objectDestroyed(QObject*)));
    } else {
        adaptorWithoutObjectList.append(accessible);
    }
    allAdaptors.append(accessible);

    // say hello to d-bus
    cache->emitAddAccessible(accessible->getCacheItem());

    // notify about the new child of our parent
    int childCount = 0;
    QSpiAdaptor* parentAdaptor = 0;
    if (index == 0) {
        QAccessibleInterface *parent = 0;
        interface->navigate(QAccessible::Ancestor, 1, &parent);
        if (parent) {
            parentAdaptor = interfaceToAccessible(parent, 0, true);
            childCount = parent->childCount();
        }
    } else {
        parentAdaptor = interfaceToAccessible(interface, 0, true);
        childCount = interface->childCount();
    }

    if (parentAdaptor) {
        QSpiObjectReference r = accessible->getReference();
        QDBusVariant data;
        data.setVariant(QVariant::fromValue(r));
        parentAdaptor->signalChildrenChanged("add", childCount, 0, data);
    }

    return accessible;
}

void QSpiAccessibleBridge::objectDestroyed(QObject* o)
{
    QHash<QObject*, QSpiAdaptor*>::iterator i = adaptorWithObjectMap.find(o);
    if (i != adaptorWithObjectMap.end()) {
        allAdaptors.removeAll(i.value());
        adaptorWithObjectMap.erase(i);
    }
}

void QSpiAccessibleBridge::removeAdaptor(QSpiAdaptor *adaptor)
{
    if (adaptorWithoutObjectList.contains(adaptor)) {
        adaptorWithoutObjectList.removeAll(adaptor);
    } else {
        adaptorWithObjectMap.remove(adaptor->getObject());
    }

    allAdaptors.removeAll(adaptor);
}
