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

#ifndef Q_SPI_APPLICATION_H
#define Q_SPI_APPLICATION_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include <QDBusObjectPath>

#include <QAccessible>
#include <QAccessibleInterface>

#include "object.h"

class QSpiAccessibleCache;

class QSpiAccessibleApplication : public QSpiAccessibleObject
{
    Q_OBJECT
public:
    QSpiAccessibleApplication (QSpiAccessibleCache  *cache,
                               QAccessibleInterface *interface);

    virtual QDBusObjectPath getParentPath () const;
};

#endif /* Q_SPI_APPLICATION_H */
