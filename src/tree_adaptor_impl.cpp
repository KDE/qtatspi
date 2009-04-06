/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2008, 2009      Nokia.
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

#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

#include "tree_adaptor.h"
#include "cache.h"
#include "object.h"
#include "constant_mappings.h"

/*---------------------------------------------------------------------------*/

static QSpiAccessibleCacheItem getCacheItem (const QSpiAccessibleObject &obj) {
        QSpiAccessibleCacheItem item;
        QList <QSpiAccessibleObject *> children;
        QList <QDBusObjectPath> childPaths;

        /* Path */
        item.path = obj.getPath();
        /* Parent */
        item.parent = obj.getParentPath();
        /* Children */
        children = obj.getChildren();
        foreach (QSpiAccessibleObject *obj, children)
        {
            childPaths << obj->getPath();
        }
        item.children = childPaths;
        /* Supported interfaces */
        item.supported = obj.getSupported();
        /* Name */
        item.name = obj.getInterface().text(QAccessible::Name, 0);
        /* Role */
        item.role = qSpiRoleMapping.value(obj.getInterface().role(0));
        /* Description */
        item.description = obj.getInterface().text(QAccessible::Description, 0);
        /* State set */
        qspi_stateset_from_qstate (obj.getInterface().state(0), item.states);
        return item;
}


/*
 * Implementation of adaptor class QSpiTreeAdaptor
 */

QSpiTreeAdaptor::QSpiTreeAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

QSpiTreeAdaptor::~QSpiTreeAdaptor()
{
    // destructor
}

QDBusObjectPath QSpiTreeAdaptor::getRoot()
{
    // handle method call org.freedesktop.atspi.Tree.getRoot
    return static_cast <QSpiAccessibleCache *> (parent())->getRoot()->getPath();
}

QSpiAccessibleCacheArray QSpiTreeAdaptor::getTree()
{
    // handle method call org.freedesktop.atspi.Tree.getTree
    QList <QSpiAccessibleCacheItem> cacheArray;
    QList <QSpiAccessibleObject *> accessibles =
        static_cast <QSpiAccessibleCache *> (parent())->listAccessibles ();
    foreach (QSpiAccessibleObject *obj, accessibles)
    {
        cacheArray << getCacheItem (*obj);
    }
    return cacheArray;
}
