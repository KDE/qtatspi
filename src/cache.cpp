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

#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QWidgetList>
#include <QApplication>
#include <QStackedWidget>

#include "bridge.h"
#include "cache.h"
#include "object.h"
#include "adaptor.h"
#include "accessible.h"
#include "application.h"

#include "generated/cache_adaptor.h"

#define QSPI_OBJECT_PATH_CACHE "/org/a11y/atspi/cache"


QSpiDBusCache::QSpiDBusCache(QObject* parent)
    : QObject(parent)
{
    CacheAdaptor *adaptor;
    adaptor = new CacheAdaptor(this);
    QDBusConnection c = QSpiAccessibleBridge::instance()->dBusConnection();
    c.registerObject(QSPI_OBJECT_PATH_CACHE, this, QDBusConnection::ExportAdaptors);
}

void QSpiDBusCache::emitAddAccessible(const QSpiAccessibleCacheItem& item)
{
    emit AddAccessible(item);
}

QSpiAccessibleCacheArray QSpiDBusCache::GetItems()
{
    QList <QSpiAccessibleCacheItem> cacheArray;

    foreach (QSpiObject *obj, spiBridge->cacheObjects())
    {
        cacheArray << obj->getCacheItem();
    }
    return cacheArray;
}
