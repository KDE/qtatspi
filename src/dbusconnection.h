/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2011      Nokia.
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

#ifndef DBUSCONNECTION_H
#define DBUSCONNECTION_H

#include <QtCore/QString>
#include <QtDBus/QDBusConnection>

class DBusConnection
{
public:
    DBusConnection();
    QDBusConnection connection() const;

private:
    QString getAccessibilityBusAddress() const;
    QDBusConnection connectDBus();

    QDBusConnection dbusConnection;
};

#endif // DBUSCONNECTION_H
