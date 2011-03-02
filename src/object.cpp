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

#include "adaptor.h"
#include "bridge.h"
#include "constant_mappings.h"
#include "struct_marshallers.h"

#include "generated/event_adaptor.h"

QSpiObject::QSpiObject(QAccessibleInterface *_interface, int child)
    : interface(_interface), child(child)
{
}

QSpiObjectReference &QSpiObject::getReference() const
{
    return *reference;
}

QStringList QSpiObject::getSupportedInterfaces() const
{
    return supportedInterfaces;
}

QSpiAccessibleCacheItem QSpiObject::getCacheItem() const
{
    Q_ASSERT(interface);
    if (!interface->isValid()) {
        qWarning() << "QSpiObject::getCacheItem: invalid interface" << reference->path.path();
        return QSpiAccessibleCacheItem();
    }

    QSpiAccessibleCacheItem item;
    item.path = getReference();
    item.parent = getParentReference();
    item.application = spiBridge->getRootReference();

    /* Children */
    QList<QSpiAdaptor *> children;
    QList<QSpiObjectReference> childPaths;
    for (int i = 1; i <= interface->childCount(); i++) {
        QAccessibleInterface *child = 0;
        interface->navigate(QAccessible::Child, i, &child);
        if (child && child->isValid() && child->object()) {
            // FIXME: interfaces without QObject?
            QSpiAdaptor* current = spiBridge->objectToAccessible(child->object());
            if (current)
                children << current;
            delete child;
        }
    }
    foreach (QSpiAdaptor *obj, children)
    {
        childPaths << obj->getReference();
    }
    item.children = childPaths;

    item.supportedInterfaces = getSupportedInterfaces();
    item.name = interface->text(QAccessible::Name, child);
    item.role = qSpiRoleMapping.value(interface->role(child)).spiRole();
    item.description = interface->text(QAccessible::Description, child);
    item.states = qSpiStatesetFromQState(interface->state(child));
    return item;
}
