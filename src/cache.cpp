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

#include "cache.h"
#include "object.h"
#include "adaptor.h"
#include "accessible.h"
#include "application.h"

#include "generated/cache_adaptor.h"

#define QSPI_OBJECT_PATH_CACHE "/org/a11y/atspi/cache"


QSpiAccessibleCache::QSpiAccessibleCache(QObject *root, QDBusConnection c)
    : dbusConnection(c)
{
    CacheAdaptor *adaptor;

    rootObject = root;
    cache.insert(root, new QSpiApplication(this, QAccessible::queryAccessibleInterface(root), dbusConnection));
    registerChildren(QAccessible::queryAccessibleInterface(root));

    adaptor = new CacheAdaptor(this);
    c.registerObject(QSPI_OBJECT_PATH_CACHE, this, QDBusConnection::ExportAdaptors);
}


QObject *QSpiAccessibleCache::getRoot ()
{
    return rootObject;
}


/* 
 * Registers all descendants of the given accessible interface with the cache.
 *
 * No Idea about the threading implications here.
 * What is the equivalent of the GDK lock?
 */
void QSpiAccessibleCache::registerChildren(QAccessibleInterface *interface)
{
    qDebug() << "QSpiAccessibleCache::registerChildren";

    QAccessibleInterface *current;
    QSpiObject *accessible = NULL;
    QStack <QAccessibleInterface *> stack;

    if (interface == NULL || interface->object() == NULL)
        return;

    /* Depth first iteration over all un-registered objects */
    stack.push (interface);
    while (!stack.empty())
    {
        current = stack.pop();
        accessible = objectToAccessible(current->object());

        for (int i = 1; i <= current->childCount (); i++)
        {
            QAccessibleInterface *child = NULL;
            current->navigate(QAccessible::Child, i, &child);
            if (child)
            {
                stack.push (child);
            }
        }
    }
}

QSpiObject *QSpiAccessibleCache::objectToAccessible(QObject *obj)
{
    if (cache.contains(obj)) {
        return cache.value(obj);
    } else {
        QAccessibleInterface *interface = QAccessible::queryAccessibleInterface(obj);
        if (interface)
        {
            QSpiObject *accessible = new QSpiAccessible(this, interface, dbusConnection);
            cache.insert(interface->object(), accessible);
            emit AddAccessible(accessible->getCacheItem());
            return accessible;
        }
    }
    return 0;
}


QSpiAccessibleCacheArray QSpiAccessibleCache::GetItems()
{
    QList <QSpiAccessibleCacheItem> cacheArray;

    foreach (QSpiObject *obj, cache.values())
    {
        cacheArray << obj->getCacheItem();
    }
    return cacheArray;
}
