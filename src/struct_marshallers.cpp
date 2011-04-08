/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2008, 2009      Nokia.
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

#include <QtDBus>

#include "struct_marshallers.h"
#include "bridge.h"

#define QSPI_OBJECT_PATH_PREFIX  "/org/a11y/atspi/accessible/"
#define QSPI_OBJECT_PATH_NULL    QSPI_OBJECT_PATH_PREFIX "null"

QSpiObjectReference::QSpiObjectReference()
    : path(QDBusObjectPath(QSPI_OBJECT_PATH_NULL))
{}

/* QSpiAccessibleCacheArray */
/*---------------------------------------------------------------------------*/

QDBusArgument &operator<<(QDBusArgument &argument, const QSpiAccessibleCacheItem &item)
{
    argument.beginStructure();
    argument << item.path;
    argument << item.application;
    argument << item.parent;
    argument << item.children;
    argument << item.supportedInterfaces;
    argument << item.name;
    argument << item.role;
    argument << item.description;
    argument << item.state;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiAccessibleCacheItem &item)
{
    argument.beginStructure();
    argument >> item.path;
    argument >> item.application;
    argument >> item.parent;
    argument >> item.children;
    argument >> item.supportedInterfaces;
    argument >> item.name;
    argument >> item.role;
    argument >> item.description;
    argument >> item.state;
    argument.endStructure();
    return argument;
}

/* QSpiObjectReference */
/*---------------------------------------------------------------------------*/

QDBusArgument &operator<<(QDBusArgument &argument, const QSpiObjectReference &address)
{
    argument.beginStructure();
    argument << address.service;
    argument << address.path;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiObjectReference &address)
{
    argument.beginStructure();
    argument >> address.service;
    argument >> address.path;
    argument.endStructure();
    return argument;
}

/* QSpiAction */
/*---------------------------------------------------------------------------*/

QDBusArgument &operator<<(QDBusArgument &argument, const QSpiAction &action)
{
    argument.beginStructure();
    argument << action.name;
    argument << action.description;
    argument << action.keyBinding;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiAction &action)
{
    Q_UNUSED(action)
    qWarning() << "QDBusArgument &operator>>(const QDBusArgument &argument, QSpiAction &action): implement me!";
    return argument;
}

/* QSpiRect */
/*---------------------------------------------------------------------------*/

QDBusArgument &operator<<(QDBusArgument &argument, const QSpiRect &rect)
{
    argument.beginStructure();
    argument << rect.x;
    argument << rect.y;
    argument << rect.width;
    argument << rect.height;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiRect &rect)
{
    Q_UNUSED(rect)
    qWarning() << "QDBusArgument &operator>>(const QDBusArgument &argument, QSpiRect &rect): implement me!";
    return argument;
}

/* QSpiAppUpdate */
/*---------------------------------------------------------------------------*/

QDBusArgument &operator<<(QDBusArgument &argument, const QSpiAppUpdate &update) {
    argument.beginStructure();
    argument << update.type << update.address;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiAppUpdate &update) {
    argument.beginStructure();
    argument >> update.type >> update.address;
    argument.endStructure();
    return argument;
}

/* QSpiDeviceEvent */
/*---------------------------------------------------------------------------*/

QDBusArgument &operator<<(QDBusArgument &argument, const QSpiDeviceEvent &event) {
    argument.beginStructure();
    argument << event.type
             << event.id
             << event.hardwareCode
             << event.modifiers
             << event.timestamp
             << event.text
             << event.isText;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiDeviceEvent &event) {
    argument.beginStructure();
    argument >> event.type
             >> event.id
             >> event.hardwareCode
             >> event.modifiers
             >> event.timestamp
             >> event.text
             >> event.isText;
    argument.endStructure();
    return argument;
}

/*---------------------------------------------------------------------------*/

void qSpiInitializeStructTypes ()
{
    qDBusRegisterMetaType<QSpiIntList>();
    qDBusRegisterMetaType<QSpiAccessibleCacheItem>();
    qDBusRegisterMetaType<QSpiAccessibleCacheArray>();
    qDBusRegisterMetaType<QSpiObjectReference>();
    qDBusRegisterMetaType<QSpiObjectReferenceArray>();
    qDBusRegisterMetaType<QSpiRect>();
    qDBusRegisterMetaType<QSpiAttributeSet>();
    qDBusRegisterMetaType<QSpiAction>();
    qDBusRegisterMetaType<QSpiActionArray>();
    qDBusRegisterMetaType<QSpiDeviceEvent>();
    qDBusRegisterMetaType<QSpiAppUpdate>();
//    qDBusRegisterMetaType<QSpiRelationArray>();
}

/*END------------------------------------------------------------------------*/
