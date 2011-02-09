/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2008, 2009      Nokia Corporation
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

#ifndef Q_SPI_STRUCT_MARSHALLERS_H
#define Q_SPI_STRUCT_MARSHALLERS_H

#include <QDBusObjectPath>
#include <QDBusArgument>
#include <QList>

/* QSpiIntList */
/*---------------------------------------------------------------------------*/

typedef QList <int> QSpiIntList;

Q_DECLARE_METATYPE (QSpiIntList);

/* QSpiObjectReference */
/*---------------------------------------------------------------------------*/

struct QSpiObjectReference
{
    QString name;
    QDBusObjectPath path;

    QSpiObjectReference()
        : path(QDBusObjectPath("/")) {}
    QSpiObjectReference(QString name, QDBusObjectPath path)
        : name(name), path(path) {}
};

Q_DECLARE_METATYPE (QSpiObjectReference);

QDBusArgument &operator<<(QDBusArgument &argument, const QSpiObjectReference &address);
const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiObjectReference &address);

/* QSpiObjectReferenceArray */
/*---------------------------------------------------------------------------*/
typedef QList <QSpiObjectReference> QSpiObjectReferenceArray;

Q_DECLARE_METATYPE (QSpiObjectReferenceArray);

/* QSpiAccessibleCacheArray */
/*---------------------------------------------------------------------------*/

struct QSpiAccessibleCacheItem
{
    QSpiObjectReference         path;
    QSpiObjectReference         application;
    QSpiObjectReference         parent;
    int                         role;
    QString                     description;
    QSpiIntList                 states;
    QList <QSpiObjectReference> children;
    QStringList                 supported;
    QString                     name;
};

typedef QList <QSpiAccessibleCacheItem> QSpiAccessibleCacheArray;

Q_DECLARE_METATYPE (QSpiAccessibleCacheItem);
Q_DECLARE_METATYPE (QSpiAccessibleCacheArray);

QDBusArgument &operator<<(QDBusArgument &argument, const QSpiAccessibleCacheItem &item);
const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiAccessibleCacheItem &item);

/* QSpiActionArray */
/*---------------------------------------------------------------------------*/

struct QSpiAction
{
    QString name;
    QString description;
    QString keyBinding;
};

typedef QList <QSpiAction> QSpiActionArray;

Q_DECLARE_METATYPE (QSpiAction);
Q_DECLARE_METATYPE (QSpiActionArray);

QDBusArgument &operator<<(QDBusArgument &argument, const QSpiAction &action);
const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiAction &action);

/* QSpiRelationArray */
/*---------------------------------------------------------------------------*/

typedef QMap < unsigned int, QMap < QString, QSpiObjectReference > > QSpiRelationArray;

/* QSpiRangeList */
/*---------------------------------------------------------------------------*/

//a(iisv)
struct QSpiTextRange {
    int startOffset;
    int endOffset;
    QString contents;
    QVariant v; // TODO: check
};
typedef QList <QSpiTextRange> QSpiRangeList;

/* QSpiRect */
/*---------------------------------------------------------------------------*/

struct QSpiRect
{
    int x;
    int y;
    int width;
    int height;
};

Q_DECLARE_METATYPE (QSpiRect);

QDBusArgument &operator<<(QDBusArgument &argument, const QSpiRect &rect);
const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiRect &rect);

/* QSpiAttributeSet */
/*---------------------------------------------------------------------------*/

typedef QMap <QString, QString> QSpiAttributeSet;

Q_DECLARE_METATYPE (QSpiAttributeSet);

/* QSpiAppUpdate */
/*---------------------------------------------------------------------------*/

enum QSpiAppUpdateType {
    QSPI_APP_UPDATE_ADDED = 0,
    QSPI_APP_UPDATE_REMOVED = 1
};

struct QSpiAppUpdate {
    int type; /* Is an application added or removed */
    QString address; /* D-Bus address of application added or removed */
};

QDBusArgument &operator<<(QDBusArgument &argument, const QSpiAppUpdate &update);
const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiAppUpdate &update);

Q_DECLARE_METATYPE (QSpiAppUpdate)

/* QSpiDeviceEvent */
/*---------------------------------------------------------------------------*/

struct QSpiDeviceEvent {
    unsigned int type;
    int id;
    short int hw_code;
    short int modifiers;
    short int timestamp;
    QString event_string;
    bool is_text;
};

QDBusArgument &operator<<(QDBusArgument &argument, const QSpiDeviceEvent &event);
const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiDeviceEvent &event);

Q_DECLARE_METATYPE (QSpiDeviceEvent)

/*---------------------------------------------------------------------------*/

void qspi_initialize_struct_types ();

#endif /* Q_SPI_STRUCT_MARSHALLERS_H */
