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

#include "generated/event_adaptor.h"

QSpiObject::QSpiObject(QSpiAccessibleCache  *_cache,
                       QAccessibleInterface *_interface)
    : interface(_interface),
      cache(_cache)
{
}

QSpiObjectReference &QSpiObject::getReference() const
{
    return *reference;
}

QAccessibleInterface &QSpiObject::getInterface() const
{
    return *interface;
}

QStringList QSpiObject::getSupportedInterfaces() const
{
    return supportedInterfaces;
}

QSpiAccessibleCacheItem QSpiObject::getCacheItem() const
{
    QSpiAccessibleCacheItem item;

    QList<QSpiObject *> children;
    QList<QSpiObjectReference> childPaths;

    /* Path */
    item.path = getReference();
    /* Parent */
    item.parent = getParentReference();

    item.application = cache->objectToAccessible(cache->getRoot())->getReference();

    /* Children */
    for (int i = 1; i <= getInterface().childCount(); i++)
    {
        QAccessibleInterface *child = 0;
        getInterface().navigate(QAccessible::Child, i, &child);
        if (child)
        {
            QSpiObject *current;
            current = cache->objectToAccessible(child->object());
            if (current)
                children << current;
            delete child;
        }
    }
    foreach (QSpiObject *obj, children)
    {
        childPaths << obj->getReference();
    }
    item.children = childPaths;
    /* Supported interfaces */
    item.supported = getSupportedInterfaces();
    /* Name */
    item.name = getInterface().text(QAccessible::Name, 0);
    /* Role */
    item.role = qSpiRoleMapping.value(getInterface().role(0)).spiRole();

    /* Description */
    item.description = getInterface().text(QAccessible::Description, 0);
    /* State set */
    item.states = qSpiStatesetFromQState(getInterface().state(0));
    return item;
}

