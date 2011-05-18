/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2008-2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef Q_SPI_CACHE_H
#define Q_SPI_CACHE_H

#include <QtCore/QObject>
#include "struct_marshallers.h"

/** This class is responsible for the AT-SPI cache interface.
  *
  * The idea behind the cache is that starting an application would
  * result in many dbus calls. The way GTK/Gail/ATK work is that
  * they create accessibles for all objects on startup.
  * In order to avoid querying all the objects individually via DBus
  * they get sent by using the GetItems call of the cache.
  *
  * Additionally the AddAccessible and RemoveAccessible signals
  * are responsible for adding/removing objects from the cache.
  */
class QSpiDBusCache : public QObject
{
    Q_OBJECT

public:
    QSpiDBusCache(QDBusConnection c, QObject* parent = 0);
    void emitAddAccessible(const QSpiAccessibleCacheItem& item);

signals:
    void AddAccessible(const QSpiAccessibleCacheItem &nodeAdded);
    void RemoveAccessible(const QSpiObjectReference &nodeRemoved);

public Q_SLOTS:
    QSpiAccessibleCacheArray GetItems();
};

#endif /* Q_SPI_CACHE_H */
