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


#include "bridge.h"

QT_BEGIN_NAMESPACE

class QSpiAccessibleBridgePlugin: public QAccessibleBridgePlugin
{
public:
        QSpiAccessibleBridgePlugin(QObject *parent = 0);
        virtual ~QSpiAccessibleBridgePlugin() {}

        virtual QAccessibleBridge* create(const QString &key);
        virtual QStringList keys() const;
};

QSpiAccessibleBridgePlugin::QSpiAccessibleBridgePlugin(QObject *parent)
: QAccessibleBridgePlugin(parent)
{
}

QAccessibleBridge* QSpiAccessibleBridgePlugin::create(const QString &key)
{
    if (key == "QSPIACCESSIBLEBRIDGE")
        return new QSpiAccessibleBridge();
    return 0;
}

QStringList QSpiAccessibleBridgePlugin::keys() const
{
    return QStringList() << "QSPIACCESSIBLEBRIDGE";
}

Q_EXPORT_PLUGIN2(qspiaccessiblebridge, QSpiAccessibleBridgePlugin)

QT_END_NAMESPACE
