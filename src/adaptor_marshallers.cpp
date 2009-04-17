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

#include "adaptor_marshallers.h"

/* QSpiAccessibleCacheArray */
/*---------------------------------------------------------------------------*/

QDBusArgument &operator<<(QDBusArgument &argument, const QSpiAccessibleCacheItem &item)
{
        argument.beginStructure();
        argument << item.path;
        argument << item.parent;
        argument << item.children;
        argument << item.supported;
        argument << item.name;
        argument << item.role;
        argument << item.description;
        argument << item.states;
        argument.endStructure();
        return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiAccessibleCacheItem &item)
{
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
        return argument;
}

/*---------------------------------------------------------------------------*/

void qspi_initialize_adaptor_types ()
{
        qDBusRegisterMetaType<QSpiStateSet>();
        qDBusRegisterMetaType<QSpiAccessibleCacheItem>();
        qDBusRegisterMetaType<QSpiAccessibleCacheArray>();
        qDBusRegisterMetaType<QSpiRect>();
}

/*END------------------------------------------------------------------------*/
