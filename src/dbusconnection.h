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

#ifndef DBUSCONNECTION_H
#define DBUSCONNECTION_H

#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusPendingCallWatcher>

class DBusConnection : public QObject
{
    Q_OBJECT
public:
    DBusConnection();
    bool isFetchingConnection() const;
    bool isConnected() const;
    QDBusConnection connection() const;
//     static bool isEnabled();

signals:
    void connectionFetched();

private slots:
    void initFinished();

private:
    void init();
    QDBusConnection connectDBus();
    QString getAccessibilityBusAddress() const;
    QString getAccessibilityBusAddressDBus() const;
    QString getAccessibilityBusAddressXAtom() const;

    QDBusConnection dbusConnection;
    QDBusPendingCallWatcher *initWatcher;
    bool connected;
};

#endif // DBUSCONNECTION_H
