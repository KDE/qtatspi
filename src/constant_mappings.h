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

/*
 * This file contains AT-SPI constants and mappings between QAccessible
 * and AT-SPI constants such as 'role' and 'state' enumerations.
 */

#ifndef Q_SPI_CONSTANT_MAPPINGS_H
#define Q_SPI_CONSTANT_MAPPINGS_H

#include "struct_marshallers.h"


#include <atspi/atspi-constants.h>
#include <QAccessible>

#define QSPI_OBJECT_PATH_PREFIX  "/org/a11y/atspi/accessible/"
#define QSPI_OBJECT_PATH_NULL    QSPI_OBJECT_PATH_PREFIX "null"
#define QSPI_OBJECT_PATH_ROOT    QSPI_OBJECT_PATH_PREFIX "root"

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

#define QSPI_REGISTRY_NAME "org.a11y.atspi.Registry"


struct RoleNames {
    RoleNames() {}
    RoleNames(AtspiRole r, const QString& n, const QString& ln)
        :spiRole_(r), name_(n), localizedName_(ln)
    {}

    AtspiRole spiRole() const {return spiRole_;}
    QString name() const {return name_;}
    QString localizedName() const {return localizedName_;}

private:
    AtspiRole spiRole_;
    QString name_;
    QString localizedName_;
};

extern QHash <QAccessible::Role, RoleNames> qSpiRoleMapping;
extern QHash <int, AtspiStateType> qSpiStateMapping;

inline void setSpiStateBit(quint64* state, AtspiStateType spiState)
{
    *state |= quint64(1) << spiState;
}

inline void unsetSpiStateBit(quint64* state, AtspiStateType spiState)
{
    *state &= ~(quint64(1) << spiState);
}

quint64 spiStatesFromQState(QAccessible::State state);
QSpiUIntList spiStateSetFromSpiStates(quint64 states);

/*---------------------------------------------------------------------------*/

void qSpiInitializeConstantMappings();

#endif /* Q_SPI_CONSTANT_MAPPINGS_H */
