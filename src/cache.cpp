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

#include "cache.h"
#include "application.h"
#include "tree_adaptor.h"

#define QSPI_TREE_OBJECT_PATH "/org/freedesktop/atspi/tree"
#define QSPI_OBJECT_PATH_DESKTOP "/root"

/*---------------------------------------------------------------------------*/

QSpiAccessibleCache::QSpiAccessibleCache (QObject *root)
{
    QAccessibleInterface *interface;
    QSpiAccessibleObject *accessible;

    QApplication::instance()->installEventFilter(this);

    this->root = root;
    interface = QAccessible::queryAccessibleInterface (this->root);
    accessible = new QSpiAccessibleApplication (this, interface);
    connect(this->root, SIGNAL(destroyed(QObject *)), this, SLOT(objectDestroyed(QObject *)));
    cache.insert (this->root, accessible);
    emit accessibleAdded (accessible);

    /* Big TODO here - Get hold of the widgets *Somehow* and register them */

    Q_ASSERT (cache.contains (root));

    new QSpiTreeAdaptor (this);
    QDBusConnection::sessionBus().registerObject(QSPI_TREE_OBJECT_PATH, this, QDBusConnection::ExportAdaptors);
}

/*---------------------------------------------------------------------------*/

bool QSpiAccessibleCache::eventFilter(QObject *obj, QEvent *event)
{
    /* Check whether this event is for a registered object or not */
#if 0
    if (cache.contains (obj))
    {
#endif
        switch (event->type ())
        {
            case QEvent::ChildAdded:
            {
                QObject *child = static_cast<QChildEvent *>(event)->child();
                registerConnected (child);
            }
            case QEvent::Create:
                /* Not sure whether the create event should do anything. Just children added right? */
            case QEvent::ChildRemoved:
                /* Child removed shouldn't do anything. Objects are deleted by their destroy */
            case QEvent::Show:
                /* Perhaps there is a show event to emit here. Is it missing from the accessibility updates */
            default:
                break;
        }
#if 0
    }
#endif
    /* Think false is the right thing to do, no need to block these events here. */
    return false;
}

/*---------------------------------------------------------------------------*/

void QSpiAccessibleCache::registerConnected (QObject *object)
{
    QList <QObject*> parents;
    QObject *current;

    current = object;
    while (current)
    {
        parents.insert(0, current);
        current = current->parent();
    }

    foreach (QObject *parent, parents)
    {
        registerChildren (parent);
    }
}

/*---------------------------------------------------------------------------*/

/* TODO - No Idea about the threading implications here.
 * What is the equivalent of the GDK lock?
 */
void QSpiAccessibleCache::registerChildren (QObject *object)
{
    QObject *current;
    QAccessibleInterface *interface = NULL;
    QSpiAccessibleObject *accessible = NULL;
    QStack <QObject *> stack;

#if 0
    if (!object->isWidgetType ())
        return;
#endif

    stack.push (object);

    /* Depth first iteration over all un-registered objects */
    while (!stack.empty())
    {

        current = stack.pop();

        if (cache.contains (current))
            continue;

        /* Create the QSpiAccessibleObject,
         * Connect the destroy signal,
         * Add to the cache,
         * Emit the added signal
         */
        interface = QAccessible::queryAccessibleInterface (current);
        if (!interface)
            continue;

        accessible = new QSpiAccessibleObject (this, interface);
        connect(current, SIGNAL(destroyed(QObject *)), this, SLOT(objectDestroyed(QObject *)));
        cache.insert (current, accessible);
        emit accessibleAdded (accessible);

        foreach (QObject *child, current->findChildren<QObject *>())
        {
            stack.push (child);
        }
    }
}

/*---------------------------------------------------------------------------*/

void QSpiAccessibleCache::objectDestroyed (QObject *obj)
{
    /* Remove from the cache, emit signal, and destroy object */
    if (cache.contains (obj))
    {
        QSpiAccessibleObject *accessible = cache.take (obj);
        emit accessibleDestroyed (accessible);
        delete accessible;
    }
}

/*---------------------------------------------------------------------------*/

QSpiAccessibleObject *QSpiAccessibleCache::lookupObject (QObject *obj)
{
    if (cache.contains(obj))
        return cache.value(obj);
    else
        return NULL;
}

/*---------------------------------------------------------------------------*/

QSpiAccessibleObject *QSpiAccessibleCache::getRoot ()
{
    Q_ASSERT (cache.contains(root));
    return cache.value(root);
}

/*---------------------------------------------------------------------------*/

QList <QSpiAccessibleObject *> QSpiAccessibleCache::listAccessibles ()
{
    return cache.values();
}

/*END------------------------------------------------------------------------*/
