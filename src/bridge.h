/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2008-2011 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright 2008, 2009      Codethink Ltd.
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

#ifndef QSPIACCESSIBLEBRIDGE_H
#define QSPIACCESSIBLEBRIDGE_H

#include <QtGui/QApplication>
#include <QAccessibleInterface>
#include <QAccessibleBridge>
#include <QDBusConnection>

class DBusConnection;
class DeviceEventControllerProxy;
class QSpiDBusCache;
class AtSpiAdaptor;

#define spiBridge QSpiAccessibleBridge::instance()

class QSpiAccessibleBridge: public QObject, public QAccessibleBridge
{
        Q_OBJECT
public:
        QSpiAccessibleBridge();

        static QSpiAccessibleBridge *instance() { return self; }

        virtual ~QSpiAccessibleBridge();
        virtual void setRootObject(QAccessibleInterface *obj);

        virtual void notifyAccessibilityUpdate(int reason, QAccessibleInterface *obj, int child);

//        QSpiAdaptor* createSpiObject(QAccessibleInterface* interface, int index = 0);
//        void removeAdaptor(QSpiAdaptor* adaptor);

        QDBusConnection dBusConnection() const;

private slots:
        void initDBusConnection();

private:
//        void notifyAboutCreation(QSpiAdaptor* accessible);
//        void notifyAboutDestruction(QSpiAdaptor* accessible);
        static QSpiAccessibleBridge* self;
        QSpiDBusCache *cache;
        DeviceEventControllerProxy *dec;

        AtSpiAdaptor *dbusAdaptor;
        bool rootObjectInitialized;
        DBusConnection* dbusConnection;
};

#endif
