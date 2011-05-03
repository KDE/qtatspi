/*
 * D-Bus AT-SPI 2 plugin
 *
 * Copyright 2011       Nokia.
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

#include "bridge.h"

QT_BEGIN_NAMESPACE

class QSpiAccessibleBridgePlugin: public QAccessibleBridgePlugin
{
public:
        QSpiAccessibleBridgePlugin(QObject *parent = 0);
        virtual ~QSpiAccessibleBridgePlugin() {};

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
