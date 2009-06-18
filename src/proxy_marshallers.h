/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2008, 2009      Nokia Corporation
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

/*
 * This file contains marshallers for structs used in the D-Bus proxy
 * interfaces.
 */

#ifndef Q_SPI_PROXY_MARSHALLERS_H
#define Q_SPI_PROXY_MARSHALLERS_H

#include <QString>
#include <QtDBus/QtDBus>

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

#endif /* Q_SPI_PROXY_MARSHALLERS_H */
