/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QSPIACCESSIBLEINTERFACE_H
#define QSPIACCESSIBLEINTERFACE_H

#include "struct_marshallers.h"

class QAccessibleInterface;
class QDBusConnection;
class QDBusMessage;
class QDBusVariant;
class QObject;
class QString;
class QVariant;

class QSpiAccessibleInterface
{
public:
    QSpiAccessibleInterface();
    bool handleMessage(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection);

private:
    void sendReply(const QDBusConnection &connection, const QDBusMessage &message, const QVariant &argument);
    QAccessibleInterface *accessibleParent(QAccessibleInterface *iface, int child);

    static QString pathForInterface(QAccessibleInterface *interface, int index);
    static QString pathForObject(QObject *object);

    QSpiRelationArray relationSet(QAccessibleInterface *interface, int child, const QDBusConnection &connection) const;
    QStringList accessibleInterfaces(QAccessibleInterface *interface, int child) const;
};

#endif // QSPIACCESSIBLEINTERFACE_H
