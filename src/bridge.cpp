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

#include "object.h"
#include "cache.h"
#include "constant_mappings.h"
#include "struct_marshallers.h"

#include "generated/dec_proxy.h"

#define QSPI_DEC_NAME        "/org/a11y/atspi/registry"
#define QSPI_DEC_OBJECT_PATH "/org/a11y/atspi/registry/deviceeventcontroller"

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
        DECProxy             *dec;
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
        ;
}

void QSpiAccessibleBridge::setRootObject (QAccessibleInterface *rootInterface)
{
        QDBusError error;

        qDebug ("QSpiAccessibleBridge : Initializing bridge");

        qspi_initialize_struct_types ();
        qspi_initialize_constant_mappings ();

        /* Create the cache of accessible objects */
        cache = new QSpiAccessibleCache (rootInterface->object());

        /* Connect to the session bus and register with the AT-SPI registry daemon */
        if (!QDBusConnection::sessionBus().isConnected())
        {
            qDebug ("QSpiAccessibleBridge : Failed to connect to session bus");
            return;
        }

        dec = new DECProxy (QSPI_DEC_NAME,
                            QSPI_DEC_OBJECT_PATH,
                            QDBusConnection::sessionBus());

        /* Register for application events to handle key events */
        /* TODO, should this be registered on the root object? */
        QApplication::instance()->installEventFilter(this);

        /* Connect to the applications about-to-quit signal for de-registering this app */
        connect (QApplication::instance(), SIGNAL (aboutToQuit()),
                 this, SLOT (aboutToQuit()));

}

void QSpiAccessibleBridge::notifyAccessibilityUpdate (int reason, QAccessibleInterface *interface, int index)
{
        QSpiObject *accessible = NULL;

        if (!cache)
                return;

        if (index > 0)
        {
                QAccessibleInterface *child = NULL;

                interface->navigate(QAccessible::Child, index, &child);
                accessible = cache->objectToAccessible (child->object());
        }
        else
        {
                accessible = cache->objectToAccessible (interface->object());
        }
	accessible->accessibleEvent ((QAccessible::Event) reason);
}

enum QSpiKeyEventType
{
      QSPI_KEY_EVENT_PRESS,
      QSPI_KEY_EVENT_RELEASE,
      QSPI_KEY_EVENT_LAST_DEFINED
};

bool QSpiAccessibleBridge::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED (obj);
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

#if 0
            qDebug ("QSpiAccessibleBridge : keyEvent.\n\t%s",
                    qPrintable (de.event_string)
                   );
#endif

            /* TODO Work through the sync issues with key event notifications.
             * How can we block the events here?
             */
            /*return this->dec->NotifyListenersSync(de);*/
            return false;
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
