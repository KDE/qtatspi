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

#include <qaccessible2.h>
#include <qdbusvirtualobject.h>

#include "dbusconnection.h"
#include "struct_marshallers.h"

class QAccessibleInterface;
class QSpiAccessibleInterface;

class QSpiAdaptorV2 :public QDBusVirtualObject
{
public:
    QSpiAdaptorV2();

    explicit QSpiAdaptorV2(DBusConnection *connection, QObject *parent = 0);
    virtual ~QSpiAdaptorV2();


    /**
      Register this application as accessible on the accessibility DBus.
      */
    void registerApplication();

    virtual QString introspect(const QString &path) const;
    virtual bool handleMessage(const QDBusMessage &message, const QDBusConnection &connection);

private:

    // handlers for the different accessible interfaces
    bool applicationInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection);
    bool accessibleInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection);
    bool componentInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection);
    bool actionInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection);
    bool textInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection);
    bool editableTextInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection);
    bool valueInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection);
    bool tableInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection);

    void sendReply(const QDBusConnection &connection, const QDBusMessage &message, const QVariant &argument);
    QAccessibleInterface *accessibleParent(QAccessibleInterface *iface, int child);

    QPair<QAccessibleInterface*, int> interfaceFromPath(const QString& dbusPath);
    static QString pathForInterface(QAccessibleInterface *interface, int index);
    static QString pathForObject(QObject *object);

    // accessible helper functions
    QSpiRelationArray relationSet(QAccessibleInterface *interface, int child, const QDBusConnection &connection) const;
    QStringList accessibleInterfaces(QAccessibleInterface *interface, int child) const;

    // component helper functions
    static QSpiRect getExtents(QAccessibleInterface *interface, int child, uint coordType);

    // action helper functions
    QSpiActionArray getActions(QAccessibleInterface *interface) const;

    // text helper functions
    QVariantList getAttributes(QAccessibleInterface *interface, int offset, bool includeDefaults) const;
    QVariantList getAttributeValue(QAccessibleInterface *interface, int offset, const QString &attributeName) const;
    QVariantList getCharacterExtents(QAccessibleInterface *interface, int offset, uint coordType) const;
    QVariantList getRangeExtents(QAccessibleInterface *interface, int startOffset, int endOffset, uint coordType) const;
    QAccessible2::BoundaryType qAccessibleBoundaryType(int atspiTextBoundaryType) const;

    // private vars
    QSpiObjectReference accessibilityRegistry;
    DBusConnection *m_dbus;

    /// Assigned from the accessibility registry.
    int m_applicationId;
};

#endif // QSPIADAPTORV2_H
