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
#include "object.h"
#include "struct_marshallers.h"

#include "generated/dec_proxy.h"
#include "generated/event_adaptor.h"

#include <QEvent>
#include <QKeyEvent>

#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#define QSPI_DEC_NAME        "/org/a11y/atspi/Registry"
#define QSPI_DEC_OBJECT_PATH "/org/a11y/atspi/registry/deviceeventcontroller"

QSpiAccessibleBridge* QSpiAccessibleBridge::self = 0;

QSpiAccessibleBridge::QSpiAccessibleBridge()
    : cache(0), dbusConnection(connectDBus())
{
    Q_ASSERT(self == 0);
    self = this;

    if (!dbusConnection.isConnected()) {
        qWarning() << "Could not connect to dbus.";
    }

    qSpiInitializeStructTypes();
    qSpiInitializeConstantMappings();

    /* Create the cache of accessible objects */
    cache = new QSpiDBusCache(this);
    dec = new DeviceEventControllerProxy(this);

    bool reg = dbusConnection.registerObject(QSPI_DEC_OBJECT_PATH, this, QDBusConnection::ExportAdaptors);
    qDebug() << "Registered DEC: " << reg;

    QAccessibleInterface* i = QAccessible::queryAccessibleInterface(qApp);
    cacheObjects_.insert(qApp, new QSpiApplication(i));
    registerChildren(i);
}

QSpiAccessibleBridge::~QSpiAccessibleBridge ()
{} // Qt currently doesn't delete plugins.

QDBusConnection QSpiAccessibleBridge::connectDBus()
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

    QDBusConnection c = QDBusConnection::sessionBus();
    if (!c.isConnected()) {
        qWarning("Could not connect to DBus.");
    }
    return QDBusConnection::sessionBus();
}

QString QSpiAccessibleBridge::getAccessibilityBusAddress() const
{
    Display* bridge_display = QX11Info::display();

    Atom actualType;
    int actualFormat;
    char *propData = 0;
    unsigned long nItems;
    unsigned long leftOver;
    Atom AT_SPI_BUS = XInternAtom (bridge_display, "AT_SPI_BUS", False);
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

QDBusConnection QSpiAccessibleBridge::dBusConnection() const
{
    return dbusConnection;
}

void QSpiAccessibleBridge::setRootObject(QAccessibleInterface *interface)
{
    // the interface we get will be for the QApplication object.
    Q_ASSERT(interface->object() == qApp);
    if (!cacheObjects_.contains(qApp))
        cacheObjects_.insert(qApp, new QSpiApplication(interface));
}

QSpiObjectReference QSpiAccessibleBridge::getRootReference() const
{
    Q_ASSERT(cacheObjects_.contains(qApp));
    return cacheObjects_.value(qApp)->getReference();
}

void QSpiAccessibleBridge::notifyAccessibilityUpdate(int reason, QAccessibleInterface *interface, int index)
{
    if (index == 0) {
        notifyAccessibilityUpdate(reason, interface);
        return;
    }

    qDebug() << "notifyAccessibilityUpdate CHILD" << interface->object() << " i: " << index << " reason: " << QString::number(reason, 16);

    QAccessibleInterface *child = 0;
    interface->navigate(QAccessible::Child, index, &child);
    Q_ASSERT(child);

    if (!cacheObjects_.contains(child->object())) {
        qDebug() << " Add new child: " << interface->object();

        QSpiAdaptor *accessibleChild = interfaceToAccessible(child);
        cacheObjects_.insert(child->object(), accessibleChild);
        cache->emitAddAccessible(accessibleChild->getCacheItem());

        QSpiAccessible* parentAccessible = qobject_cast<QSpiAccessible*>(cacheObjects_.value(interface->object()));
        Q_ASSERT(parentAccessible);
        if (parentAccessible) {
            QSpiObjectReference r = accessibleChild->getReference();
            QDBusVariant data;
            data.setVariant(QVariant::fromValue(r));
            qDebug() << "sending children changed index !0: " << interface->childCount() << r.path.path();
            parentAccessible->signalChildrenChanged("add", 0, 0, data);
        }
    } else {
        delete child;
    }
}

void QSpiAccessibleBridge::notifyAccessibilityUpdate(int reason, QAccessibleInterface *interface)
{
    QSpiObject *accessible = objectToAccessible(interface->object());

    if (reason == QAccessible::ObjectShow) {
        qDebug() << "Object Show event, notify parent..." << interface->object();
        QAccessibleInterface *parent = 0;
        interface->navigate(QAccessible::Ancestor, 1, &parent);
        if (parent) {
            QSpiObject *parentAccessibleObject = objectToAccessible(parent->object());
            QSpiAccessible* parentAccessible = qobject_cast<QSpiAccessible*>(parentAccessibleObject);
            if (!parentAccessible) {
                qWarning() << "Invalid parent accessible";
                return;
            }

            QSpiObjectReference r = accessible->getReference();
            QDBusVariant data;
            data.setVariant(QVariant::fromValue(r));
            parentAccessible->signalChildrenChanged("add", parent->childCount(), 0, data);
        }
    }

    if (accessible) {
        if (reason == QAccessible::Focus) {
            static QSpiAccessible *lastFocused = 0;
            if (lastFocused) {
                QDBusVariant data;
                data.setVariant(QVariant::fromValue(lastFocused->getReference()));
                qDebug() << "last focus: " << lastFocused;
                emit lastFocused->StateChanged("focused", 0, 0, data, lastFocused->getRootReference());
            }
            lastFocused = qobject_cast<QSpiAccessible*>(accessible);
        }

        accessible->accessibleEvent((QAccessible::Event) reason);
    } else {
        qWarning() << "QSpiAccessibleBridge::notifyAccessibilityUpdate: invalid accessible";
    }
}


void QSpiAccessibleBridge::registerChildren(QAccessibleInterface *interface)
{
    qDebug() << "QSpiAccessibleCache::registerChildren: " << interface->object();

    QAccessibleInterface *current;
    QSpiObject *accessible = 0;
    QStack <QAccessibleInterface *> stack;

    if (interface == NULL || interface->object() == NULL)
        return;

    /* Depth first iteration over all un-registered objects */
    stack.push(interface);
    while (!stack.empty()) {
        current = stack.pop();
        qDebug() << "  child: " << current->object();
        accessible = interfaceToAccessible(current);

        for (int i = 1; i <= current->childCount (); i++) {
            QAccessibleInterface *child = NULL;
            current->navigate(QAccessible::Child, i, &child);
            if (child) {
                stack.push(child);
            }
        }
    }
}

QSpiAdaptor* QSpiAccessibleBridge::objectToAccessible(QObject *object)
{
    if (cacheObjects_.contains(object))
        return cacheObjects_.value(object);

    QAccessibleInterface* interface = QAccessible::queryAccessibleInterface(object);
    if (!interface) {
        qWarning() << "Create accessible for object which cannot create an accessible interface." << object;
        return 0;
    }

    return interfaceToAccessible(interface);
}

QSpiAdaptor* QSpiAccessibleBridge::interfaceToAccessible(QAccessibleInterface *interface)
{
    Q_ASSERT(interface);
    QObject* object = interface->object();

    if (cacheObjects_.contains(object)) {
        return cacheObjects_.value(object);
    }

    QSpiAdaptor *accessible = new QSpiAccessible(interface);
    cacheObjects_.insert(object, accessible);
    cache->emitAddAccessible(accessible->getCacheItem());
    return accessible;
}

