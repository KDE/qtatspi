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

#include "generated/cache_adaptor.h"

#define QSPI_OBJECT_PATH_CACHE "/org/a11y/atspi/cache"

/*---------------------------------------------------------------------------*/

QSpiAccessibleCache::QSpiAccessibleCache(QObject *root, QDBusConnection c)
    : dbusConnection(c)
{
    CacheAdaptor *adaptor;

    //QApplication::instance()->installEventFilter(this);

    rootObject = root;
    cache.insert(root, new QSpiApplication(this, QAccessible::queryAccessibleInterface(root), dbusConnection));
    registerChildren(QAccessible::queryAccessibleInterface(root));

    adaptor = new CacheAdaptor(this);
    c.registerObject(QSPI_OBJECT_PATH_CACHE, this, QDBusConnection::ExportAdaptors);
}

/*---------------------------------------------------------------------------*/

QObject *QSpiAccessibleCache::getRoot ()
{
    return rootObject;
}

/*---------------------------------------------------------------------------*/

//bool QSpiAccessibleCache::eventFilter(QObject *obj, QEvent *event)
//{
//    QSpiObject *accessible = NULL;

//    switch (event->type ())
//    {
//        case QEvent::Show:
//        {
//            accessible = objectToAccessible (obj);
//            break;
//        }
//        /*
//         * FIXME: No Idea what to do here, the ChildAdded signal is BROKEN
//         * for accessibility as the accessible interface is not ready when this signal
//         * is emmited.
//         */
//#if 0
//        case QEvent::ChildAdded:
//        {
//            QList <QSpiObject *> children;
//            QObject *child = static_cast<QChildEvent *>(event)->child();

//            qDebug ("QSpiAccessibleBridge : childAdded.\n\t%s\n\t%s\n\t%s\n",
//                    qPrintable(accessible->getReference().path()),
//                    qPrintable(accessible->getParentPath().path()),
//                    qPrintable (accessible->getInterface().text(QAccessible::Name, 0))
//                   );

//            children = accessible->getChildren();
//            foreach (QSpiObject *chld, children)
//            {
//                qDebug ("\t\t%s", qPrintable(chld->getReference().path()));
//            }
                
//            registerChildren(child);
//            emit accessibleUpdated (accessible);
//            break;
//            // TODO
//            Emit the child added signal
//        }
//#endif
//        default:
//            break;
//    }
//    /* Think false is the right thing to do, no need to block these events here. */
//    return false;
//}

/*---------------------------------------------------------------------------*/

//void QSpiAccessibleCache::objectDestroyed (QObject *obj)
//{
//    if (cache.contains (obj))
//    {
//        QSpiObject *accessible = cache.take (obj);
//#if 0
//        qDebug("QSpiAccessibleBridge : Object Destroyed\n\t%s\n",
//               qPrintable(accessible->getReference().path()));
//#endif
//        emit RemoveAccessible (accessible->getReference());
//        delete accessible;
//    }
//}

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

#if 0
            qDebug ("QSpiAccessibleBridge : accessibleRegistered.\n\t%s\n\t%s\n\t%s\n",
                     qPrintable(accessible->getReference().path()),
                     qPrintable(accessible->getParentPath().path()),
                     qPrintable (accessible->getInterface().text(QAccessible::Name, 0))
                   );
#endif

            qDebug() << "QSpiAccessibleCache::objectToAccessible AddAccessible";
            emit AddAccessible(accessible->getCacheItem()); // this one actually works!!!
            return accessible;
        }
    }
    return 0;
}



/* AT-SPI Cache interface ---------------------------------------------------*/

QSpiAccessibleCacheArray QSpiAccessibleCache::GetItems()
{
    QList <QSpiAccessibleCacheItem> cacheArray;

    foreach (QSpiObject *obj, cache.values())
    {
        cacheArray << obj->getCacheItem();
    }
    return cacheArray;
}

/*END------------------------------------------------------------------------*/
