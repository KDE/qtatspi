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

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include <QtDBus/QtDBus>

#include <QApplication>
#include <QAccessible>
#include <QAccessibleBridge>
#include <QTime>

#include "cache.h"
#include "registry_proxy.h"
#include "device_event_controller_proxy.h"
#include "constant_mappings.h"
#include "adaptor_marshallers.h"

#define QSPI_REGISTRY_ADDRESS      "org.freedesktop.atspi.Registry"
#define QSPI_REGISTRY_OBJECT_PATH  "/org/freedesktop/atspi/registry"
#define QSPI_DEC_OBJECT_PATH       "/org/freedesktop/atspi/registry/deviceeventcontroller"

class QSpiAccessibleBridge: public QObject, public QAccessibleBridge
{
        Q_OBJECT

        bool eventFilter(QObject *obj, QEvent *event);
public:
        QSpiAccessibleBridge () {cache=NULL;}
        virtual ~QSpiAccessibleBridge ();
        virtual void setRootObject (QAccessibleInterface *obj);
        virtual void notifyAccessibilityUpdate (int reason, QAccessibleInterface *obj, int child);

public slots:
        void aboutToQuit ();

private:
        QSpiAccessibleCache  *cache;
        QSpiRegistryProxy    *registry;
        QSpiDECProxy         *dec;
};

/*---------------------------------------------------------------------------*/

class QSpiAccessibleBridgePlugin: public QAccessibleBridgePlugin
{
        Q_OBJECT
public:
        QSpiAccessibleBridgePlugin(QObject *parent = 0);

        virtual QAccessibleBridge* create(const QString &);
        virtual QStringList keys() const;
};

/*---------------------------------------------------------------------------*/

QSpiAccessibleBridgePlugin::QSpiAccessibleBridgePlugin(QObject *parent)
: QAccessibleBridgePlugin(parent)
{
        ;
}

QAccessibleBridge* QSpiAccessibleBridgePlugin::create(const QString &name)
{
        if (name == "QSPIACCESSIBLEBRIDGE")
                return new QSpiAccessibleBridge();
        return 0;
}

QStringList QSpiAccessibleBridgePlugin::keys() const
{
        QStringList l;
        l  << "QSPIACCESSIBLEBRIDGE";
        return l;
}

Q_EXPORT_PLUGIN(QSpiAccessibleBridgePlugin)

/*---------------------------------------------------------------------------*/

void QSpiAccessibleBridge::aboutToQuit ()
{
        if (registry != NULL)
        {
            registry->deregisterApplication (QDBusConnection::sessionBus().baseService ());
        }
}

void QSpiAccessibleBridge::setRootObject (QAccessibleInterface *rootInterface)
{
        QDBusError error;

        qDebug ("QSpiAccessibleBridge : Initializing bridge");

        qspi_initialize_adaptor_types ();

        /* Create the cache of accessible objects */
        cache = new QSpiAccessibleCache (rootInterface->object());

        /* Connect to the session bus and register with the AT-SPI registry daemon */
        if (!QDBusConnection::sessionBus().isConnected())
        {
            qDebug ("QSpiAccessibleBridge : Failed to connect to session bus");
            return;
        }

        registry = new QSpiRegistryProxy (QSPI_REGISTRY_ADDRESS,
                                          QSPI_REGISTRY_OBJECT_PATH,
                                          QDBusConnection::sessionBus());

        dec = new QSpiDECProxy (QSPI_REGISTRY_ADDRESS,
                                QSPI_REGISTRY_OBJECT_PATH,
                                QDBusConnection::sessionBus());

        if (!registry->isValid ())
        {
            qDebug ("QSpiAccessibleBridge : Could not contact accessibility registry");
            return;
        }

        registry->registerApplication (QDBusConnection::sessionBus().baseService ());
        error = registry->lastError ();
        if (error.type() != QDBusError::NoError)
        {
            qDebug ("QSpiAccessibleBridge : Could not register with accessibility registry.\n \"%s:%s:%s\"",
                    qPrintable (error.errorString(error.type())),
                    qPrintable (error.name()),
                    qPrintable (error.message()));
        }

        /* Register for application events to handle key events */
        /* TODO, should this be registered on the root object? */
        QApplication::instance()->installEventFilter(this);

        /* Connect to the applications about-to-quit signal for de-registering this app */
        connect (QApplication::instance(), SIGNAL (aboutToQuit()),
                 this, SLOT (aboutToQuit()));

        /* Initialize maps of QAccessible <-> AT-SPI constants */
        qspi_initialize_constant_mappings ();
}

void QSpiAccessibleBridge::notifyAccessibilityUpdate (int reason, QAccessibleInterface *interface, int index)
{
        QSpiAccessibleObject *accessible = NULL;

        if (!cache)
                return;

        if (index > 0)
        {
                QAccessibleInterface *child = NULL;

                interface->navigate(QAccessible::Child, index, &child);
                accessible = cache->lookupObject (child->object());
        }
        else
        {
                accessible = cache->lookupObject (interface->object());
        }
}

enum QSpiKeyEventType
{
      QSPI_KEY_EVENT_PRESS,
      QSPI_KEY_EVENT_RELEASE,
      QSPI_KEY_EVENT_LAST_DEFINED
};

bool QSpiAccessibleBridge::eventFilter(QObject *obj, QEvent *event)
{

    switch (event->type ())
    {
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        {
            QKeyEvent *key_event = static_cast <QKeyEvent *>(event);
            QSpiDeviceEvent de;

            if (event->type() == QEvent::KeyPress)
                de.type = QSPI_KEY_EVENT_PRESS;
            else
                de.type = QSPI_KEY_EVENT_RELEASE;
           
            de.id = key_event->key();
            de.hw_code = 0; /* What is a keyval and what is a keycode */
            /* TODO Insert the keyboard modifiers here */
            de.modifiers = 0;
            de.timestamp = QTime::currentTime().elapsed();
            de.event_string =  key_event->text();
            /* TODO Work out if there is an easy equivalent of "is_text" */
            de.is_text = false; 
            break;
        }
        default:
            break;
    }
    return false;
}

QSpiAccessibleBridge::~QSpiAccessibleBridge ()
{
        /* As far as I can tell this destructor is not called on program termination */
}

#include "bridge.moc"

/*---------------------------------------------------------------------------*/
