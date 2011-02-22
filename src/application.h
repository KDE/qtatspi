/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2009, 2011      Nokia.
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

#ifndef Q_SPI_APPLICATION_H
#define Q_SPI_APPLICATION_H

#include <QtDBus/QDBusConnection>

#include <QAccessible>
#include <QAccessibleInterface>

#include "adaptor.h"

/*
 * Used for the root object.
 *
 * Uses the root object reference and reports its parent as the desktop object.
 */
class QSpiApplication : public QSpiAdaptor
{
    Q_OBJECT

public:
    QSpiApplication(QSpiAccessibleCache  *cache,
                    QAccessibleInterface *interface,
                    QDBusConnection c);

    virtual QSpiObjectReference& getParentReference() const;

    // TODO: do we care about events here?
    virtual void accessibleEvent(QAccessible::Event event) { Q_UNUSED(event) }

private:
    QSpiObjectReference *accessibilityRegistry;
    QDBusConnection dbusConnection;
};

#endif
