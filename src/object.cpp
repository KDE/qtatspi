/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2009      Nokia.
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

#include <QDBusObjectPath>

#include "cache.h"
#include "object.h"
#include "constant_mappings.h"
#include "adaptor_marshallers.h"
#include "accessible_adaptor.h"

#define QSPI_INTERFACE_ACCESSIBLE "org.freedesktop.atspi.Accessible"
#define QSPI_INTERFACE_ACTION "org.freedesktop.atspi.Action"
#define QSPI_INTERFACE_APPLICATION "org.freedesktop.atspi.Application"
#define QSPI_INTERFACE_COLLECTION "org.freedesktop.atspi.Collection"
#define QSPI_INTERFACE_COMPONENT "org.freedesktop.atspi.Component"
#define QSPI_INTERFACE_DEC "org.freedesktop.atspi.DeviceEventController"
#define QSPI_INTERFACE_DEVICE_EVENT_LISTENER "org.freedesktop.atspi.DeviceEventListener"
#define QSPI_INTERFACE_DESKTOP "org.freedesktop.atspi.Desktop"
#define QSPI_INTERFACE_DOCUMENT "org.freedesktop.atspi.Document"
#define QSPI_INTERFACE_EDITABLE_TEXT "org.freedesktop.atspi.EditableText"
#define QSPI_INTERFACE_EVENT_KEYBOARD "org.freedesktop.atspi.Event.Keyboard"
#define QSPI_INTERFACE_EVENT_MOUSE "org.freedesktop.atspi.Event.Mouse"
#define QSPI_INTERFACE_HYPERLINK "org.freedesktop.atspi.Hyperlink"
#define QSPI_INTERFACE_HYPERTEXT "org.freedesktop.atspi.Hypertext"
#define QSPI_INTERFACE_IMAGE "org.freedesktop.atspi.Image"
#define QSPI_INTERFACE_REGISTRY "org.freedesktop.atspi.Registry"
#define QSPI_INTERFACE_SELECTION "org.freedesktop.atspi.Selection"
#define QSPI_INTERFACE_TABLE "org.freedesktop.atspi.Table"
#define QSPI_INTERFACE_TEXT "org.freedesktop.atspi.Text"
#define QSPI_INTERFACE_TREE "org.freedesktop.atspi.Tree"
#define QSPI_INTERFACE_VALUE "org.freedesktop.atspi.Value"

#define QSPI_OBJECT_PATH_PREFIX  "/org/freedesktop/atspi/accessible/"
#define QSPI_OBJECT_PATH_NULL    "/"

/*---------------------------------------------------------------------------*/

QSpiAccessibleObject::QSpiAccessibleObject (QSpiAccessibleCache  *cache,
                                            QAccessibleInterface *interface)
{
    this->cache     = cache;
    this->interface = interface;
    this->path  = getUnique ();

    /* It should be reasonable to get the interface from the adaptor here.
     * But the CLASS_INFO key is a private definition within the QtDBus bindings
     * so I'm presuming its not a public field of the adaptor objects.
     */

    new QSpiAccessibleAdaptor(this);
    supported << QSPI_INTERFACE_ACCESSIBLE;

    if (interface->textInterface())
    {
        //new AccessibleTextAdaptor(this);
        //supported << QSPI_INTERFACE_TEXT;
    }
    if (interface->editableTextInterface())
    {
        //new AccessibleEditableTextAdaptor(this);
        //supported << QSPI_INTERFACE_EDITABLE_TEXT;
    }
    if (interface->valueInterface())
    {
        //new AccessibleValueAdaptor(this);
        //supported << QSPI_INTERFACE_VALUE;
    }
    if (interface->object()->isWidgetType())
    {
        //new AccessibleComponentAdaptor(this);
        //new AccessibleActionAdaptor(this);
        //supported << QSPI_INTERFACE_COMPONENT;
        //supported << QSPI_INTERFACE_ACTION;
    }
    if (interface->tableInterface())
    {
        //new AccessibleTableAdaptor(this);
        //supported << QSPI_INTERFACE_TABLE;
    }

    QDBusConnection::sessionBus().registerObject(this->path.path(), this, QDBusConnection::ExportAdaptors);
}

/*---------------------------------------------------------------------------*/

QAccessibleInterface &QSpiAccessibleObject::getInterface () const
{
    return *interface;
}

/*---------------------------------------------------------------------------*/

QDBusObjectPath QSpiAccessibleObject::getPath () const
{
    return path;
}

/*---------------------------------------------------------------------------*/

QStringList QSpiAccessibleObject::getSupported () const
{
    return supported;
}

/*---------------------------------------------------------------------------*/

QDBusObjectPath QSpiAccessibleObject::getParentPath () const
{
    QSpiAccessibleObject *parent = cache->lookupObject (interface->object()->parent());
    if (parent)
    {
       return parent->getPath();
    }
    else
    {
       return getApplication()->getPath();
    }
}

/*---------------------------------------------------------------------------*/

QList <QSpiAccessibleObject *> QSpiAccessibleObject::getChildren () const
{
    QList<QSpiAccessibleObject *> children;
    QList<QObject *> widgets = interface->object()->findChildren<QObject *>();

    foreach (QObject *obj, widgets)
    {
#if 0
       if (obj->isWidgetType())
       {
#endif
           QSpiAccessibleObject *current;
           current = cache->lookupObject (obj);
           if (current)
               children << current;
#if 0
       }
#endif
    }
    return children;
}

/*---------------------------------------------------------------------------*/

QSpiAccessibleObject *QSpiAccessibleObject::getApplication () const
{
    return cache->getRoot();
}

/*---------------------------------------------------------------------------*/

QDBusObjectPath QSpiAccessibleObject::getUnique ()
{
    static int id = 1;
    QString prefix (QSPI_OBJECT_PATH_PREFIX);
    QString num;

    if (id == 0)
       id++;
    return QDBusObjectPath(prefix + num.setNum(id++));
}

/*END------------------------------------------------------------------------*/
