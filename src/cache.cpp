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

#include "cache.h"
#include "application.h"
#include "tree_adaptor.h"

#define QSPI_TREE_OBJECT_PATH "/org/freedesktop/atspi/tree"
#define QSPI_OBJECT_PATH_DESKTOP "/root"

/*---------------------------------------------------------------------------*/

QSpiAccessibleCache::QSpiAccessibleCache (QObject *root)
{
    QApplication::instance()->installEventFilter(this);

    this->root = root;

    /* Add all the top-level windows */
    registerChildren (QAccessible::queryAccessibleInterface(this->root));

    new QSpiTreeAdaptor (this);
    QDBusConnection::sessionBus().registerObject(QSPI_TREE_OBJECT_PATH, this, QDBusConnection::ExportAdaptors);
}

/*---------------------------------------------------------------------------*/

void QSpiAccessibleCache::updateAccessible (QSpiAccessibleObject *accessible, QAccessible::Event event)
{
    switch (event)
    {
    case QAccessible::DescriptionChanged:
    case QAccessible::NameChanged:
    case QAccessible::ParentChanged:
    case QAccessible::StateChanged:
            emit accessibleUpdated (accessible);
    default:
            break;
    }
}

/*---------------------------------------------------------------------------*/

bool QSpiAccessibleCache::eventFilter(QObject *obj, QEvent *event)
{

    switch (event->type ())
    {
        case QEvent::Show:
        {
            registerConnected (obj);
            break;
        }
#if 0
        case QEvent::ChildAdded:
        {
            QList <QSpiAccessibleObject *> children;
            QObject *child = static_cast<QChildEvent *>(event)->child();

            qDebug ("QSpiAccessibleBridge : childAdded.\n\t%s\n\t%s\n\t%s\n",
                    qPrintable(accessible->getPath().path()),
                    qPrintable(accessible->getParentPath().path()),
                    qPrintable (accessible->getInterface().text(QAccessible::Name, 0))
                   );

            children = accessible->getChildren();
            foreach (QSpiAccessibleObject *chld, children)
            {
                qDebug ("\t\t%s", qPrintable(chld->getPath().path()));
            }
                
            registerChildren (child);
            emit accessibleUpdated (accessible);
            break;
        }
#endif
        case QEvent::ChildRemoved:
        {
            QSpiAccessibleObject *accessible = NULL;

            accessible = lookupObject (obj);
            if (accessible)
            {
#if 0
                qDebug ("QSpiAccessibleBridge : childRemoved.\n\t%s\n",
                        qPrintable(accessible->getPath().path())
                       );
#endif

                emit accessibleUpdated (accessible);
            }
            break;
        }
        default:
            break;
    }
    /* Think false is the right thing to do, no need to block these events here. */
    return false;
}

/*---------------------------------------------------------------------------*/

void QSpiAccessibleCache::registerConnected (QObject *object)
{
    QList <QAccessibleInterface*> parents;
    QAccessibleInterface *current = NULL;

    current = QAccessible::queryAccessibleInterface (object);

    while (current)
    {
        parents.insert(0, current);
        current->navigate(QAccessible::Ancestor, 1, &current);
    }

    foreach (QAccessibleInterface *parent, parents)
    {
        QSpiAccessibleObject *accessible;

        accessible = lookupObject (parent->object());
        if (accessible)
        {
#if 0
            qDebug ("QSpiAccessibleBridge : parentUpdated .\n\t%s\n\t%s\n\t%s\n",
                    qPrintable(accessible->getPath().path()),
                    qPrintable(accessible->getParentPath().path()),
                    qPrintable (accessible->getInterface().text(QAccessible::Name, 0))
                   );
#endif
            emit accessibleUpdated (accessible);
        }

        registerChildren (parent);
    }
}

/*---------------------------------------------------------------------------*/

/* TODO - No Idea about the threading implications here.
 * What is the equivalent of the GDK lock?
 */
void QSpiAccessibleCache::registerChildren (QAccessibleInterface *interface)
{
    QAccessibleInterface *current;
    QSpiAccessibleObject *accessible = NULL;
    QStack <QAccessibleInterface *> stack;

    stack.push (interface);

    /* Depth first iteration over all un-registered objects */
    while (!stack.empty())
    {
        current = stack.pop();

        if (cache.contains (current->object()))
        {
            delete current;
            continue;
        }

        /* Create the QSpiAccessibleObject,
         * Connect the destroy signal,
         * Add to the cache,
         * Emit the added signal
         */

        /* Replace with factory method based on role? */
        if (current->object() == this->root)
            accessible = new QSpiAccessibleApplication (this, current);
        else
            accessible = new QSpiAccessibleObject (this, current);
        
        connect(current->object(), SIGNAL(destroyed(QObject *)), this, SLOT(objectDestroyed(QObject *)));
        cache.insert (current->object(), accessible);

#if 0
        qDebug ("QSpiAccessibleBridge : accessibleRegistered.\n\t%s\n\t%s\n\t%s\n",
                 qPrintable(accessible->getPath().path()),
                 qPrintable(accessible->getParentPath().path()),
                 qPrintable (accessible->getInterface().text(QAccessible::Name, 0))
               );
#endif

        emit accessibleUpdated (accessible);

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
