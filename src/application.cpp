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
#include <QApplication>
#include <QWidget>

#include "application.h"
#include "application_adaptor.h"
#include "cache.h"

#define QSPI_INTERFACE_APPLICATION "org.freedesktop.atspi.Application"
#define QSPI_OBJECT_PATH_DESKTOP "/root"

/*---------------------------------------------------------------------------*/

QSpiAccessibleApplication::QSpiAccessibleApplication (QSpiAccessibleCache *cache,
                                                      QAccessibleInterface *interface)
        : QSpiAccessibleObject (cache, interface)
{
        new QSpiApplicationAdaptor(this);
        supported << QSPI_INTERFACE_APPLICATION;
}

/*---------------------------------------------------------------------------*/

QDBusObjectPath QSpiAccessibleApplication::getParentPath () const
{
    return QDBusObjectPath (QSPI_OBJECT_PATH_DESKTOP);
}

/*---------------------------------------------------------------------------*/
