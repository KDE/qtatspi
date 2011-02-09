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

#ifndef QSPIACCESSIBLEBRIDGE_H
#define QSPIACCESSIBLEBRIDGE_H

#include <QtGui/QApplication>
#include <QAccessibleInterface>
#include <QAccessibleBridge>

class DeviceEventControllerProxy;
class QSpiAccessibleCache;
class QAccessibleInterface;

class QSpiAccessibleBridge: public QObject, public QAccessibleBridge
{
        Q_OBJECT
public:
        QSpiAccessibleBridge(): cache(0), rootInterface(0)
        {}

        virtual ~QSpiAccessibleBridge ();
        virtual void setRootObject(QAccessibleInterface *obj);
        virtual void notifyAccessibilityUpdate(int reason, QAccessibleInterface *obj, int child);

public Q_SLOTS:
        void aboutToQuit();

Q_SIGNALS:
        void ChildrenChanged();

private:
        QSpiAccessibleCache *cache;
        DeviceEventControllerProxy *dec;

        QAccessibleInterface* rootInterface;
};

/*---------------------------------------------------------------------------*/


#endif
