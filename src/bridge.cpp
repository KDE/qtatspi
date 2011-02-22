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

#include "accessible.h"
#include "cache.h"
#include "constant_mappings.h"
#include "object.h"
#include "struct_marshallers.h"

#include "generated/dec_proxy.h"
#include "generated/event_adaptor.h"

#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#define QSPI_DEC_NAME        "/org/a11y/atspi/registry"
#define QSPI_DEC_OBJECT_PATH "/org/a11y/atspi/registry/deviceeventcontroller"


QSpiAccessibleBridge::QSpiAccessibleBridge()
    : cache(0), rootInterface(0)
{
    accessibilityDBusAddress = getAccessibilityBusAddress();
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

QDBusConnection QSpiAccessibleBridge::dbusConnection() const
{
    if (!accessibilityDBusAddress.isEmpty()) {
        QDBusConnection c = QDBusConnection::connectToBus(accessibilityDBusAddress, "a11y");
        if (c.isConnected()) {
            qDebug() << "Connected to accessibility bus at: " << accessibilityDBusAddress;
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

void QSpiAccessibleBridge::setRootObject(QAccessibleInterface *inter)
{
    // the interface we get will be for the QApplication object.

    qDebug() << "QSpiAccessibleBridge : Initializing bridge";

    QDBusConnection c = dbusConnection();
    if (!c.isConnected())
        return;

    rootInterface = inter;
    qDebug() << "  got a11y root object. children: " << inter->childCount();

    qspi_initialize_struct_types();
    qSpiInitializeConstantMappings();

    /* Create the cache of accessible objects */
    cache = new QSpiAccessibleCache(rootInterface->object(), c);

    dec = new DeviceEventControllerProxy(QSPI_DEC_NAME,
                                         QSPI_DEC_OBJECT_PATH,
                                         c);
}

#include <QMenuBar>

void QSpiAccessibleBridge::notifyAccessibilityUpdate(int reason, QAccessibleInterface *interface, int index)
{
    if (!cache) {
        qWarning("QSpi::notifyAccessibilityUpdate: no cache, returning");
        return;
    }

    qDebug() << "notifyAccessibilityUpdate" << interface->object() << " i: " << index;

    if (qobject_cast<QMenuBar*>(interface->object())) {
        qDebug() << "got menu";
    }

    QSpiObject *accessible = 0;

    if (index > 0) {
        QAccessibleInterface *child;
        interface->navigate(QAccessible::Child, index, &child);
        if (!child) {
            qWarning() << "WARNING: Implement: QSpiAccessibleBridge::notifyAccessibilityUpdate for child of " << interface->object() << index;
            return;
        }
        accessible = cache->objectToAccessible(child->object());
        delete child;
    } else {

        accessible = cache->objectToAccessible(interface->object());

        if (reason == QAccessible::ObjectShow) {
            qDebug() << "Object Show event, notify parent..." << interface->object();
            QAccessibleInterface *parent = 0;
            interface->navigate(QAccessible::Ancestor, 1, &parent);
            if (parent) {
                QSpiObject *parentAccessibleObject = cache->objectToAccessible(parent->object());
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
    }
    if (accessible) {
        accessible->accessibleEvent((QAccessible::Event) reason);
    } else {
        qWarning() << "QSpiAccessibleBridge::notifyAccessibilityUpdate: invalid accessible";
    }
}

//enum QSpiKeyEventType
//{
//      QSPI_KEY_EVENT_PRESS,
//      QSPI_KEY_EVENT_RELEASE,
//      QSPI_KEY_EVENT_LAST_DEFINED
//};

//bool QSpiAccessibleBridge::eventFilter(QObject *obj, QEvent *event)
//{
//    Q_UNUSED (obj);
//    switch (event->type ())
//    {
//        case QEvent::KeyPress:
//        case QEvent::KeyRelease:
//        {
//            QKeyEvent *key_event = static_cast <QKeyEvent *>(event);
//            QSpiDeviceEvent de;

//            if (event->type() == QEvent::KeyPress)
//                de.type = QSPI_KEY_EVENT_PRESS;
//            else
//                de.type = QSPI_KEY_EVENT_RELEASE;
           
//            de.id = key_event->key();
//            de.hw_code = 0; /* What is a keyval and what is a keycode */
//            /* TODO Insert the keyboard modifiers here */
//            de.modifiers = 0;
//            de.timestamp = QTime::currentTime().elapsed();
//            de.event_string =  key_event->text();
//            /* TODO Work out if there is an easy equivalent of "is_text" */
//            de.is_text = false;

//#if 0
//            qDebug ("QSpiAccessibleBridge : keyEvent.\n\t%s",
//                    qPrintable (de.event_string)
//                   );
//#endif

//            /* TODO Work through the sync issues with key event notifications.
//             * How can we block the events here?
//             */
//            /*return dec->NotifyListenersSync(de);*/
//            return false;
//            break;
//        }
//        default:
//            break;
//    }
//    return false;
//}

QSpiAccessibleBridge::~QSpiAccessibleBridge ()
{
    qDebug() << "QSpiAccessibleBridge::~QSpiAccessibleBridge";
}
