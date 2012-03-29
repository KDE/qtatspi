/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2008-2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef Q_SPI_STRUCT_MARSHALLERS_H
#define Q_SPI_STRUCT_MARSHALLERS_H

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QList>
#include <QPair>

/* QSpiIntList */
/*---------------------------------------------------------------------------*/

typedef QList <int> QSpiIntList;
Q_DECLARE_METATYPE(QSpiIntList)

typedef QList <uint> QSpiUIntList;
Q_DECLARE_METATYPE(QSpiUIntList)

/* QSpiObjectReference */
/*---------------------------------------------------------------------------*/

// FIXME: make this copy on write
struct QSpiObjectReference
{
    QString service;
    QDBusObjectPath path;

    QSpiObjectReference();
    QSpiObjectReference(const QDBusConnection& connection, const QDBusObjectPath& path)
        : service(connection.baseService()), path(path) {}
};

Q_DECLARE_METATYPE(QSpiObjectReference)

QDBusArgument &operator<<(QDBusArgument &argument, const QSpiObjectReference &address);
const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiObjectReference &address);


/* QSpiObjectReferenceArray */
/*---------------------------------------------------------------------------*/
typedef QList <QSpiObjectReference> QSpiObjectReferenceArray;

Q_DECLARE_METATYPE(QSpiObjectReferenceArray)

/* QSpiAccessibleCacheArray */
/*---------------------------------------------------------------------------*/

struct QSpiAccessibleCacheItem
{
    QSpiObjectReference         path;
    QSpiObjectReference         application;
    QSpiObjectReference         parent;
    QList <QSpiObjectReference> children;
    QStringList                 supportedInterfaces;
    QString                     name;
    uint                        role;
    QString                     description;
    QSpiUIntList                state;
};

typedef QList <QSpiAccessibleCacheItem> QSpiAccessibleCacheArray;

Q_DECLARE_METATYPE(QSpiAccessibleCacheItem)
Q_DECLARE_METATYPE(QSpiAccessibleCacheArray)

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

Q_DECLARE_METATYPE(QSpiAction)
Q_DECLARE_METATYPE(QSpiActionArray)

QDBusArgument &operator<<(QDBusArgument &argument, const QSpiAction &action);
const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiAction &action);

/* QSpiRelationArray */
/*---------------------------------------------------------------------------*/

typedef QPair < unsigned int, QList < QSpiObjectReference > > QSpiRelationArrayEntry;
Q_DECLARE_METATYPE(QSpiRelationArrayEntry)
typedef QList< QSpiRelationArrayEntry > QSpiRelationArray;
Q_DECLARE_METATYPE(QSpiRelationArray)

/* QSpiRangeList */
/*---------------------------------------------------------------------------*/

//a(iisv)
struct QSpiTextRange {
    int startOffset;
    int endOffset;
    QString contents;
    QVariant v;
};
Q_DECLARE_METATYPE(QSpiTextRange)
typedef QList <QSpiTextRange> QSpiTextRangeList;
Q_DECLARE_METATYPE(QSpiTextRangeList)

/* QSpiAttributeSet */
/*---------------------------------------------------------------------------*/

typedef QMap <QString, QString> QSpiAttributeSet;

Q_DECLARE_METATYPE(QSpiAttributeSet)

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

Q_DECLARE_METATYPE(QSpiAppUpdate)

/* QSpiDeviceEvent */
/*---------------------------------------------------------------------------*/

struct QSpiDeviceEvent {
    unsigned int type;
    int id;
    int hardwareCode;
    int modifiers;
    int timestamp;
    QString text;
    bool isText;
};

QDBusArgument &operator<<(QDBusArgument &argument, const QSpiDeviceEvent &event);
const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiDeviceEvent &event);

Q_DECLARE_METATYPE(QSpiDeviceEvent)

/*---------------------------------------------------------------------------*/

void qSpiInitializeStructTypes ();

#endif /* Q_SPI_STRUCT_MARSHALLERS_H */
