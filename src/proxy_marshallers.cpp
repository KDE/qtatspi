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

/*
 * This file contains marshallers for structs used in the D-Bus proxy
 * interfaces.
 */

#include "proxy_marshallers.h"

/* QSpiAppUpdate */
/*---------------------------------------------------------------------------*/

QDBusArgument &operator<<(QDBusArgument &argument, const QSpiAppUpdate &update) {
        argument.beginStructure();
        argument << update.type << update.address;
        argument.endStructure();
        return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiAppUpdate &update) {
        argument.beginStructure();
        argument >> update.type >> update.address;
        argument.endStructure();
        return argument;
}

/*---------------------------------------------------------------------------*/
