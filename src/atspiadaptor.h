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

#ifndef ATSPIADAPTOR_H
#define ATSPIADAPTOR_H

#include <qaccessible2.h>
#include <qdbusvirtualobject.h>
#include <qsharedpointer.h>

#include "dbusconnection.h"
#include "struct_marshallers.h"

class QAccessibleInterface;
class QSpiAccessibleInterface;

typedef QSharedPointer<QAccessibleInterface> QAccessibleInterfacePointer;

class AtSpiAdaptor :public QDBusVirtualObject
{
    Q_OBJECT

public:
    explicit AtSpiAdaptor(DBusConnection *connection, QObject *parent = 0);
    virtual ~AtSpiAdaptor();

    /**
      Register this application as accessible on the accessibility DBus.
      */
    void registerApplication();

    /**
      Provide DBus introspection.
      */
    virtual QString introspect(const QString &path) const;

    /**
      Handle incoming DBus messages.
      */
    virtual bool handleMessage(const QDBusMessage &message, const QDBusConnection &connection);

    /**
      Updates from Qt.
      */
    void notify(int reason, QAccessibleInterface *interface, int child);

    /**
      When initialized we will send updates, not before this.
      */
    void setInitialized(bool init);

public Q_SLOTS:
    void eventListenerRegistered(const QString &bus, const QString &path);
    void eventListenerDeregistered(const QString &bus, const QString &path);
    void windowActivated(QObject* window, bool active);

private:

    void updateEventListeners();
    void setBitFlag(const QString &flag);


    // sending messages
    QVariantList packDBusSignalArguments(const QString &type, int data1, int data2, const QVariant &variantData) const;
    bool sendDBusSignal(const QString &path, const QString &interface, const QString &name, const QVariantList &arguments) const;
    QVariant variantForPath(const QString &path) const;

    void sendFocusChanged(QAccessibleInterface *interface, int child) const;
    void notifyAboutCreation(QAccessibleInterface *interface, int child) const;
    void notifyAboutDestruction(QAccessibleInterface *interface, int child) const;

    // handlers for the different accessible interfaces
    bool applicationInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection);
    bool accessibleInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection);
    bool componentInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection);
    bool actionInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection);
    bool textInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection);
    bool editableTextInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection);
    bool valueInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection);
    bool tableInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection);

    void sendReply(const QDBusConnection &connection, const QDBusMessage &message, const QVariant &argument) const;
    QAccessibleInterface *accessibleParent(QAccessibleInterface *iface, int child) const;

    QPair<QAccessibleInterfacePointer, int> interfaceFromPath(const QString& dbusPath) const;
    QString pathForInterface(QAccessibleInterface *interface, int index, bool inDestructor = false) const;
    QString pathForObject(QObject *object) const;

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
    bool initialized;

    mutable QHash<quintptr, QWeakPointer<QObject> > m_handledObjects;

    // Bit fields - which updates to send

    // AT-SPI has some events that we do not care about:
    // document
    // document-load-complete
    // document-load-stopped
    // document-reload
    uint sendFocus : 1;
    // mouse abs/rel/button

    // all of object
    uint sendObject : 1;
    uint sendObject_active_descendant_changed : 1;
    uint sendObject_attributes_changed : 1;
    uint sendObject_bounds_changed : 1;
    uint sendObject_children_changed : 1;
//    uint sendObject_children_changed_add : 1;
//    uint sendObject_children_changed_remove : 1;
    uint sendObject_column_deleted : 1;
    uint sendObject_column_inserted : 1;
    uint sendObject_column_reordered : 1;
    uint sendObject_link_selected : 1;
    uint sendObject_model_changed : 1;
    uint sendObject_property_change : 1;
    uint sendObject_property_change_accessible_description : 1;
    uint sendObject_property_change_accessible_name : 1;
    uint sendObject_property_change_accessible_parent : 1;
    uint sendObject_property_change_accessible_role : 1;
    uint sendObject_property_change_accessible_table_caption : 1;
    uint sendObject_property_change_accessible_table_column_description : 1;
    uint sendObject_property_change_accessible_table_column_header : 1;
    uint sendObject_property_change_accessible_table_row_description : 1;
    uint sendObject_property_change_accessible_table_row_header : 1;
    uint sendObject_property_change_accessible_table_summary : 1;
    uint sendObject_property_change_accessible_value : 1;
    uint sendObject_row_deleted : 1;
    uint sendObject_row_inserted : 1;
    uint sendObject_row_reordered : 1;
    uint sendObject_selection_changed : 1;
    uint sendObject_state_changed : 1;
    uint sendObject_text_attributes_changed : 1;
    uint sendObject_text_bounds_changed : 1;
    uint sendObject_text_caret_moved : 1;
    uint sendObject_text_changed : 1;
//    uint sendObject_text_changed_delete : 1;
//    uint sendObject_text_changed_insert : 1;
    uint sendObject_text_selection_changed : 1;
    uint sendObject_value_changed : 1;
    uint sendObject_visible_data_changed : 1;

    // we don't implement terminal
    // terminal-application_changed/charwidth_changed/columncount_changed/line_changed/linecount_changed
    uint sendWindow : 1;
    uint sendWindow_activate : 1;
    uint sendWindow_close: 1;
    uint sendWindow_create : 1;
    uint sendWindow_deactivate : 1;
//    uint sendWindow_desktop_create : 1;
//    uint sendWindow_desktop_destroy : 1;
    uint sendWindow_lower : 1;
    uint sendWindow_maximize : 1;
    uint sendWindow_minimize : 1;
    uint sendWindow_move : 1;
    uint sendWindow_raise : 1;
    uint sendWindow_reparent : 1;
    uint sendWindow_resize : 1;
    uint sendWindow_restore : 1;
    uint sendWindow_restyle : 1;
    uint sendWindow_shade : 1;
    uint sendWindow_unshade : 1;
};

#endif
