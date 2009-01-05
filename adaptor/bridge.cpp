/*
 * D-Bus AT-SPI, Qt Adaptor
 *
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

#include <qaccessible.h>
#include <qaccessiblebridge.h>

#include "registry-proxy.h"

#define QSPI_REGISTRY_ADDRESS      "org.freedesktop.atspi.Registry"
#define QSPI_REGISTRY_OBJECT_PATH  "/org/freedesktop/atspi/registry"

class QSpiAccessibleBridge: public QAccessibleBridge
{
        Q_OBJECT
public:
        virtual void setRootObject (QAccessibleInterface *obj);
        virtual void notifyAccessibilityUpdate (int reason, QAccessibleInterface *obj, int child);

private:
        QDBusConnection      *bus;
        QAccessibleInterface *root;
        QSpiRegistryProxy    *registry;
};

class QSpiAccessibleBridgePlugin: public QAccessibleBridgePlugin
{
        Q_OBJECT
public:
        QSpiAccessibleBridgePlugin (QObject *parent = NULL);

        virtual QAccessibleBridge *create (const QString &key);
        virtual QStringList keys () const;
};

/*---------------------------------------------------------------------------*/

QSpiAccessibleBridgePlugin::QSpiAccessibleBridgePlugin (QObject *parent)
        :QAccessibleBridgePlugin (parent)
{
        ;
}

QAccessibleBridge *QSpiAccessibleBridgePlugin::create (const QString &name)
{
        if (name == "QDBUSBRIDGE")
                return new QSpiAccessibleBridge ();
        else
                return NULL;
}

QStringList QSpiAccessibleBridgePlugin::keys () const
{
        QStringList l;
        l << "QDBUSBRIDGE";
        return l;
}

Q_EXPORT_PLUGIN(QSpiAccessibleBridgePlugin)

/*---------------------------------------------------------------------------*/

void QSpiAccessibleBridge::setRootObject (QAccessibleInterface *obj)
{
        Q_ASSERT (obj != NULL);
        Q_ASSERT (this->root == NULL);

        root = obj;

        bus = new QDBusConnection (
                        QDBusConnection::connectToBus (QDBusConnection::SessionBus,
                                                       "org.freedesktop.atspi")
                  );

        registry = new QSpiRegistryProxy (QSPI_REGISTRY_ADDRESS,
                                          QSPI_REGISTRY_OBJECT_PATH,
                                          *bus);

        registry->registerApplication (bus->baseService ());
}

void QSpiAccessibleBridge::notifyAccessibilityUpdate (int event, QAccessibleInterface *obj, int child)
{
        ;
}

/*---------------------------------------------------------------------------*/

