/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2009-2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef Q_SPI_ACCESSIBLE_H
#define Q_SPI_ACCESSIBLE_H

#include "adaptor.h"

#include <QtCore/QObject>
#include <QDBusObjectPath>
#include <QtDBus/QtDBus>

#include <QAccessible>


/*
 * Used for all accessible objects other than the root object.
 *
 * Assigns its own D-Bus reference and uses the QAccessibleInterface 'navigate'
 * method to report its parent.
 */
class QSpiAccessible : public QSpiAdaptor
{
    Q_OBJECT

public:
    QSpiAccessible(QAccessibleInterface *interface, int index = 0);
    virtual ~QSpiAccessible() {}
    virtual QSpiObjectReference getParentReference() const;
    virtual void accessibleEvent(QAccessible::Event event);
    void windowActivated();

    static QString pathForInterface(QAccessibleInterface *interface, int index);

Q_SIGNALS:
    // window
    void Activate(const QString &type, int detail1, int detail2, const QDBusVariant &data, const QSpiObjectReference &parent);
    void Create(const QString &type, int detail1, int detail2, const QDBusVariant &data, const QSpiObjectReference &parent);
    void Restore(const QString &type, int detail1, int detail2, const QDBusVariant &data, const QSpiObjectReference &parent);

private:
    static QDBusObjectPath getUnique();

    // AT-SPI wants updates of what changed, not only the new state.
    QAccessible::State state;
    // When changing text, we remove the complete old text, but for that we need its length.
    QString oldText;

    friend class QSpiAccessibleBridge;
};

#endif /* Q_SPI_ACCESSIBLE_H */
