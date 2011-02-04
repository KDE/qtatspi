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

/* QSpiObject ------------------------------------------------------*/

QSpiObject::QSpiObject(QSpiAccessibleCache  *_cache,
                        QAccessibleInterface *_interface)
    :interface(_interface), cache(_cache)
{
    new ObjectAdaptor(this);
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
    QList <QSpiObjectReference> childPaths;

    /* Path */
    item.path = this->getReference();
    /* Parent */
    item.parent = this->getParentReference();
    /* Children */
    for (int i = 1; i <= this->getInterface().childCount(); i++)
    {
        QAccessibleInterface *child = NULL;
        this->getInterface().navigate(QAccessible::Child, i, &child);
        if (child)
        {
            QSpiObject *current;
            current = cache->objectToAccessible (child->object ());
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
    item.name = this->getInterface().text(QAccessible::Name, 0);
    /* Role */
    item.role = qSpiRoleMapping.value(this->getInterface().role(0));
    /* Description */
    item.description = this->getInterface().text(QAccessible::Description, 0);
    /* State set */
    qspi_stateset_from_qstate (this->getInterface().state(0), item.states);
    return item;
}

void QSpiObject::accessibleEvent(QAccessible::Event event)
{
    /* TODO Create an event type and emit the event on the object */
    switch (event)
    {
    case QAccessible::ObjectShow:
        qDebug() << "ObjectShow";
        // send signal ChildrenChanged
        break;
    case QAccessible::ObjectHide:
        break;
    case QAccessible::DescriptionChanged:
    case QAccessible::NameChanged:
    case QAccessible::ParentChanged:
    case QAccessible::StateChanged:
    default:
        break;
    }
}

