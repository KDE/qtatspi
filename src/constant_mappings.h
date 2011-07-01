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

/* State mapping */
/*---------------------------------------------------------------------------*/

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
