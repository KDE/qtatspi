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

#include "cache.h"
#include "registry_proxy.h"
#include "constant_mappings.h"

#define QSPI_REGISTRY_ADDRESS      "org.freedesktop.atspi.Registry"
#define QSPI_REGISTRY_OBJECT_PATH  "/org/freedesktop/atspi/registry"

class QSpiAccessibleBridge: public QObject, public QAccessibleBridge
{
        Q_OBJECT
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
};

/*---------------------------------------------------------------------------*/

class QDBusAccessibleBridgePlugin: public QAccessibleBridgePlugin
{
        Q_OBJECT
public:
        QDBusAccessibleBridgePlugin(QObject *parent = 0);

        virtual QAccessibleBridge* create(const QString &);
        virtual QStringList keys() const;
};

/*---------------------------------------------------------------------------*/

QDBusAccessibleBridgePlugin::QDBusAccessibleBridgePlugin(QObject *parent)
: QAccessibleBridgePlugin(parent)
{
        ;
}

QAccessibleBridge* QDBusAccessibleBridgePlugin::create(const QString &name)
{
        if (name == "QSPIACESSIBLEBRIDGE")
                return new QSpiAccessibleBridge();
        return 0;
}

QStringList QDBusAccessibleBridgePlugin::keys() const
{
        QStringList l;
        l  << "QSPIACCESSIBLEBRIDGE";
        return l;
}

Q_EXPORT_PLUGIN(QDBusAccessibleBridgePlugin)

/*---------------------------------------------------------------------------*/

void QSpiAccessibleBridge::aboutToQuit ()
{
        if (registry != NULL)
        {
            registry->deregisterApplication (QDBusConnection::sessionBus().baseService ());
        }
}

void QSpiAccessibleBridge::setRootObject (QAccessibleInterface *useless)
{
        QDBusError error;

        qDebug ("QSpiAccessibleBridge : Initializing bridge");

        /* Create the cache of accessible objects */
        cache = new QSpiAccessibleCache (QApplication::instance());

        /* Connect to the session bus and register with the AT-SPI registry daemon */
        if (!QDBusConnection::sessionBus().isConnected())
        {
            qDebug ("QSpiAccessibleBridge : Failed to connect to session bus");
            return;
        }

        registry = new QSpiRegistryProxy (QSPI_REGISTRY_ADDRESS,
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

        /* Connect to the applications about-to-quit signal for de-registering this app */
        connect (QApplication::instance(), SIGNAL (aboutToQuit()),
                 this, SLOT (aboutToQuit()));

        /* Initialize maps of QAccessible <-> AT-SPI constants */
        qspi_initialize_constant_mappings ();
}

void QSpiAccessibleBridge::notifyAccessibilityUpdate (int event, QAccessibleInterface *obj, int child)
{
        /* Needs to emit AT-SPI events based on the updates */
}

QSpiAccessibleBridge::~QSpiAccessibleBridge ()
{
        /* As far as I can tell this destructor is not called on program termination */
}

#include "bridge.moc"

/*---------------------------------------------------------------------------*/
