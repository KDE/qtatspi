/*
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

#ifndef QSPIADAPTORV2_H
#define QSPIADAPTORV2_H

#include <qdbusvirtualobject.h>

#include "dbusconnection.h"
#include "struct_marshallers.h"

class QSpiAdaptorV2 :public QDBusVirtualObject
{
public:
    QSpiAdaptorV2();

    explicit QSpiAdaptorV2(DBusConnection *connection, QObject *parent = 0);
    virtual ~QSpiAdaptorV2()
    {}

    virtual QString introspect(const QString &path) const;
    virtual bool handleMessage(const QDBusMessage &message, const QDBusConnection &connection);

private:
    void callAccessibilityRegistry();

    QSpiObjectReference accessibilityRegistry;
    DBusConnection *m_dbus;
};

#endif // QSPIADAPTORV2_H
