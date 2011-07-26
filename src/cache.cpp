/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2008-2011 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright 2008, 2009      Codethink Ltd.
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

#include "cache.h"
#include "generated/cache_adaptor.h"

#include "bridge.h"

#define QSPI_OBJECT_PATH_CACHE "/org/a11y/atspi/cache"


QSpiDBusCache::QSpiDBusCache(QDBusConnection c, QObject* parent)
    : QObject(parent)
{
    new CacheAdaptor(this);
    c.registerObject(QSPI_OBJECT_PATH_CACHE, this, QDBusConnection::ExportAdaptors);
}

void QSpiDBusCache::emitAddAccessible(const QSpiAccessibleCacheItem& item)
{
    emit AddAccessible(item);
}

void QSpiDBusCache::emitRemoveAccessible(const QSpiObjectReference& item)
{
    emit RemoveAccessible(item);
}

//QSpiAccessibleCacheArray QSpiDBusCache::GetItems()
//{
//    QList <QSpiAccessibleCacheItem> cacheArray;

//    foreach (QSpiAdaptor* obj, spiBridge->cacheObjects()) {
//        cacheArray << obj->getCacheItem();
//    }
//    return cacheArray;
//}
