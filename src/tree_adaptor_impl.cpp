/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2008, 2009      Nokia.
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

#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

#include "tree_adaptor.h"
#include "cache.h"

/*
 * Implementation of adaptor class QSpiTreeAdaptor
 */

QSpiTreeAdaptor::QSpiTreeAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

QSpiTreeAdaptor::~QSpiTreeAdaptor()
{
    // destructor
}

QDBusObjectPath QSpiTreeAdaptor::getRoot()
{
    // handle method call org.freedesktop.atspi.Tree.getRoot
    return static_cast <QSpiAccessibleCache *> (parent())->getRoot()->getPath();
}

QSpiAccessibleCacheArray QSpiTreeAdaptor::getTree()
{
    // handle method call org.freedesktop.atspi.Tree.getTree
    return static_cast <QSpiAccessibleCache *> (parent())->listAccessibles();
}
