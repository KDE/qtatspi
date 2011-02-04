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

#include "object.h"
#include "cache.h"
#include "constant_mappings.h"
#include "struct_marshallers.h"

#include "generated/dec_proxy.h"

#define QSPI_DEC_NAME        "/org/a11y/atspi/registry"
#define QSPI_DEC_OBJECT_PATH "/org/a11y/atspi/registry/deviceeventcontroller"

#include "generated/event_adaptor.h"

void QSpiAccessibleBridge::setRootObject(QAccessibleInterface *inter)
{
    qDebug() << "QSpiAccessibleBridge : Initializing bridge";
    /* Connect to the session bus and register with the AT-SPI registry daemon */
    if (!QDBusConnection::sessionBus().isConnected())
    {
        qWarning() << "QSpiAccessibleBridge : Failed to connect to session bus";
        return;
    }

    rootInterface = inter;

    qDebug() << "  got a11y root object. children: " << inter->childCount();

    // FIXME: notify about children here
    //ObjectAdaptor* atspiEvent = new ObjectAdaptor(this);
    //connect(this, SIGNAL(ChildrenChanged()), atspiEvent, SIGNAL(ChildrenChanged()));
    //emit ChildrenChanged();

    qspi_initialize_struct_types();
    qspi_initialize_constant_mappings();

    /* Create the cache of accessible objects */
    cache = new QSpiAccessibleCache(rootInterface->object());

    dec = new DeviceEventControllerProxy(QSPI_DEC_NAME,
                                         QSPI_DEC_OBJECT_PATH,
                                         QDBusConnection::sessionBus());
}

void QSpiAccessibleBridge::notifyAccessibilityUpdate(int reason, QAccessibleInterface *interface, int index)
{
    qDebug() << "QSpi::notifyAccessibilityUpdate " << QString::number(reason, 16) << " obj: " << interface->object()->objectName() << index;

    if (!cache) {
        qWarning("QSpi::notifyAccessibilityUpdate: no cache, returning");
        return; // why?
    }

    if (reason == QAccessible::ObjectShow) {
        qDebug() << "Object Show event";
    }

    QSpiObject *accessible = NULL;

    if (index > 0) {
        QAccessibleInterface *child = NULL;

        interface->navigate(QAccessible::Child, index, &child);
        accessible = cache->objectToAccessible(child->object());
    } else {
        accessible = cache->objectToAccessible(interface->object());
    }
    accessible->accessibleEvent((QAccessible::Event) reason);
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
