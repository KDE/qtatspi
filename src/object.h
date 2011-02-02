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

#ifndef Q_SPI_OBJECT_H
#define Q_SPI_OBJECT_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include <QDBusObjectPath>

#include <QAccessible>
#include <QAccessibleInterface>

#include "struct_marshallers.h"

#define QSPI_INTERFACE_PREFIX "org.a11y.atspi"

#define QSPI_INTERFACE_ACCESSIBLE            QSPI_INTERFACE_PREFIX ".Accessible"
#define QSPI_INTERFACE_ACTION                QSPI_INTERFACE_PREFIX ".Action"
#define QSPI_INTERFACE_APPLICATION           QSPI_INTERFACE_PREFIX ".Application"
#define QSPI_INTERFACE_COLLECTION            QSPI_INTERFACE_PREFIX ".Collection"
#define QSPI_INTERFACE_COMPONENT             QSPI_INTERFACE_PREFIX ".Component"
#define QSPI_INTERFACE_DOCUMENT              QSPI_INTERFACE_PREFIX ".Document"
#define QSPI_INTERFACE_EDITABLE_TEXT         QSPI_INTERFACE_PREFIX ".EditableText"
#define QSPI_INTERFACE_HYPERLINK             QSPI_INTERFACE_PREFIX ".Hyperlink"
#define QSPI_INTERFACE_HYPERTEXT             QSPI_INTERFACE_PREFIX ".Hypertext"
#define QSPI_INTERFACE_IMAGE                 QSPI_INTERFACE_PREFIX ".Image"
#define QSPI_INTERFACE_REGISTRY              QSPI_INTERFACE_PREFIX ".Registry"
#define QSPI_INTERFACE_SELECTION             QSPI_INTERFACE_PREFIX ".Selection"
#define QSPI_INTERFACE_TABLE                 QSPI_INTERFACE_PREFIX ".Table"
#define QSPI_INTERFACE_TEXT                  QSPI_INTERFACE_PREFIX ".Text"
#define QSPI_INTERFACE_TREE                  QSPI_INTERFACE_PREFIX ".Tree"
#define QSPI_INTERFACE_VALUE                 QSPI_INTERFACE_PREFIX ".Value"

#define QSPI_OBJECT_PATH_PREFIX  "/org/a11y/atspi/accessible/"
#define QSPI_OBJECT_PATH_NULL    QSPI_OBJECT_PATH_PREFIX "null"
#define QSPI_OBJECT_PATH_ROOT    QSPI_OBJECT_PATH_PREFIX "root"

/*---------------------------------------------------------------------------*/

class QSpiAccessibleCache;

/*
 * This class is the virtual base class for all accessible objects.
 *
 * These are objects that are resident within this program and have a 
 * QAccessibleInterface backing them.
 * 
 */

class QSpiObject : public QObject
{
    Q_OBJECT

public:
    QSpiObject(QSpiAccessibleCache  *cache,
               QAccessibleInterface *interface);

    QAccessibleInterface     &getInterface () const;
    QStringList               supportedInterfaces () const;
    QSpiAccessibleCacheItem   getCacheItem      () const;

    virtual QSpiObjectReference &getReference () const;
    virtual QSpiObjectReference &getParentReference () const = 0;

    void accessibleEvent (QAccessible::Event event);
    //bool eventFilter     (QObject *obj, QEvent *event);

protected:
    QAccessibleInterface *interface;
    QSpiAccessibleCache *cache;
    QSpiObjectReference *reference;
    QStringList supported;
};

#endif /* Q_SPI_OBJECT_H */
