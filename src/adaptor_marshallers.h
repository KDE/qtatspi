/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2008, 2009      Nokia Corporation
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

/*
 * This file contains marshallers for types used in the accessible adaptors.
 */

#ifndef Q_SPI_ADAPTOR_MARSHALLERS_H
#define Q_SPI_ADAPTOR_MARSHALLERS_H


/* QSpiAccessibleCacheArray */
/*---------------------------------------------------------------------------*/

#include <QList>

#include "object.h"

typedef QList <QSpiAccessibleObject *> QSpiAccessibleCacheArray;

QDBusArgument &operator<<(QDBusArgument &argument, const QSpiAccessibleObject &update);
const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiAccessibleObject &update);

/* QSpiRelationArray TODO */
/*---------------------------------------------------------------------------*/

typedef int QSpiRelationArray;

/* QSpiStateSet */
/*---------------------------------------------------------------------------*/

struct QSpiStateSet
{
        int one;
        int two;
};

QDBusArgument &operator<<(QDBusArgument &argument, const QSpiStateSet &set);
const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiStateSet &set);

/*---------------------------------------------------------------------------*/

#endif /* Q_SPI_ADAPTOR_MARSHALLERS_H */
