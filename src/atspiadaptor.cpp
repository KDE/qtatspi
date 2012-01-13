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


#include "atspiadaptor.h"

#include <qaccessible.h>
#include <qaccessible2.h>
#include <qapplication.h>
#include <qdbusmessage.h>
#include <qwidget.h>

#include <qdebug.h>

#include "generated/socket_proxy.h"

#include "standardactionwrapper.h"
#include "constant_mappings.h"

#define ACCESSIBLE_LAST_TEXT "QIA2_LAST_TEXT"
#define ACCESSIBLE_LAST_STATE "QIA2_LAST_STATE"

/*!
    \class AtSpiAdaptor

    \brief AtSpiAdaptor is the main class to forward between QAccessibleInterface and AT-SPI DBus

    AtSpiAdaptor implements the functions specified in all at-spi interfaces.
    It sends notifications comming from Qt via dbus and listens to incoming dbus requests.
*/

AtSpiAdaptor::AtSpiAdaptor(DBusConnection *connection, QObject *parent)
    : QDBusVirtualObject(parent), m_dbus(connection), initialized(false)
    , sendFocus(0)
    , sendObject(0)
    , sendObject_active_descendant_changed(0)
    , sendObject_attributes_changed(0)
    , sendObject_bounds_changed(0)
    , sendObject_children_changed(0)
//    , sendObject_children_changed_add(0)
//    , sendObject_children_changed_remove(0)
    , sendObject_column_deleted(0)
    , sendObject_column_inserted(0)
    , sendObject_column_reordered(0)
    , sendObject_link_selected(0)
    , sendObject_model_changed(0)
    , sendObject_property_change(0)
    , sendObject_property_change_accessible_description(0)
    , sendObject_property_change_accessible_name(0)
    , sendObject_property_change_accessible_parent(0)
    , sendObject_property_change_accessible_role(0)
    , sendObject_property_change_accessible_table_caption(0)
    , sendObject_property_change_accessible_table_column_description(0)
    , sendObject_property_change_accessible_table_column_header(0)
    , sendObject_property_change_accessible_table_row_description(0)
    , sendObject_property_change_accessible_table_row_header(0)
    , sendObject_property_change_accessible_table_summary(0)
    , sendObject_property_change_accessible_value(0)
    , sendObject_row_deleted(0)
    , sendObject_row_inserted(0)
    , sendObject_row_reordered(0)
    , sendObject_selection_changed(0)
    , sendObject_text_attributes_changed(0)
    , sendObject_text_bounds_changed(0)
    , sendObject_text_caret_moved(0)
    , sendObject_text_changed(0)
//    , sendObject_text_changed_delete(0)
//    , sendObject_text_changed_insert(0)
    , sendObject_text_selection_changed(0)
    , sendObject_value_changed(0)
    , sendObject_visible_data_changed(0)
    , sendWindow(0)
    , sendWindow_activate(0)
    , sendWindow_close(0)
    , sendWindow_create(0)
    , sendWindow_deactivate(0)
//    , sendWindow_desktop_create(0)
//    , sendWindow_desktop_destroy(0)
    , sendWindow_lower(0)
    , sendWindow_maximize(0)
    , sendWindow_minimize(0)
    , sendWindow_move(0)
    , sendWindow_raise(0)
    , sendWindow_reparent(0)
    , sendWindow_resize(0)
    , sendWindow_restore(0)
    , sendWindow_restyle(0)
    , sendWindow_shade(0)
    , sendWindow_unshade(0)
{
}

AtSpiAdaptor::~AtSpiAdaptor()
{
}

/*!
  Provide DBus introspection.
  */
QString AtSpiAdaptor::introspect(const QString &path) const
{
    QLatin1String accessibleIntrospection(
                "  <interface name=\"org.a11y.atspi.Accessible\">\n"
                "    <property access=\"read\" type=\"s\" name=\"Name\"/>\n"
                "    <property access=\"read\" type=\"s\" name=\"Description\"/>\n"
                "    <property access=\"read\" type=\"(so)\" name=\"Parent\">\n"
                "      <annotation value=\"QSpiObjectReference\" name=\"com.trolltech.QtDBus.QtTypeName\"/>\n"
                "    </property>\n"
                "    <property access=\"read\" type=\"i\" name=\"ChildCount\"/>\n"
                "    <method name=\"GetChildAtIndex\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"index\"/>\n"
                "      <arg direction=\"out\" type=\"(so)\"/>\n"
                "      <annotation value=\"QSpiObjectReference\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
                "    </method>\n"
                "    <method name=\"GetChildren\">\n"
                "      <arg direction=\"out\" type=\"a(so)\"/>\n"
                "      <annotation value=\"QSpiObjectReferenceArray\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
                "    </method>\n"
                "    <method name=\"GetIndexInParent\">\n"
                "      <arg direction=\"out\" type=\"i\"/>\n"
                "    </method>\n"
                "    <method name=\"GetRelationSet\">\n"
                "      <arg direction=\"out\" type=\"a(ua(so))\"/>\n"
                "      <annotation value=\"QSpiRelationArray\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
                "    </method>\n"
                "    <method name=\"GetRole\">\n"
                "      <arg direction=\"out\" type=\"u\"/>\n"
                "    </method>\n"
                "    <method name=\"GetRoleName\">\n"
                "      <arg direction=\"out\" type=\"s\"/>\n"
                "    </method>\n"
                "    <method name=\"GetLocalizedRoleName\">\n"
                "      <arg direction=\"out\" type=\"s\"/>\n"
                "    </method>\n"
                "    <method name=\"GetState\">\n"
                "      <arg direction=\"out\" type=\"au\"/>\n"
                "      <annotation value=\"QSpiUIntList\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
                "    </method>\n"
                "    <method name=\"GetAttributes\">\n"
                "      <arg direction=\"out\" type=\"a{ss}\"/>\n"
                "      <annotation value=\"QSpiAttributeSet\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
                "    </method>\n"
                "    <method name=\"GetApplication\">\n"
                "      <arg direction=\"out\" type=\"(so)\"/>\n"
                "      <annotation value=\"QSpiObjectReference\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
                "    </method>\n"
                "  </interface>\n"
                );

    QLatin1String actionIntrospection(
                "  <interface name=\"org.a11y.atspi.Action\">\n"
                "    <property access=\"read\" type=\"i\" name=\"NActions\"/>\n"
                "    <method name=\"GetDescription\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"index\"/>\n"
                "      <arg direction=\"out\" type=\"s\"/>\n"
                "    </method>\n"
                "    <method name=\"GetName\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"index\"/>\n"
                "      <arg direction=\"out\" type=\"s\"/>\n"
                "    </method>\n"
                "    <method name=\"GetKeyBinding\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"index\"/>\n"
                "      <arg direction=\"out\" type=\"s\"/>\n"
                "    </method>\n"
                "    <method name=\"GetActions\">\n"
                "      <arg direction=\"out\" type=\"a(sss)\" name=\"index\"/>\n"
                "      <annotation value=\"QSpiActionArray\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
                "    </method>\n"
                "    <method name=\"DoAction\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"index\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "  </interface>\n"
                );

    QLatin1String applicationIntrospection(
                "  <interface name=\"org.a11y.atspi.Application\">\n"
                "    <property access=\"read\" type=\"s\" name=\"ToolkitName\"/>\n"
                "    <property access=\"read\" type=\"s\" name=\"Version\"/>\n"
                "    <property access=\"readwrite\" type=\"i\" name=\"Id\"/>\n"
                "    <method name=\"GetLocale\">\n"
                "      <arg direction=\"in\" type=\"u\" name=\"lctype\"/>\n"
                "      <arg direction=\"out\" type=\"s\"/>\n"
                "    </method>\n"
                "    <method name=\"GetApplicationBusAddress\">\n"
                "      <arg direction=\"out\" type=\"s\" name=\"address\"/>\n"
                "    </method>\n"
                "    <!--\n"
                "  <method name=\"RegisterEventListener\">\n"
                "    <arg direction=\"in\" name=\"event\" type=\"s\"/>\n"
                "  </method>\n"
                "\n"
                "  <method name=\"DeregisterEventListener\">\n"
                "    <arg direction=\"in\" name=\"event\" type=\"s\"/>\n"
                "  </method>\n"
                "-->\n"
                "  </interface>\n"
                );

    QLatin1String componentIntrospection(
                "  <interface name=\"org.a11y.atspi.Component\">\n"
                "    <method name=\"Contains\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"x\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"y\"/>\n"
                "      <arg direction=\"in\" type=\"u\" name=\"coord_type\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "    <method name=\"GetAccessibleAtPoint\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"x\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"y\"/>\n"
                "      <arg direction=\"in\" type=\"u\" name=\"coord_type\"/>\n"
                "      <arg direction=\"out\" type=\"(so)\"/>\n"
                "      <annotation value=\"QSpiObjectReference\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
                "    </method>\n"
                "    <method name=\"GetExtents\">\n"
                "      <arg direction=\"in\" type=\"u\" name=\"coord_type\"/>\n"
                "      <arg direction=\"out\" type=\"(iiii)\"/>\n"
                "      <annotation value=\"QSpiRect\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
                "    </method>\n"
                "    <method name=\"GetPosition\">\n"
                "      <arg direction=\"in\" type=\"u\" name=\"coord_type\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"x\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"y\"/>\n"
                "    </method>\n"
                "    <method name=\"GetSize\">\n"
                "      <arg direction=\"out\" type=\"i\" name=\"width\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"height\"/>\n"
                "    </method>\n"
                "    <method name=\"GetLayer\">\n"
                "      <arg direction=\"out\" type=\"u\"/>\n"
                "    </method>\n"
                "    <method name=\"GetMDIZOrder\">\n"
                "      <arg direction=\"out\" type=\"n\"/>\n"
                "    </method>\n"
                "    <method name=\"GrabFocus\">\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "    <method name=\"GetAlpha\">\n"
                "      <arg direction=\"out\" type=\"d\"/>\n"
                "    </method>\n"
                "    <method name=\"SetExtents\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"x\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"y\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"width\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"height\"/>\n"
                "      <arg direction=\"in\" type=\"u\" name=\"coord_type\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "    <method name=\"SetPosition\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"x\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"y\"/>\n"
                "      <arg direction=\"in\" type=\"u\" name=\"coord_type\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "    <method name=\"SetSize\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"width\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"height\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "  </interface>\n"
                );

    QLatin1String editableTextIntrospection(
                "  <interface name=\"org.a11y.atspi.EditableText\">\n"
                "    <method name=\"SetTextContents\">\n"
                "      <arg direction=\"in\" type=\"s\" name=\"newContents\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "    <method name=\"InsertText\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"position\"/>\n"
                "      <arg direction=\"in\" type=\"s\" name=\"text\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"length\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "    <method name=\"CopyText\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"startPos\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"endPos\"/>\n"
                "    </method>\n"
                "    <method name=\"CutText\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"startPos\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"endPos\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "    <method name=\"DeleteText\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"startPos\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"endPos\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "    <method name=\"PasteText\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"position\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "  </interface>\n"
                );

    QLatin1String tableIntrospection(
                "  <interface name=\"org.a11y.atspi.Table\">\n"
                "    <property access=\"read\" type=\"i\" name=\"NRows\"/>\n"
                "    <property access=\"read\" type=\"i\" name=\"NColumns\"/>\n"
                "    <property access=\"read\" type=\"(so)\" name=\"Caption\">\n"
                "      <annotation value=\"QSpiObjectReference\" name=\"com.trolltech.QtDBus.QtTypeName\"/>\n"
                "    </property>\n"
                "    <property access=\"read\" type=\"(so)\" name=\"Summary\">\n"
                "      <annotation value=\"QSpiObjectReference\" name=\"com.trolltech.QtDBus.QtTypeName\"/>\n"
                "    </property>\n"
                "    <property access=\"read\" type=\"i\" name=\"NSelectedRows\"/>\n"
                "    <property access=\"read\" type=\"i\" name=\"NSelectedColumns\"/>\n"
                "    <method name=\"GetAccessibleAt\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"row\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"column\"/>\n"
                "      <arg direction=\"out\" type=\"(so)\"/>\n"
                "      <annotation value=\"QSpiObjectReference\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
                "    </method>\n"
                "    <method name=\"GetIndexAt\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"row\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"column\"/>\n"
                "      <arg direction=\"out\" type=\"i\"/>\n"
                "    </method>\n"
                "    <method name=\"GetRowAtIndex\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"index\"/>\n"
                "      <arg direction=\"out\" type=\"i\"/>\n"
                "    </method>\n"
                "    <method name=\"GetColumnAtIndex\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"index\"/>\n"
                "      <arg direction=\"out\" type=\"i\"/>\n"
                "    </method>\n"
                "    <method name=\"GetRowDescription\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"row\"/>\n"
                "      <arg direction=\"out\" type=\"s\"/>\n"
                "    </method>\n"
                "    <method name=\"GetColumnDescription\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"column\"/>\n"
                "      <arg direction=\"out\" type=\"s\"/>\n"
                "    </method>\n"
                "    <method name=\"GetRowExtentAt\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"row\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"column\"/>\n"
                "      <arg direction=\"out\" type=\"i\"/>\n"
                "    </method>\n"
                "    <method name=\"GetColumnExtentAt\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"row\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"column\"/>\n"
                "      <arg direction=\"out\" type=\"i\"/>\n"
                "    </method>\n"
                "    <method name=\"GetRowHeader\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"row\"/>\n"
                "      <arg direction=\"out\" type=\"(so)\"/>\n"
                "      <annotation value=\"QSpiObjectReference\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
                "    </method>\n"
                "    <method name=\"GetColumnHeader\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"column\"/>\n"
                "      <arg direction=\"out\" type=\"(so)\"/>\n"
                "      <annotation value=\"QSpiObjectReference\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
                "    </method>\n"
                "    <method name=\"GetSelectedRows\">\n"
                "      <arg direction=\"out\" type=\"ai\"/>\n"
                "      <annotation value=\"QSpiIntList\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
                "    </method>\n"
                "    <method name=\"GetSelectedColumns\">\n"
                "      <arg direction=\"out\" type=\"ai\"/>\n"
                "      <annotation value=\"QSpiIntList\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
                "    </method>\n"
                "    <method name=\"IsRowSelected\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"row\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "    <method name=\"IsColumnSelected\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"column\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "    <method name=\"IsSelected\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"row\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"column\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "    <method name=\"AddRowSelection\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"row\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "    <method name=\"AddColumnSelection\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"column\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "    <method name=\"RemoveRowSelection\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"row\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "    <method name=\"RemoveColumnSelection\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"column\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "    <method name=\"GetRowColumnExtentsAtIndex\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"index\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"row\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"col\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"row_extents\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"col_extents\"/>\n"
                "      <arg direction=\"out\" type=\"b\" name=\"is_selected\"/>\n"
                "    </method>\n"
                "  </interface>\n"
                );

    QLatin1String textIntrospection(
                "  <interface name=\"org.a11y.atspi.Text\">\n"
                "    <property access=\"read\" type=\"i\" name=\"CharacterCount\"/>\n"
                "    <property access=\"read\" type=\"i\" name=\"CaretOffset\"/>\n"
                "    <method name=\"GetText\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"startOffset\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"endOffset\"/>\n"
                "      <arg direction=\"out\" type=\"s\"/>\n"
                "    </method>\n"
                "    <method name=\"SetCaretOffset\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"offset\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "    <method name=\"GetTextBeforeOffset\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"offset\"/>\n"
                "      <arg direction=\"in\" type=\"u\" name=\"type\"/>\n"
                "      <arg direction=\"out\" type=\"s\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"startOffset\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"endOffset\"/>\n"
                "    </method>\n"
                "    <method name=\"GetTextAtOffset\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"offset\"/>\n"
                "      <arg direction=\"in\" type=\"u\" name=\"type\"/>\n"
                "      <arg direction=\"out\" type=\"s\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"startOffset\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"endOffset\"/>\n"
                "    </method>\n"
                "    <method name=\"GetTextAfterOffset\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"offset\"/>\n"
                "      <arg direction=\"in\" type=\"u\" name=\"type\"/>\n"
                "      <arg direction=\"out\" type=\"s\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"startOffset\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"endOffset\"/>\n"
                "    </method>\n"
                "    <method name=\"GetCharacterAtOffset\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"offset\"/>\n"
                "      <arg direction=\"out\" type=\"i\"/>\n"
                "    </method>\n"
                "    <method name=\"GetAttributeValue\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"offset\"/>\n"
                "      <arg direction=\"in\" type=\"s\" name=\"attributeName\"/>\n"
                "      <arg direction=\"out\" type=\"s\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"startOffset\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"endOffset\"/>\n"
                "      <arg direction=\"out\" type=\"b\" name=\"defined\"/>\n"
                "    </method>\n"
                "    <method name=\"GetAttributes\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"offset\"/>\n"
                "      <arg direction=\"out\" type=\"a{ss}\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"startOffset\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"endOffset\"/>\n"
                "      <annotation value=\"QSpiAttributeSet\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
                "    </method>\n"
                "    <method name=\"GetDefaultAttributes\">\n"
                "      <arg direction=\"out\" type=\"a{ss}\"/>\n"
                "      <annotation value=\"QSpiAttributeSet\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
                "    </method>\n"
                "    <method name=\"GetCharacterExtents\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"offset\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"x\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"y\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"width\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"height\"/>\n"
                "      <arg direction=\"in\" type=\"u\" name=\"coordType\"/>\n"
                "    </method>\n"
                "    <method name=\"GetOffsetAtPoint\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"x\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"y\"/>\n"
                "      <arg direction=\"in\" type=\"u\" name=\"coordType\"/>\n"
                "      <arg direction=\"out\" type=\"i\"/>\n"
                "    </method>\n"
                "    <method name=\"GetNSelections\">\n"
                "      <arg direction=\"out\" type=\"i\"/>\n"
                "    <method name=\"GetSelection\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"selectionNum\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"startOffset\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"endOffset\"/>\n"
                "    </method>\n"
                "    <method name=\"AddSelection\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"startOffset\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"endOffset\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "    <method name=\"RemoveSelection\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"selectionNum\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "    <method name=\"SetSelection\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"selectionNum\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"startOffset\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"endOffset\"/>\n"
                "      <arg direction=\"out\" type=\"b\"/>\n"
                "    </method>\n"
                "    <method name=\"GetRangeExtents\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"startOffset\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"endOffset\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"x\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"y\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"width\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"height\"/>\n"
                "      <arg direction=\"in\" type=\"u\" name=\"coordType\"/>\n"
                "    </method>\n"
                "    <method name=\"GetBoundedRanges\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"x\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"y\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"width\"/>\n"
                "      <arg direction=\"in\" type=\"i\" name=\"height\"/>\n"
                "      <arg direction=\"in\" type=\"u\" name=\"coordType\"/>\n"
                "      <arg direction=\"in\" type=\"u\" name=\"xClipType\"/>\n"
                "      <arg direction=\"in\" type=\"u\" name=\"yClipType\"/>\n"
                "      <arg direction=\"out\" type=\"a(iisv)\"/>\n"
                "      <annotation value=\"QSpiRangeList\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
                "    </method>\n"
                "    <method name=\"GetAttributeRun\">\n"
                "      <arg direction=\"in\" type=\"i\" name=\"offset\"/>\n"
                "      <arg direction=\"in\" type=\"b\" name=\"includeDefaults\"/>\n"
                "      <arg direction=\"out\" type=\"a{ss}\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"startOffset\"/>\n"
                "      <arg direction=\"out\" type=\"i\" name=\"endOffset\"/>\n"
                "      <annotation value=\"QSpiAttributeSet\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
                "    </method>\n"
                "    <method name=\"GetDefaultAttributeSet\">\n"
                "      <arg direction=\"out\" type=\"a{ss}\"/>\n"
                "      <annotation value=\"QSpiAttributeSet\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
                "    </method>\n"
                "  </interface>\n"
                );

    QLatin1String valueIntrospection(
                "  <interface name=\"org.a11y.atspi.Value\">\n"
                "    <property access=\"read\" type=\"d\" name=\"MinimumValue\"/>\n"
                "    <property access=\"read\" type=\"d\" name=\"MaximumValue\"/>\n"
                "    <property access=\"read\" type=\"d\" name=\"MinimumIncrement\"/>\n"
                "    <property access=\"readwrite\" type=\"d\" name=\"CurrentValue\"/>\n"
                "    <method name=\"SetCurrentValue\">\n"
                "      <arg direction=\"in\" type=\"d\" name=\"value\"/>\n"
                "    </method>\n"
                "  </interface>\n"
                );
    QPair<QAccessibleInterface*, int> pair;

    if (path != QSPI_OBJECT_PATH_ROOT) {
        QPair<QAccessibleInterfacePointer, int> pair = interfaceFromPath(path);
        if (!pair.first) {
            qWarning() << "WARNING Qt AtSpiAdaptor: Could not find accessible on path: " << path;
            return QString();
        }
    }

    QStringList interfaces = accessibleInterfaces(pair.first, pair.second);

    QString xml;
    xml.append(accessibleIntrospection);

    if (interfaces.contains(ATSPI_DBUS_INTERFACE_COMPONENT))
        xml.append(componentIntrospection);
    if (interfaces.contains(ATSPI_DBUS_INTERFACE_TEXT))
        xml.append(textIntrospection);
    if (interfaces.contains(ATSPI_DBUS_INTERFACE_EDITABLE_TEXT))
        xml.append(editableTextIntrospection);
    if (interfaces.contains(ATSPI_DBUS_INTERFACE_ACTION))
        xml.append(actionIntrospection);
    if (interfaces.contains(ATSPI_DBUS_INTERFACE_TABLE))
        xml.append(tableIntrospection);
    if (interfaces.contains(ATSPI_DBUS_INTERFACE_VALUE))
        xml.append(valueIntrospection);
    if (path == QSPI_OBJECT_PATH_ROOT)
        xml.append(applicationIntrospection);

    return xml;
}

/*!
  When initialized we will send updates, not before this.

  This function also checks which event listeners are registered in the at-spi registry.
  */
void AtSpiAdaptor::setInitialized(bool init)
{
    initialized = init;

    if (!initialized)
        return;

    updateEventListeners();
    bool success = m_dbus->connection().connect("org.a11y.atspi.Registry", "/org/a11y/atspi/registry",
                                               "org.a11y.atspi.Registry", "EventListenerRegistered", this,
                                               SLOT(eventListenerRegistered(QString,QString)));
    success = success && m_dbus->connection().connect("org.a11y.atspi.Registry", "/org/a11y/atspi/registry",
                                               "org.a11y.atspi.Registry", "EventListenerDeregistered", this,
                                               SLOT(eventListenerDeregistered(QString,QString)));
    qDebug() << "Registered event listener change listener: " << success;
}

void AtSpiAdaptor::setBitFlag(const QString &flag)
{
    Q_ASSERT(flag.size());

    // assume we don't get nonsense - look at first letter only
    switch (flag.at(0).toLower().toLatin1()) {
    case 'o': {
        if (flag.size() <= 8) { // Object::
            sendObject = 1;
            break;
        } else { // Object:Foo:Bar
            QString right = flag.mid(7);
            if (false) {
            } else if (right.startsWith(QLatin1String("ActiveDescendantChanged"))) {
                sendObject_active_descendant_changed = 1;
            } else if (right.startsWith(QLatin1String("AttributesChanged"))) {
                sendObject_attributes_changed = 1;
            } else if (right.startsWith(QLatin1String("BoundsChanged"))) {
                sendObject_bounds_changed = 1;
            } else if (right.startsWith(QLatin1String("ChildrenChanged"))) {
                sendObject_children_changed = 1;
            } else if (right.startsWith(QLatin1String("ColumnDeleted"))) {
                sendObject_column_deleted = 1;
            } else if (right.startsWith(QLatin1String("ColumnInserted"))) {
                sendObject_column_inserted = 1;
            } else if (right.startsWith(QLatin1String("ColumnReordered"))) {
                sendObject_column_reordered = 1;
            } else if (right.startsWith(QLatin1String("LinkSelected"))) {
                sendObject_link_selected = 1;
            } else if (right.startsWith(QLatin1String("ModelChanged"))) {
                sendObject_model_changed = 1;
            } else if (right.startsWith(QLatin1String("PropertyChange"))) {
                if (right == QLatin1String("PropertyChange:AccessibleDescription")) {
                    sendObject_property_change_accessible_description = 1;
                } else if (right == QLatin1String("PropertyChange:AccessibleName")) {
                    sendObject_property_change_accessible_name = 1;
                } else if (right == QLatin1String("PropertyChange:AccessibleParent")) {
                    sendObject_property_change_accessible_parent = 1;
                } else if (right == QLatin1String("PropertyChange:AccessibleRole")) {
                    sendObject_property_change_accessible_role = 1;
                } else if (right == QLatin1String("PropertyChange:TableCaption")) {
                    sendObject_property_change_accessible_table_caption = 1;
                } else if (right == QLatin1String("PropertyChange:TableColumnDescription")) {
                    sendObject_property_change_accessible_table_column_description = 1;
                } else if (right == QLatin1String("PropertyChange:TableColumnHeader")) {
                    sendObject_property_change_accessible_table_column_header = 1;
                } else if (right == QLatin1String("PropertyChange:TableRowDescription")) {
                    sendObject_property_change_accessible_table_row_description = 1;
                } else if (right == QLatin1String("PropertyChange:TableRowHeader")) {
                    sendObject_property_change_accessible_table_row_header = 1;
                } else if (right == QLatin1String("PropertyChange:TableSummary")) {
                    sendObject_property_change_accessible_table_summary = 1;
                } else if (right == QLatin1String("PropertyChange:AccessibleValue")) {
                    sendObject_property_change_accessible_value = 1;
                } else {
                    sendObject_property_change = 1;
                }
            } else if (right.startsWith(QLatin1String("RowDeleted"))) {
                sendObject_row_deleted = 1;
            } else if (right.startsWith(QLatin1String("RowInserted"))) {
                sendObject_row_inserted = 1;
            } else if (right.startsWith(QLatin1String("RowReordered"))) {
                sendObject_row_reordered = 1;
            } else if (right.startsWith(QLatin1String("SelectionChanged"))) {
                sendObject_selection_changed = 1;
            } else if (right.startsWith(QLatin1String("StateChanged"))) {
                sendObject_state_changed = 1;
            } else if (right.startsWith(QLatin1String("TextAttributesChanged"))) {
                sendObject_text_attributes_changed = 1;
            } else if (right.startsWith(QLatin1String("TextBoundsChanged"))) {
                sendObject_text_bounds_changed = 1;
            } else if (right.startsWith(QLatin1String("TextCaretMoved"))) {
                sendObject_text_caret_moved = 1;
            } else if (right.startsWith(QLatin1String("TextChanged"))) {
                sendObject_text_changed = 1;
            } else if (right.startsWith(QLatin1String("TextSelectionChanged"))) {
                sendObject_text_selection_changed = 1;
            } else if (right.startsWith(QLatin1String("ValueChanged"))) {
                sendObject_value_changed = 1;
            } else if (right.startsWith(QLatin1String("VisibleDataChanged"))) {
                sendObject_visible_data_changed = 1;
            } else {
                qWarning() << "WARNING: subscription string not handled:" << flag;
            }
        }
        break;
    }
    case 'w': { // window
        if (flag.size() < 7) {
            sendWindow = 1;
        } else { // object:Foo:Bar
            QString right = flag.mid(7);
            if (false) {
            } else if (right.startsWith(QLatin1String("Activate"))) {
                sendWindow_activate = 1;
            } else if (right.startsWith(QLatin1String("Close"))) {
                sendWindow_close= 1;
            } else if (right.startsWith(QLatin1String("Create"))) {
                sendWindow_create = 1;
            } else if (right.startsWith(QLatin1String("Deactivate"))) {
                sendWindow_deactivate = 1;
            } else if (right.startsWith(QLatin1String("Lower"))) {
                sendWindow_lower = 1;
            } else if (right.startsWith(QLatin1String("Maximize"))) {
                sendWindow_maximize = 1;
            } else if (right.startsWith(QLatin1String("Minimize"))) {
                sendWindow_minimize = 1;
            } else if (right.startsWith(QLatin1String("Move"))) {
                sendWindow_move = 1;
            } else if (right.startsWith(QLatin1String("Raise"))) {
                sendWindow_raise = 1;
            } else if (right.startsWith(QLatin1String("Reparent"))) {
                sendWindow_reparent = 1;
            } else if (right.startsWith(QLatin1String("Resize"))) {
                sendWindow_resize = 1;
            } else if (right.startsWith(QLatin1String("Restore"))) {
                sendWindow_restore = 1;
            } else if (right.startsWith(QLatin1String("Restyle"))) {
                sendWindow_restyle = 1;
            } else if (right.startsWith(QLatin1String("Shade"))) {
                sendWindow_shade = 1;
            } else if (right.startsWith(QLatin1String("Unshade"))) {
                sendWindow_unshade = 1;
            } else if (right.startsWith(QLatin1String("DesktopCreate"))) {
                // ignore this one
            } else if (right.startsWith(QLatin1String("DesktopDestroy"))) {
                // ignore this one
            } else {
                qWarning() << "WARNING: subscription string not handled:" << flag;
            }
        }
        break;
    }
    case 'f': {
        sendFocus = 1;
        break;
    }
    case 'd': { // document is not implemented
        break;
    }
    case 't': { // terminal is not implemented
        break;
    }
    case 'm': { // mouse* is handled in a different way by the gnome atspi stack
        break;
    }
    default:
        qWarning() << "WARNING: subscription string not handled:" << flag;
    }
}

/*!
  Checks via dbus which events should be sent.
  */
void AtSpiAdaptor::updateEventListeners()
{
//    QStringList watchedExpressions;
    QDBusMessage m = QDBusMessage::createMethodCall("org.a11y.atspi.Registry",
                                                    "/org/a11y/atspi/registry",
                                                    "org.a11y.atspi.Registry", "GetRegisteredEvents");
    QDBusMessage listenersReply = m_dbus->connection().call(m);
    if (listenersReply.type() == QDBusMessage::ReplyMessage) {
        const QVariant foo = listenersReply.arguments().at(0);
        const QDBusArgument a = foo.value<QDBusArgument>();
        Q_ASSERT(a.currentSignature() == "a(ss)");
        a.beginArray();
        while (!a.atEnd()) {
            a.beginStructure();
            QString listenerAddress;
            a >> listenerAddress;
            QString watchedExpression;
            a >> watchedExpression;
            setBitFlag(watchedExpression);
//            watchedExpressions.append(watchedExpression);
            a.endStructure();
        }
        a.endArray();
//        qDebug() << "Currently active listeners: " << watchedExpressions;
    } else {
        qWarning() << "Could not query active accessibility event listeners.";
    }
}

void AtSpiAdaptor::eventListenerDeregistered(const QString &/*bus*/, const QString &/*path*/)
{
//    qDebug() << "AtSpiAdaptor::eventListenerDeregistered: " << bus << path;
    updateEventListeners();
}

void AtSpiAdaptor::eventListenerRegistered(const QString &/*bus*/, const QString &/*path*/)
{
//    qDebug() << "AtSpiAdaptor::eventListenerRegistered: " << bus << path;
    updateEventListeners();
}

/*!
  This slot needs to get called when a \a window has be activated or deactivated (become focused).
  When \a active is true, the window just received focus, otherwise it lost the focus.
  */
void AtSpiAdaptor::windowActivated(QObject* window, bool active)
{
    if (!(sendWindow || sendWindow_activate))
        return;

    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(window);
    QString windowTitle = iface->text(QAccessible::Name, 0);
    delete iface;

    QDBusVariant data;
    data.setVariant(windowTitle);

    QVariantList args = packDBusSignalArguments(QString(), 0, 0, QVariant::fromValue(data));

    QString status = active ? QLatin1String("Activate") : QLatin1String("Deactivate");
    QString path = pathForObject(window);
    sendDBusSignal(path, ATSPI_DBUS_INTERFACE_EVENT_WINDOW, status, args);

    QVariantList stateArgs = packDBusSignalArguments(QLatin1String("active"), active ? 1 : 0, 0, variantForPath(path));
    sendDBusSignal(path, QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_OBJECT),
                   QLatin1String("StateChanged"), stateArgs);
}

QVariantList AtSpiAdaptor::packDBusSignalArguments(const QString &type, int data1, int data2, const QVariant &variantData) const
{
    QVariantList arguments;
    arguments << type << data1 << data2 << variantData
              << QVariant::fromValue(QSpiObjectReference(m_dbus->connection(), QDBusObjectPath(QSPI_OBJECT_PATH_ROOT)));
    return arguments;
}

QVariant AtSpiAdaptor::variantForPath(const QString &path) const
{
    QDBusVariant data;
    data.setVariant(QVariant::fromValue(QSpiObjectReference(m_dbus->connection(), QDBusObjectPath(path))));
    return QVariant::fromValue(data);
}

bool AtSpiAdaptor::sendDBusSignal(const QString &path, const QString &interface, const QString &signalName, const QVariantList &arguments) const
{
    QDBusMessage message = QDBusMessage::createSignal(path, interface, signalName);
    message.setArguments(arguments);
    return m_dbus->connection().send(message);
}

QPair<QAccessibleInterfacePointer, int> AtSpiAdaptor::interfaceFromPath(const QString& dbusPath) const
{
    int index = 0;

    if (dbusPath == QSPI_OBJECT_PATH_ROOT) {
        QAccessibleInterfacePointer interface = QAccessibleInterfacePointer(QAccessible::queryAccessibleInterface(qApp));
        return QPair<QAccessibleInterfacePointer, int>(interface, index);
    }

    QStringList parts = dbusPath.split('/');
    if (parts.size() <= 5) {
        qWarning() << "invalid path: " << dbusPath;
        return QPair<QAccessibleInterfacePointer, int>(QAccessibleInterfacePointer(), 0);
    }

    QString objectString = parts.at(5);
    quintptr uintptr = objectString.toULongLong();

    if (uintptr && m_handledObjects.contains(uintptr)) {
        // We found the pointer, check if it's still valid:
        if (m_handledObjects[uintptr].data() != 0) {
            QObject* object = reinterpret_cast<QObject*>(uintptr);

            QAccessibleInterfacePointer interface = QAccessibleInterfacePointer(QAccessible::queryAccessibleInterface(object));
            QAccessibleInterfacePointer child;

            for (int i = 6; i < parts.size(); ++i) {
                QAccessibleInterface *childInterface;
                index = interface->navigate(QAccessible::Child, parts.at(i).toInt(), &childInterface);
                child = QAccessibleInterfacePointer(childInterface);
                if (index == 0)
                    interface = child;
            }
            return QPair<QAccessibleInterfacePointer, int>(interface, index);

        } else {
            m_handledObjects.remove(uintptr);
        }
    }
    return QPair<QAccessibleInterfacePointer, int>(QAccessibleInterfacePointer(), 0);
}


/*!
    This function gets called when Qt notifies about accessibility updates.
*/
void AtSpiAdaptor::notify(int reason, QAccessibleInterface *interface, int child)
{
    Q_ASSERT(interface);

    if (!interface->isValid()) {
        //spiBridge->removeAdaptor(this);
        // FIXME announce that this thing is dead? will it ever happen?
        Q_ASSERT_X(0, "", "Got an update for an invalid inteface. Investigate this.");
        return;
    }

    if (reason == QAccessible::ObjectShow && interface->object()) {
        if (child != 0) {
            qWarning() << "State for child changed: " << interface->object() << child;
            return;
        }
        int state = interface->state(child);
        interface->object()->setProperty(ACCESSIBLE_LAST_STATE, state);
    }
    // Saving of the last text should not be skipped, even when initialized is still false.
    if (reason == QAccessible::ObjectShow && interface->textInterface()) {
        Q_ASSERT(interface->object());
        QString text = interface->textInterface()->text(0, interface->textInterface()->characterCount());
        interface->object()->setProperty(ACCESSIBLE_LAST_TEXT, text);
    }

    if (!initialized)
        return;

    switch (reason) {
    case QAccessible::ObjectCreated:
        if (sendObject || sendObject_children_changed)
            notifyAboutCreation(interface, child);
        break;
    case QAccessible::ObjectShow: {
        if (sendObject || sendObject_state_changed) {
            QString path = pathForInterface(interface, child);
            QVariantList stateArgs = packDBusSignalArguments(QLatin1String("showing"), 1, 0, variantForPath(path));
            sendDBusSignal(path, QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_OBJECT),
                           QLatin1String("StateChanged"), stateArgs);
        }
        break;
    }
    case QAccessible::ObjectHide: {
        if (sendObject || sendObject_state_changed) {
            QString path = pathForInterface(interface, child);
            QVariantList stateArgs = packDBusSignalArguments(QLatin1String("showing"), 0, 0, variantForPath(path));
            sendDBusSignal(path, QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_OBJECT),
                           QLatin1String("StateChanged"), stateArgs);
        }
        break;
    }
    case QAccessible::ObjectDestroyed: {
        if (sendObject || sendObject_state_changed)
            notifyAboutDestruction(interface, child);
        break;
    }
    case QAccessible::NameChanged: {
        if (sendObject || sendObject_property_change || sendObject_property_change_accessible_name) {
            QString path = pathForInterface(interface, child);
            QVariantList args = packDBusSignalArguments(QLatin1String("accessible-name"), 0, 0, variantForPath(path));
            sendDBusSignal(path, QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_OBJECT),
                           QLatin1String("PropertyChange"), args);
        }
        break;
    }
    case QAccessible::DescriptionChanged: {
        if (sendObject || sendObject_property_change || sendObject_property_change_accessible_description) {
            QString path = pathForInterface(interface, child);
            QVariantList args = packDBusSignalArguments(QLatin1String("accessible-description"), 0, 0, variantForPath(path));
            sendDBusSignal(path, QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_OBJECT),
                           QLatin1String("PropertyChange"), args);
        }
        break;
    }
    case QAccessible::Focus: {
        if(sendFocus)
            sendFocusChanged(interface, child);
        break;
    }
    case QAccessible::TextUpdated: {
        if (sendObject || sendObject_text_changed) {
            Q_ASSERT(interface->textInterface());
            QString path = pathForInterface(interface, child);
            // at-spi doesn't have a text updated/changed, so remove all and re-add the new text
            QString oldText = interface->object()->property(ACCESSIBLE_LAST_TEXT).toString();

            QDBusVariant data;
            data.setVariant(QVariant::fromValue(oldText));
            QVariantList args = packDBusSignalArguments(QLatin1String("delete"), 0, oldText.length(), variantForPath(path));
            sendDBusSignal(path, QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_OBJECT),
                           QLatin1String("TextChanged"), args);

            QString text = interface->textInterface()->text(0, interface->textInterface()->characterCount());
            data.setVariant(QVariant::fromValue(text));
            args = packDBusSignalArguments(QLatin1String("insert"), 0, text.length(), variantForPath(path));
            sendDBusSignal(path, QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_OBJECT),
                           QLatin1String("TextChanged"), args);

            interface->object()->setProperty(ACCESSIBLE_LAST_TEXT, text);
        }
        break;
    }
    case QAccessible::TextCaretMoved: {
        if (sendObject || sendObject_text_caret_moved) {
            Q_ASSERT(interface->textInterface());
            QString path = pathForInterface(interface, child);
            QDBusVariant cursorData;
            int pos = interface->textInterface()->cursorPosition();
            cursorData.setVariant(QVariant::fromValue(pos));
            QVariantList args = packDBusSignalArguments(QString(), pos, 0, QVariant::fromValue(cursorData));
            sendDBusSignal(path, QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_OBJECT),
                           QLatin1String("TextCaretMoved"), args);
        }
        break;
    }
    case QAccessible::ValueChanged: {
            if (sendObject || sendObject_value_changed) {
                Q_ASSERT(interface->valueInterface());
                QString path = pathForInterface(interface, child);
                QVariantList args = packDBusSignalArguments(QLatin1String("accessible-value"), 0, 0, variantForPath(path));
                sendDBusSignal(path, QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_OBJECT),
                               QLatin1String("PropertyChange"), args);
            }
            break;
    }

    case QAccessible::Selection: {
        QString path = pathForInterface(interface, child);
        int selected = (interface->state(child) & QAccessible::Selected) ? 1 : 0;
        QVariantList stateArgs = packDBusSignalArguments(QLatin1String("selected"), selected, 0, variantForPath(path));
        sendDBusSignal(path, QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_OBJECT),
                       QLatin1String("StateChanged"), stateArgs);
    }

    case QAccessible::StateChanged: {
        if (sendObject || sendObject_state_changed) {
            if (child != 0) {
                qWarning() << "State for child changed: " << interface->object() << child;
                return;
            }

            QAccessible::State oldState = (QAccessible::State) interface->object()->property(ACCESSIBLE_LAST_STATE).toUInt();
            QAccessible::State newState = interface->state(child);
            //qDebug() << "StateChanged: old: " << oldState << " new: " << newState << " xor: " << (oldState^newState);
            if ((oldState^newState) & QAccessible::Checked) {
                int checked = (newState & QAccessible::Checked) ? 1 : 0;
                QString path = pathForInterface(interface, child);
                QVariantList args = packDBusSignalArguments(QLatin1String("checked"), checked, 0, variantForPath(path));
                sendDBusSignal(path, QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_OBJECT),
                               QLatin1String("StateChanged"), args);
            }
            interface->object()->setProperty(ACCESSIBLE_LAST_STATE, (uint)newState);
        }
        break;
    }
//    case QAccessible::TableModelChanged: {
//        // This is rather evil. We don't send data and hope that at-spi fetches the right child.
//        // This hack fails when a row gets removed and a different one added in its place.
//        QDBusVariant data;
//        emit ChildrenChanged("add", 0, 0, data, spiBridge->getRootReference());
//        break;
//    }
        //    case QAccessible::TableModelChanged:
        //        QAccessible2::TableModelChange change = interface->table2Interface()->modelChange();
        //        // assume we should reset if everything is 0
        //        if (change.firstColumn == 0 && change.firstRow == 0 && change.lastColumn == 0 && change.lastRow == 0) {
        //            notifyAboutDestruction(accessible);
        //            notifyAboutCreation(accessible);
        //        }
        //        break;

    case QAccessible::ParentChanged:
        qDebug() << "FIXME: handle parent changed.";
        break;
    case QAccessible::DialogStart:
        qDebug() << "FIXME: handle dialog start.";
        break;
    case QAccessible::DialogEnd:
        qDebug() << "FIXME: handle dialog end.";
        break;
    case QAccessible::TableModelChanged:
        // For now ignore this event, should be handled together with active descendant changed
        break;
    default:
        qWarning() << "QSpiAccessible::accessibleEvent not handled: " << QString::number(reason, 16)
                   << " obj: " << interface->object()
                   << ((interface->isValid() && interface->object()) ? interface->object()->objectName() : " invalid interface!");
        break;
    }
}

void AtSpiAdaptor::sendFocusChanged(QAccessibleInterface *interface, int child) const
{
    static QString lastFocusPath;
    // "remove" old focus
    if (!lastFocusPath.isEmpty()) {
        QVariantList stateArgs = packDBusSignalArguments(QLatin1String("focused"), 0, 0, variantForPath(lastFocusPath));
        sendDBusSignal(lastFocusPath, QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_OBJECT),
                       QLatin1String("StateChanged"), stateArgs);
    }
    // send new focus
    {
        QString path = pathForInterface(interface, child);

        QVariantList stateArgs = packDBusSignalArguments(QLatin1String("focused"), 1, 0, variantForPath(path));
        sendDBusSignal(path, QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_OBJECT),
                       QLatin1String("StateChanged"), stateArgs);

        QVariantList focusArgs = packDBusSignalArguments(QString(), 0, 0, variantForPath(path));
        sendDBusSignal(path, QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_FOCUS),
                       QLatin1String("Focus"), focusArgs);
        lastFocusPath = path;
    }
}

void AtSpiAdaptor::notifyAboutCreation(QAccessibleInterface *interface, int child) const
{
//    // say hello to d-bus
//    cache->emitAddAccessible(accessible->getCacheItem());

    // notify about the new child of our parent
    QAccessibleInterface *parent = accessibleParent(interface, child);
    if (!parent) {
        qWarning() << "AtSpiAdaptor::notifyAboutCreation: Could not find parent for " << interface->object() << child;
        return;
    }
    QString path = pathForInterface(interface, child);
    int childCount = parent->childCount();
    QString parentPath = pathForInterface(parent, 0);
    QVariantList args = packDBusSignalArguments(QLatin1String("add"), childCount, 0, variantForPath(path));
    sendDBusSignal(parentPath, ATSPI_DBUS_INTERFACE_EVENT_OBJECT, "ChildrenChanged", args);
    delete parent;
}

void AtSpiAdaptor::notifyAboutDestruction(QAccessibleInterface *interface, int child) const
{
    if (!interface->isValid())
        return;

    QAccessibleInterface *parent = accessibleParent(interface, child);
    if (!parent) {
        qWarning() << "AtSpiAdaptor::notifyAboutDestruction: Could not find parent for " << interface->object() << child;
        return;
    }
    QString path = pathForInterface(interface, child);

    // this is in the destructor. we have no clue which child we used to be.
    // FIXME
    int childIndex = -1;
    //    if (child) {
    //        childIndex = child;
    //    } else {
    //        childIndex = parent->indexOfChild(interface);
    //    }

    QString parentPath = pathForInterface(parent, 0, true);
    QVariantList args = packDBusSignalArguments(QLatin1String("remove"), childIndex, 0, variantForPath(path));
    sendDBusSignal(parentPath, ATSPI_DBUS_INTERFACE_EVENT_OBJECT, "ChildrenChanged", args);
    delete parent;
}

/*!
  Handle incoming DBus message.
  This function dispatches the dbus message to the right interface handler.
  */
bool AtSpiAdaptor::handleMessage(const QDBusMessage &message, const QDBusConnection &connection)
{
    // get accessible interface
    QPair<QAccessibleInterfacePointer, int> accessible = interfaceFromPath(message.path());
    if (!(accessible.first)) {
        qWarning() << "WARNING Qt AtSpiAdaptor: Could not find accessible on path: " << message.path();
        return false;
    }

    QString interface = message.interface();
    QString function = message.member();

    if (function == "Introspect") {
        //introspect(message.path());
        return false;
    }

    // handle properties like regular functions
    if (interface == "org.freedesktop.DBus.Properties") {
        interface = message.arguments().at(0).toString();
        // Get/Set + Name
        function = message.member() + message.arguments().at(1).toString();
    }

    // switch interface to call
    if (interface == ATSPI_DBUS_INTERFACE_ACCESSIBLE) {
        return accessibleInterface(accessible.first.data(), accessible.second, function, message, connection);
    } else if (interface == ATSPI_DBUS_INTERFACE_APPLICATION) {
        return applicationInterface(accessible.first.data(), accessible.second, function, message, connection);
    } else if (interface == ATSPI_DBUS_INTERFACE_COMPONENT) {
        return componentInterface(accessible.first.data(), accessible.second, function, message, connection);
    } else if (interface == ATSPI_DBUS_INTERFACE_ACTION) {
        return actionInterface(accessible.first.data(), accessible.second, function, message, connection);
    } else if (interface == ATSPI_DBUS_INTERFACE_TEXT) {
        return textInterface(accessible.first.data(), accessible.second, function, message, connection);
    } else if (interface == ATSPI_DBUS_INTERFACE_EDITABLE_TEXT) {
        return editableTextInterface(accessible.first.data(), accessible.second, function, message, connection);
    } else if (interface == ATSPI_DBUS_INTERFACE_VALUE) {
        return valueInterface(accessible.first.data(), accessible.second, function, message, connection);
    } else if (interface == ATSPI_DBUS_INTERFACE_TABLE) {
        return tableInterface(accessible.first.data(), accessible.second, function, message, connection);
    } else {
        qDebug() << "AtSpiAdaptor::handleMessage " << message.path() << interface << function;
    }
    return false;
}

// Application
bool AtSpiAdaptor::applicationInterface(QAccessibleInterface *interface, int, const QString &function, const QDBusMessage &message, const QDBusConnection &connection)
{
    if (message.path() != ATSPI_DBUS_PATH_ROOT) {
        qWarning() << "WARNING Qt AtSpiAdaptor: Could not find application interface for: " << message.path() << interface;
        return false;
    }

    if (function == "SetId") {
        Q_ASSERT(message.signature() == "ssv");
        QVariant value = qvariant_cast<QDBusVariant>(message.arguments().at(2)).variant();

        m_applicationId = value.toInt();
        return true;

    } else if (function == "GetId") {
        Q_ASSERT(message.signature() == "ss");
        QDBusMessage reply = message.createReply(QVariant::fromValue(QDBusVariant(m_applicationId)));
        return connection.send(reply);
    } else if (function == "GetToolkitName") {
        Q_ASSERT(message.signature() == "ss");
        QDBusMessage reply = message.createReply(QVariant::fromValue(QDBusVariant(QLatin1String("Qt"))));
        return connection.send(reply);
    } else {
        qDebug() << "AtSpiAdaptor::applicationInterface " << message.path() << interface << function;
    }

    return false;
}

/*!
  Register this application as accessible on the accessibility DBus.
  */
void AtSpiAdaptor::registerApplication()
{
    SocketProxy *registry;
    registry = new SocketProxy(QSPI_REGISTRY_NAME,
                               QSPI_OBJECT_PATH_ROOT, m_dbus->connection());

    QDBusPendingReply<QSpiObjectReference> reply;
    QSpiObjectReference ref = QSpiObjectReference(m_dbus->connection(), QDBusObjectPath(QSPI_OBJECT_PATH_ROOT));
    reply = registry->Embed(ref);
    reply.waitForFinished();
    if (reply.isValid ()) {
        const QSpiObjectReference &socket = reply.value();
        accessibilityRegistry = QSpiObjectReference(socket);
    } else {
        qWarning() << "Error in contacting registry";
        qWarning() << reply.error().name();
        qWarning() << reply.error().message();
    }
    delete registry;
}

// Accessible
bool AtSpiAdaptor::accessibleInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection)
{
    if (child < 0) {
        qWarning() << "AtSpiAdaptor::accessibleInterface called with child<0. FIXME";
        return false;
    }

    if (function == "GetRole") {
        sendReply(connection, message, (uint) qSpiRoleMapping[interface->role(child)].spiRole());
    } else if (function == "GetName") {
        sendReply(connection, message, QVariant::fromValue(QDBusVariant(interface->text(QAccessible::Name, child))));
    } else if (function == "GetRoleName") {
        sendReply(connection, message, qSpiRoleMapping[interface->role(child)].name());
    } else if (function == "GetLocalizedRoleName") {
        sendReply(connection, message, QVariant::fromValue(qSpiRoleMapping[interface->role(child)].localizedName()));
    } else if (function == "GetChildCount") {
        int childCount = child ? 0 : interface->childCount();
        sendReply(connection, message, QVariant::fromValue(QDBusVariant(childCount)));
    } else if (function == "GetIndexInParent") {
        int childIndex = -1;
        if (child) {
            childIndex = child;
        } else {
            QAccessibleInterface *parent = accessibleParent(interface, child);
            if (parent)
                childIndex = parent->indexOfChild(interface);
            delete parent;
        }
        sendReply(connection, message, childIndex);
    } else if (function == "GetParent") {
        QString path;
        QAccessibleInterface *parent = accessibleParent(interface, child);
        if (!parent || parent->role(0) == QAccessible::Application) {
            path = QSPI_OBJECT_PATH_ROOT;
        } else {
            path = pathForInterface(parent, 0);
        }
        if (parent != interface)
            delete parent;

        // Parent is a property, so it needs to be wrapped inside an extra variant.
        sendReply(connection, message, QVariant::fromValue(
                      QDBusVariant(QVariant::fromValue(QSpiObjectReference(connection, QDBusObjectPath(path))))));
    } else if (function == "GetChildAtIndex") {
        Q_ASSERT(child == 0); // Don't support child of virtual child
        int index = message.arguments().first().toInt() + 1;
        sendReply(connection, message, QVariant::fromValue(
                      QSpiObjectReference(connection, QDBusObjectPath(pathForInterface(interface, index)))));
    } else if (function == "GetInterfaces") {
        sendReply(connection, message, accessibleInterfaces(interface, child));
    } else if (function == "GetDescription") {
        sendReply(connection, message, QVariant::fromValue(QDBusVariant(interface->text(QAccessible::Description, child))));
    } else if (function == "GetState") {
        quint64 spiState = spiStatesFromQState(interface->state(child));
        if (interface->table2Interface()) {
            setSpiStateBit(&spiState, ATSPI_STATE_MANAGES_DESCENDANTS);
        }
        if (interface->object() && interface->object()->isWidgetType()) {
            QWidget *w = qobject_cast<QWidget*>(interface->object());
            if (w && w->topLevelWidget() && w->isActiveWindow()) {
                setSpiStateBit(&spiState, ATSPI_STATE_ACTIVE);
            }
        }
        sendReply(connection, message,
                  QVariant::fromValue(spiStateSetFromSpiStates(spiState)));
    } else if (function == "GetAttributes") {
        sendReply(connection, message, QVariant::fromValue(QSpiAttributeSet()));
    } else if (function == "GetRelationSet") {
        sendReply(connection, message, QVariant::fromValue(relationSet(interface, child, connection)));
    } else if (function == "GetApplication") {
        sendReply(connection, message, QVariant::fromValue(
                      QSpiObjectReference(connection, QDBusObjectPath(QSPI_OBJECT_PATH_ROOT))));
    } else if (function == "GetChildren") {
        QSpiObjectReferenceArray children;
        for (int i = 0; i < interface->childCount(); ++i) {
            QSpiObjectReference ref(connection, QDBusObjectPath(pathForInterface(interface, i + 1)));
            children << ref;
        }
        connection.send(message.createReply(QVariant::fromValue(children)));
    } else {
        qWarning() << "WARNING: AtSpiAdaptor::handleMessage does not implement " << function << message.path();
        return false;
    }
    return true;
}

QStringList AtSpiAdaptor::accessibleInterfaces(QAccessibleInterface *interface, int index) const
{
    QStringList ifaces;
#ifdef ACCESSIBLE_CREATION_DEBUG
    qDebug() << "ACCESSIBLE: " << interface->object();
#endif
    ifaces << ATSPI_DBUS_INTERFACE_ACCESSIBLE;

    if (    (!interface->rect(index).isEmpty()) ||
            (interface->object() && interface->object()->isWidgetType()) ||
            (interface->role(index) == QAccessible::ListItem) ||
            (interface->role(index) == QAccessible::Cell) ||
            (interface->role(index) == QAccessible::TreeItem) ||
            (interface->role(index) == QAccessible::Row) ||
            (interface->object() && interface->object()->inherits("QSGItem"))
            ) {
        ifaces << ATSPI_DBUS_INTERFACE_COMPONENT;
        }
#ifdef ACCESSIBLE_CREATION_DEBUG
    else {
        qDebug() << " IS NOT a component";
    }
#endif

    ifaces << ATSPI_DBUS_INTERFACE_ACTION;

    if (!index) {
        if (interface->textInterface()) {
            ifaces << ATSPI_DBUS_INTERFACE_TEXT;
            // Cache the last text?
            // oldText = interface->textInterface()->text(0, interface->textInterface()->characterCount());
        }

        if (interface->editableTextInterface())
            ifaces << ATSPI_DBUS_INTERFACE_EDITABLE_TEXT;

        if (interface->valueInterface())
            ifaces << ATSPI_DBUS_INTERFACE_VALUE;

        if (interface->table2Interface())
            ifaces << ATSPI_DBUS_INTERFACE_TABLE;
    }

    // Do we need to cache the state?
    //    state = interface->state(childIndex());

    return ifaces;
}

QSpiRelationArray AtSpiAdaptor::relationSet(QAccessibleInterface *interface, int child, const QDBusConnection &connection) const
{
    QSpiRelationArray relations;
    if (child == 0) {
        qDebug() << "AtSpiAdaptor::relationSet currently has a problem with child ids.";
        // FIXME for example trees need to express their child relations here.
        return relations;
    }

    const QAccessible::RelationFlag relationsToCheck[] = {QAccessible::Label, QAccessible::Labelled, QAccessible::Controller, QAccessible::Controlled, static_cast<QAccessible::RelationFlag>(-1)};
    const AtspiRelationType relationTypes[] = {ATSPI_RELATION_LABELLED_BY, ATSPI_RELATION_LABEL_FOR, ATSPI_RELATION_CONTROLLED_BY, ATSPI_RELATION_CONTROLLER_FOR};

    for (int i = 0; relationsToCheck[i] >= 0; i++) {
        QList<QSpiObjectReference> related;
        int navigateResult = 1;

        for (int j = 1; navigateResult >= 0; j++) {
            QAccessibleInterface *target;
            navigateResult = interface->navigate(relationsToCheck[i], j, &target);

            if (navigateResult >= 0) {
                QDBusObjectPath path = QDBusObjectPath(pathForInterface(target ? target : interface, navigateResult));
                related.append(QSpiObjectReference(connection, path));
                delete target;
            }
        }
        if (!related.isEmpty())
            relations.append(QSpiRelationArrayEntry(relationTypes[i], related));
    }
    return relations;
}

void AtSpiAdaptor::sendReply(const QDBusConnection &connection, const QDBusMessage &message, const QVariant &argument) const
{
    QDBusMessage reply = message.createReply(argument);
    connection.send(reply);
}

QAccessibleInterface *AtSpiAdaptor::accessibleParent(QAccessibleInterface *iface, int child) const
{
    if (child)
        return iface;
    QAccessibleInterface *parent = 0;
    iface->navigate(QAccessible::Ancestor, 1, &parent);
    return parent;
}

QString AtSpiAdaptor::pathForObject(QObject *object) const
{
    Q_ASSERT(object);

    if (object->metaObject()->className() == QLatin1String("QAction")) {
        qDebug() << "AtSpiAdaptor::pathForObject: warning: creating path with QAction as object.";
    }
    quintptr uintptr = reinterpret_cast<quintptr>(object);
    if (!m_handledObjects.contains(uintptr))
        m_handledObjects[uintptr] = QWeakPointer<QObject>(object);
    return QSPI_OBJECT_PATH_PREFIX + QString::number(uintptr);
}

QString AtSpiAdaptor::pathForInterface(QAccessibleInterface *interface, int childIndex, bool inDestructor) const
{
    // Try to navigate to the child. If we get a proper interface, use it since it might have an object associated.
    QAccessibleInterface* childInterface = 0;
    if (childIndex) {
        int ret = interface->navigate(QAccessible::Child, childIndex, &childInterface);
        if (ret == 0 && childInterface) {
            // This is an ugly hack for QAction. It cannot create adaptors from the QObject.
            QAccessibleInterface *tmp = QAccessible::queryAccessibleInterface(childInterface->object());
            if (tmp) {
                interface = childInterface;
                childIndex = 0;
                delete tmp;
            }
        }
    }

    QString path;
    QAccessibleInterface* interfaceWithObject = interface;
    while(!interfaceWithObject->object()) {
        QAccessibleInterface* parentInterface;
        interfaceWithObject->navigate(QAccessible::Ancestor, 1, &parentInterface);
        Q_ASSERT(parentInterface->isValid());
        int index = parentInterface->indexOfChild(interfaceWithObject);
        //Q_ASSERT(index >= 0);
        // FIXME: This should never happen!
        if (index < 0) {

            index = 999;
            path.prepend("/BROKEN_OBJECT_HIERARCHY");
            qWarning() << "Object claims to have child that we cannot navigate to. FIX IT!" << parentInterface->object();

            qDebug() << "Original interface: " << interface->object() << index;
            qDebug() << "Parent interface: " << parentInterface->object() << " childcount:" << parentInterface->childCount();
            QObject* p = parentInterface->object();
            qDebug() << p->children();

            QAccessibleInterface* tttt;
            int id = parentInterface->navigate(QAccessible::Child, 1, &tttt);
            qDebug() << "Nav child: " << id << tttt->object();
        }
        path.prepend('/' + QString::number(index));
        interfaceWithObject = parentInterface;
    }
    quintptr uintptr = reinterpret_cast<quintptr>(interfaceWithObject->object());
    path.prepend(QSPI_OBJECT_PATH_PREFIX + QString::number(uintptr));

    if (childIndex > 0) {
        path.append('/' + QString::number(childIndex));
    }
    if (!inDestructor && !m_handledObjects.contains(uintptr))
        m_handledObjects[uintptr] = QWeakPointer<QObject>(interfaceWithObject->object());
    delete childInterface;
    return path;
}

// Component
static QAccessibleInterface *getWindow(QAccessibleInterface* interface)
{
    QAccessibleInterface *current=NULL, *tmp=NULL;

    interface->navigate(QAccessible::Ancestor, 0, &current);

    while (current &&
           current->role(0) != QAccessible::Window &&
           current->role(0) != QAccessible::Application)
    {
        tmp = NULL;
        current->navigate (QAccessible::Ancestor, 1, &tmp);
        current = tmp;
    }

    if (current) {
        return current;
    } else {
        return NULL;
    }
}

static QRect getRelativeRect(QAccessibleInterface* interface, int child)
{
    QAccessibleInterface *window;
    QRect wr, cr;

    cr = interface->rect(child);

    window = getWindow(interface);
    if (window)
    {
        wr = window->rect(child);

        cr.setX(cr.x() - wr.x());
        cr.setY(cr.x() - wr.y());
    }
    return cr;
}

bool AtSpiAdaptor::componentInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection)
{
    if (function == "Contains") {
        bool ret = false;
        int x = message.arguments().at(0).toInt();
        int y = message.arguments().at(1).toInt();
        uint coordType = message.arguments().at(2).toUInt();
        if (coordType == ATSPI_COORD_TYPE_SCREEN)
            ret = interface->rect(child).contains(x, y);
        else
            ret = getRelativeRect(interface, child).contains(x, y);
        sendReply(connection, message, ret);
    } else if (function == "GetAccessibleAtPoint") {
        int x = message.arguments().at(0).toInt();
        int y = message.arguments().at(1).toInt();
        uint coordType = message.arguments().at(2).toUInt();
        Q_UNUSED (coordType) // FIXME

        // Grab the top level widget. For complex widgets we want to return a child
        // at the right position instead.
        QWidget* w = qApp->widgetAt(x, y);
        if (w) {
            QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(w);
            if (!iface) {
                return false;
            }
            int childIndex = iface->childAt(x, y);
            if (childIndex < 0)
                childIndex = 0;
            QString path = pathForInterface(iface, childIndex);
            sendReply(connection, message, QVariant::fromValue(
                          QSpiObjectReference(connection, QDBusObjectPath(path))));
        } else {
            sendReply(connection, message, QVariant::fromValue(
                          QSpiObjectReference(connection, QDBusObjectPath(ATSPI_DBUS_PATH_NULL))));
        }
    } else if (function == "GetAlpha") {
        sendReply(connection, message, (double) 1.0);
    } else if (function == "GetExtents") {
        uint coordType = message.arguments().at(0).toUInt();
        sendReply(connection, message, QVariant::fromValue(getExtents(interface, child, coordType)));
    } else if (function == "GetLayer") {
        sendReply(connection, message, QVariant::fromValue((uint)1));
    } else if (function == "GetMDIZOrder") {
        sendReply(connection, message, QVariant::fromValue((short)0));
    } else if (function == "GetPosition") {
        uint coordType = message.arguments().at(0).toUInt();
        QRect rect;
        if (coordType == ATSPI_COORD_TYPE_SCREEN)
            rect = interface->rect(child);
        else
            rect = getRelativeRect(interface, child);
        QVariantList pos;
        pos << rect.x() << rect.y();
        connection.send(message.createReply(pos));
    } else if (function == "GetSize") {
        QRect rect = interface->rect(child);
        QVariantList size;
        size << rect.width() << rect.height();
        connection.send(message.createReply(size));
    } else if (function == "GrabFocus") {
        if (interface->object() && interface->object()->isWidgetType()) {
            QWidget* w = static_cast<QWidget*>(interface->object());
            w->setFocus(Qt::OtherFocusReason);
            sendReply(connection, message, true);
        }
        sendReply(connection, message, false);
    } else if (function == "SetExtents") {
//        int x = message.arguments().at(0).toInt();
//        int y = message.arguments().at(1).toInt();
//        int width = message.arguments().at(2).toInt();
//        int height = message.arguments().at(3).toInt();
//        uint coordinateType = message.arguments().at(4).toUInt();
        qWarning() << "SetExtents is not implemented.";
        sendReply(connection, message, false);
    } else if (function == "SetPosition") {
//        int x = message.arguments().at(0).toInt();
//        int y = message.arguments().at(1).toInt();
//        uint coordinateType = message.arguments().at(2).toUInt();
        qWarning() << "SetPosition is not implemented.";
        sendReply(connection, message, false);
    } else if (function == "SetSize") {
//        int width = message.arguments().at(0).toInt();
//        int height = message.arguments().at(1).toInt();
        qWarning() << "SetSize is not implemented.";
        sendReply(connection, message, false);
    } else {
        qWarning() << "WARNING: AtSpiAdaptor::handleMessage does not implement " << function << message.path();
        return false;
    }
    return true;
}

QSpiRect AtSpiAdaptor::getExtents(QAccessibleInterface *interface, int child, uint coordType)
{
    QSpiRect val;
    QRect rect;
    if (coordType == 0)
        rect = interface->rect(child);
    else
        rect = getRelativeRect(interface, child);

    val.x = rect.x();
    val.y = rect.y();
    val.width = rect.width();
    val.height = rect.height();
    return val;
}

// Action interface
bool AtSpiAdaptor::actionInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection)
{
    QAccessibleActionInterface *actionIface = interface->actionInterface();
    bool deleteActionInterface = false;
    if (!actionIface) {
        actionIface = new StandardActionWrapper(interface, child);
        deleteActionInterface = true;
        child = 0;
    }

    if (function == "GetNActions") {
        sendReply(connection, message, QVariant::fromValue(QDBusVariant(QVariant::fromValue(actionIface->actionCount()))));
    } else if (function == "DoAction") {
        int index = message.arguments().at(0).toInt();
        actionIface->doAction(index);
        sendReply(connection, message, true);
    } else if (function == "GetActions") {
        if (child) {
            qWarning() << "AtSpiAdaptor::actionInterface: Requesting action interface for child";
            return false;
        }
        sendReply(connection, message, QVariant::fromValue(getActions(actionIface)));
    } else if (function == "GetName") {
        int index = message.arguments().at(0).toInt();
        sendReply(connection, message, actionIface->name(index));
    } else if (function == "GetDescription") {
        int index = message.arguments().at(0).toInt();
        sendReply(connection, message, actionIface->description(index));
    } else if (function == "GetKeyBinding") {
        int index = message.arguments().at(0).toInt();
        QStringList keyBindings;
        keyBindings = actionIface->keyBindings(index);
        /* Might as well return the first key binding, what are the other options? */
        if (keyBindings.length() > 0)
            sendReply(connection, message, keyBindings.at(0));
        else
            sendReply(connection, message, QString());
    } else {
        qWarning() << "WARNING: AtSpiAdaptor::handleMessage does not implement " << function << message.path();
        if (deleteActionInterface)
            delete actionIface;

        return false;
    }

    if (deleteActionInterface)
        delete actionIface;

    return true;
}

QSpiActionArray AtSpiAdaptor::getActions(QAccessibleActionInterface *actionInterface) const
{
    QSpiActionArray actions;
    for (int i = 0; i < actionInterface->actionCount(); i++)
    {
        QSpiAction action;
        QStringList keyBindings;

        action.name = actionInterface->name(i);
        action.description = actionInterface->description(i);

        keyBindings = actionInterface->keyBindings(i);

        if (keyBindings.length() > 0)
                action.keyBinding = keyBindings[0];
        else
                action.keyBinding = "";

        actions << action;
    }
    return actions;
}

// Text interface
bool AtSpiAdaptor::textInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection)
{
    Q_ASSERT(child == 0); // We should never claim to have a text interface on a virtual child
    if (!interface->textInterface()) {
        qWarning() << "WARNING Qt AtSpiAdaptor: Could not find text interface for: " << message.path() << interface;
        return false;
    }

    // properties
    if (function == "GetCaretOffset") {
        sendReply(connection, message, QVariant::fromValue(QDBusVariant(QVariant::fromValue(interface->textInterface()->cursorPosition()))));
    } else if (function == "GetCharacterCount") {
        sendReply(connection, message, QVariant::fromValue(QDBusVariant(QVariant::fromValue(interface->textInterface()->characterCount()))));

    // functions
    } else if (function == "AddSelection") {
        int startOffset = message.arguments().at(0).toInt();
        int endOffset = message.arguments().at(1).toInt();
        int lastSelection = interface->textInterface()->selectionCount();
        interface->textInterface()->setSelection(lastSelection, startOffset, endOffset);
        sendReply(connection, message, (interface->textInterface()->selectionCount() > lastSelection));
    } else if (function == "GetAttributeRun") {
        int offset = message.arguments().at(0).toInt();
        bool includeDefaults = message.arguments().at(1).toBool();
        Q_UNUSED(includeDefaults)
        connection.send(message.createReply(getAttributes(interface, offset, includeDefaults)));
    } else if (function == "GetAttributeValue") {
        int offset = message.arguments().at(0).toInt();
        QString attributeName = message.arguments().at(1).toString();
        connection.send(message.createReply(getAttributeValue(interface, offset, attributeName)));
    } else if (function == "GetAttributes") {
        int offset = message.arguments().at(0).toInt();
        connection.send(message.createReply(getAttributes(interface, offset, true)));
    } else if (function == "GetBoundedRanges") {
        int x = message.arguments().at(0).toInt();
        int y = message.arguments().at(1).toInt();
        int width = message.arguments().at(2).toInt();
        int height = message.arguments().at(3).toInt();
        uint coordType = message.arguments().at(4).toUInt();
        uint xClipType = message.arguments().at(5).toUInt();
        uint yClipType = message.arguments().at(6).toUInt();
        Q_UNUSED(x) Q_UNUSED (y) Q_UNUSED(width)
        Q_UNUSED(height) Q_UNUSED(coordType)
        Q_UNUSED(xClipType) Q_UNUSED(yClipType)
        qWarning("Not implemented: QSpiAdaptor::GetBoundedRanges");
        sendReply(connection, message, QVariant::fromValue(QSpiTextRangeList()));
    } else if (function == "GetCharacterAtOffset") {
        int offset = message.arguments().at(0).toInt();
        int start;
        int end;
        QString result = interface->textInterface()->textAtOffset(offset, QAccessible2::CharBoundary, &start, &end);
        sendReply(connection, message, (int) *(qPrintable (result)));
        Q_ASSERT(0); // FIXME wtf is this!!!???!!! at least test if it produces the desired result bah

    } else if (function == "GetCharacterExtents") {
        int offset = message.arguments().at(0).toInt();
        int coordType = message.arguments().at(1).toUInt();
        connection.send(message.createReply(getCharacterExtents(interface, offset, coordType)));
    } else if (function == "GetDefaultAttributeSet" || function == "GetDefaultAttributes") {
        // GetDefaultAttributes is deprecated in favour of GetDefaultAttributeSet.
        // Empty set seems reasonable. There is no default attribute set.
        sendReply(connection, message, QVariant::fromValue(QSpiAttributeSet()));
    } else if (function == "GetNSelections") {
        sendReply(connection, message, interface->textInterface()->selectionCount());
    } else if (function == "GetOffsetAtPoint") {
        int x = message.arguments().at(0).toInt();
        int y = message.arguments().at(1).toInt();
        uint coordType = message.arguments().at(2).toUInt();
        //            int (int x, int y, uint coordType);
        // FIXME: relative to screen and relative to parent is not the same
        int offset = interface->textInterface()->offsetAtPoint(QPoint (x, y), static_cast <QAccessible2::CoordinateType> (coordType));
        sendReply(connection, message, offset);
    } else if (function == "GetRangeExtents") {
        int startOffset = message.arguments().at(0).toInt();
        int endOffset = message.arguments().at(1).toInt();
        uint coordType = message.arguments().at(2).toUInt();
        connection.send(message.createReply(getRangeExtents(interface, startOffset, endOffset, coordType)));
    } else if (function == "GetSelection") {
        int selectionNum = message.arguments().at(0).toInt();
        int start, end;
        interface->textInterface()->selection(selectionNum, &start, &end);
            if (start < 0) {
            // FIXME: what is this for?
            end = interface->textInterface()->cursorPosition();
        }
        QVariantList sel;
        sel << start << end;
        connection.send(message.createReply(sel));
    } else if (function == "GetText") {
        int startOffset = message.arguments().at(0).toInt();
        int endOffset = message.arguments().at(1).toInt();
        if (endOffset == -1) // AT-SPI uses -1 to signal all characters
            endOffset = interface->textInterface()->characterCount();
        sendReply(connection, message, interface->textInterface()->text(startOffset, endOffset));
    } else if (function == "GetTextAfterOffset") {
        int offset = message.arguments().at(0).toInt();
        int type = message.arguments().at(1).toUInt();
        int startOffset, endOffset;
        QString text = interface->textInterface()->textAfterOffset(offset, qAccessibleBoundaryType(type), &startOffset, &endOffset);
        QVariantList ret;
        ret << text << startOffset << endOffset;
        connection.send(message.createReply(ret));
    } else if (function == "GetTextAtOffset") {
        int offset = message.arguments().at(0).toInt();
        int type = message.arguments().at(1).toUInt();
        int startOffset, endOffset;
        QString text = interface->textInterface()->textAtOffset(offset, qAccessibleBoundaryType(type), &startOffset, &endOffset);
        QVariantList ret;
        ret << text << startOffset << endOffset;
        connection.send(message.createReply(ret));
    } else if (function == "GetTextBeforeOffset") {
        int offset = message.arguments().at(0).toInt();
        int type = message.arguments().at(1).toUInt();
        int startOffset, endOffset;
        QString text = interface->textInterface()->textBeforeOffset(offset, qAccessibleBoundaryType(type), &startOffset, &endOffset);
        QVariantList ret;
        ret << text << startOffset << endOffset;
        connection.send(message.createReply(ret));
    } else if (function == "RemoveSelection") {
        int selectionNum = message.arguments().at(0).toInt();
        interface->textInterface()->removeSelection(selectionNum);
        sendReply(connection, message, true);
    } else if (function == "SetCaretOffset") {
        int offset = message.arguments().at(0).toInt();
        interface->textInterface()->setCursorPosition(offset);
        sendReply(connection, message, true);
    } else if (function == "SetSelection") {
        int selectionNum = message.arguments().at(0).toInt();
        int startOffset = message.arguments().at(1).toInt();
        int endOffset = message.arguments().at(2).toInt();
        interface->textInterface()->setSelection(selectionNum, startOffset, endOffset);
        sendReply(connection, message, true);
    } else {
        qWarning() << "WARNING: AtSpiAdaptor::handleMessage does not implement " << function << message.path();
        return false;
    }
    return true;
}

QAccessible2::BoundaryType AtSpiAdaptor::qAccessibleBoundaryType(int atspiTextBoundaryType) const
{
    switch (atspiTextBoundaryType) {
    case ATSPI_TEXT_BOUNDARY_CHAR:
        return QAccessible2::CharBoundary;
    case ATSPI_TEXT_BOUNDARY_WORD_START:
    case ATSPI_TEXT_BOUNDARY_WORD_END:
        return QAccessible2::WordBoundary;
    case ATSPI_TEXT_BOUNDARY_SENTENCE_START:
    case ATSPI_TEXT_BOUNDARY_SENTENCE_END:
        return QAccessible2::SentenceBoundary;
    case ATSPI_TEXT_BOUNDARY_LINE_START:
    case ATSPI_TEXT_BOUNDARY_LINE_END:
        return QAccessible2::LineBoundary;
    }
    Q_ASSERT_X(0, "", "Requested invalid boundary type.");
    return QAccessible2::CharBoundary;
}

// FIXME all attribute methods below should share code
QVariantList AtSpiAdaptor::getAttributes(QAccessibleInterface *interface, int offset, bool includeDefaults) const
{
    Q_UNUSED(includeDefaults);

    QSpiAttributeSet set;
    int startOffset;
    int endOffset;

    QString joined = interface->textInterface()->attributes(offset, &startOffset, &endOffset);
    QStringList attributes = joined.split (';', QString::SkipEmptyParts, Qt::CaseSensitive);
    foreach (const QString &attr, attributes) {
        QStringList items;
        items = attr.split(':', QString::SkipEmptyParts, Qt::CaseSensitive);
        set[items[0]] = items[1];
    }

    QVariantList list;
    list << QVariant::fromValue(set) << startOffset << endOffset;

    return list;
}

QVariantList AtSpiAdaptor::getAttributeValue(QAccessibleInterface *interface, int offset, const QString &attributeName) const
{
    QString     mapped;
    QString     joined;
    QStringList attributes;
    QSpiAttributeSet map;
    int startOffset;
    int endOffset;
    bool defined;

    joined = interface->textInterface()->attributes(offset, &startOffset, &endOffset);
    attributes = joined.split (';', QString::SkipEmptyParts, Qt::CaseSensitive);
    foreach (const QString& attr, attributes) {
        QStringList items;
        items = attr.split(':', QString::SkipEmptyParts, Qt::CaseSensitive);
        map[items[0]] = items[1];
    }
    mapped = map[attributeName];
    if (mapped == QString())
       defined = true;
    else
       defined = false;
    QVariantList list;
    list << mapped << startOffset << endOffset << defined;
    return list;
}

QVariantList AtSpiAdaptor::getCharacterExtents(QAccessibleInterface *interface, int offset, uint coordType) const
{
    int x;
    int y;
    int width;
    int height;

    // QAccessible2 has RelativeToParent as a coordinate type instead of relative
    // to top-level window, which is an AT-SPI coordinate type.
    if (static_cast<QAccessible2::CoordinateType>(coordType) != QAccessible2::RelativeToScreen) {
        const QWidget *widget = qobject_cast<const QWidget*>(interface->object());
        if (!widget) {
            return QVariantList() << 0 << 0 << 0 << 0;
        }
        const QWidget *parent = widget->parentWidget();
        while (parent) {
            widget = parent;
            parent = widget->parentWidget();
        }
        x = -widget->x();
        y = -widget->y();
    } else {
        x = 0;
        y = 0;
    }

    QRect rect = interface->textInterface()->characterRect(offset, QAccessible2::RelativeToScreen);
    width = rect.width();
    height = rect.height();
    x += rect.x();
    y += rect.y();

    QVariantList list;
    list << x << y << width << height;
    return list;
}

QVariantList AtSpiAdaptor::getRangeExtents(QAccessibleInterface *interface,
                                            int startOffset, int endOffset, uint coordType) const
{
    int x;
    int y;
    int width;
    int height;

    if (endOffset == -1)
        endOffset = interface->textInterface()->characterCount();

    if (endOffset <= startOffset) {
        return QVariantList() << 0 << 0 << 0 << 0;
    }

    int xOffset = 0, yOffset = 0;
    QAccessibleTextInterface *textInterface = interface->textInterface();

    // QAccessible2 has RelativeToParent as a coordinate type instead of relative
    // to top-level window, which is an AT-SPI coordinate type.
    if (static_cast<QAccessible2::CoordinateType>(coordType) != QAccessible2::RelativeToScreen) {
        const QWidget *widget = qobject_cast<const QWidget*>(interface->object());
        if (!widget) {
            return QVariantList() << 0 << 0 << 0 << 0;
        }
        const QWidget *parent = widget->parentWidget();
        while (parent) {
            widget = parent;
            parent = widget->parentWidget();
        }
        xOffset = -widget->x();
        yOffset = -widget->y();
    }

    int minX=INT_MAX, minY=INT_MAX, maxX=0, maxY=0;

    for (int i=startOffset; i<endOffset; i++) {
        QRect rect = textInterface->characterRect(i, QAccessible2::RelativeToScreen);
        if (rect.x() < minX) {
            minX = rect.x();
        }
        if (rect.y() < minY) {
            minY = rect.y();
        }
        if ((rect.x() + rect.width()) > maxX) {
            maxX = (rect.x() + rect.width());
        }
        if ((rect.y() + rect.height()) > maxY) {
            maxY = (rect.y() + rect.height());
        }
    }

    width = maxX - minX;
    height = maxY - minY;
    y = minY + yOffset;
    x = minX + xOffset;
    QVariantList list;
    list << x << y << width << height;
    return list;
}



// Editable Text interface
bool AtSpiAdaptor::editableTextInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection)
{
    Q_ASSERT(child == 0); // We should never claim to have a text interface on a virtual child
    if (!interface->editableTextInterface()) {
        qWarning() << "WARNING Qt AtSpiAdaptor: Could not find editable text interface for: " << message.path() << interface;
        return false;
    }

    if (function == "CopyText") {
        int startPos = message.arguments().at(0).toInt();
        int endPos = message.arguments().at(1).toInt();
        interface->editableTextInterface()->copyText(startPos, endPos);
        connection.send(message.createReply(true));
    } else if (function == "CutText") {
        int startPos = message.arguments().at(0).toInt();
        int endPos = message.arguments().at(1).toInt();
        interface->editableTextInterface()->cutText(startPos, endPos);
        connection.send(message.createReply(true));
    } else if (function == "DeleteText") {
        int startPos = message.arguments().at(0).toInt();
        int endPos = message.arguments().at(1).toInt();
        interface->editableTextInterface()->deleteText(startPos, endPos);
        connection.send(message.createReply(true));
    } else if (function == "InsertText") {
        int position = message.arguments().at(0).toInt();
        QString text = message.arguments().at(1).toString();
        int length = message.arguments().at(2).toInt();
        QString resized (text);
        resized.resize(length);
        interface->editableTextInterface()->insertText(position, resized);
        connection.send(message.createReply(true));
    } else if (function == "PasteText") {
        int position = message.arguments().at(0).toInt();
        interface->editableTextInterface()->pasteText(position);
        connection.send(message.createReply(true));
    } else if (function == "SetTextContents") {
        QString newContents = message.arguments().at(0).toString();
        interface->editableTextInterface()->replaceText(0, interface->textInterface()->characterCount(), newContents);
        connection.send(message.createReply(true));
    } else if (function == "") {
        connection.send(message.createReply());
    } else {
        qWarning() << "WARNING: AtSpiAdaptor::handleMessage does not implement " << function << message.path();
        return false;
    }
    return true;
}

// Value interface
bool AtSpiAdaptor::valueInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection)
{
    Q_ASSERT(child == 0);
    if (!interface->valueInterface()) {
        qWarning() << "WARNING Qt AtSpiAdaptor: Could not find value interface for: " << message.path() << interface;
        return false;
    }

    if (0) {
    } else if (function == "SetCurrentValue") {
        QDBusVariant v = message.arguments().at(2).value<QDBusVariant>();
        double value = v.variant().toDouble();
        //Temporal fix
        //See https://bugzilla.gnome.org/show_bug.cgi?id=652596
        interface->valueInterface()->setCurrentValue(value);
        connection.send(message.createReply()); // FIXME is the reply needed?
    } else if (function == "GetCurrentValue") {
        bool success;
        double val = interface->valueInterface()->currentValue().toDouble(&success);
        if (!success) {
            qWarning ("AtSpiAdaptor::valueInterface: Could not convert current value to double.");
        }
        connection.send(message.createReply(
                            QVariant::fromValue(QDBusVariant(QVariant::fromValue(val)))));
    } else if (function == "GetMaximumValue") {
        bool success;
        double val = interface->valueInterface()->maximumValue().toDouble(&success);
        if (!success) {
            qWarning ("AtSpiAdaptor::valueInterface: Could not convert current value to double.");
        }
        connection.send(message.createReply(
                            QVariant::fromValue(QDBusVariant(QVariant::fromValue(val)))));
    } else if (function == "GetMinimumIncrement") {
        connection.send(message.createReply(
                            QVariant::fromValue(QDBusVariant(QVariant::fromValue(0.0)))));
    } else if (function == "GetMinimumValue") {
        bool success;
        double val = interface->valueInterface()->minimumValue().toDouble(&success);
        if (!success) {
            qWarning ("AtSpiAdaptor::valueInterface: Could not convert current value to double.");
        }
        connection.send(message.createReply(
                            QVariant::fromValue(QDBusVariant(QVariant::fromValue(val)))));
    } else {
        qWarning() << "WARNING: AtSpiAdaptor::handleMessage does not implement " << function << message.path();
        return false;
    }
    return true;
}

// Table interface
bool AtSpiAdaptor::tableInterface(QAccessibleInterface *interface, int child, const QString &function, const QDBusMessage &message, const QDBusConnection &connection)
{
    Q_ASSERT(child == 0);
    if (!interface->table2Interface()) {
        qWarning() << "WARNING Qt AtSpiAdaptor: Could not find table interface for: " << message.path() << interface;
        return false;
    }

    if (0) {
    // properties
    } else if (function == "GetCaption") {
        // fixme: leak of QAI
        QAccessibleInterface *caption = interface->table2Interface()->caption();
        if (!caption)
            caption = interface->tableInterface() ? interface->tableInterface()->caption() : 0;

        QObject *object = caption ? caption->object() : 0;
        if (!object) {
            connection.send(message.createReply(QVariant::fromValue(QDBusVariant(QVariant::fromValue(QSpiObjectReference())))));
        } else {
            QDBusObjectPath path(pathForObject(object));
            QSpiObjectReference ref(connection, path);
            connection.send(message.createReply(QVariant::fromValue(QDBusVariant(QVariant::fromValue(ref)))));
        }
    } else if (function == "GetNColumns") {
        connection.send(message.createReply(QVariant::fromValue(QDBusVariant(
            QVariant::fromValue(interface->table2Interface()->columnCount())))));
    } else if (function == "GetNRows") {
        connection.send(message.createReply(QVariant::fromValue(QDBusVariant(
            QVariant::fromValue(interface->table2Interface()->rowCount())))));
    } else if (function == "GetNSelectedColumns") {
        connection.send(message.createReply(QVariant::fromValue(QDBusVariant(
            QVariant::fromValue(interface->table2Interface()->selectedColumnCount())))));
    } else if (function == "GetNSelectedRows") {
        connection.send(message.createReply(QVariant::fromValue(QDBusVariant(
            QVariant::fromValue(interface->table2Interface()->selectedRowCount())))));
    } else if (function == "GetSummary") {
        // fixme: leak of QAI
        QAccessibleInterface *summary = interface->table2Interface()->summary();
        if (!summary)
            summary = interface->tableInterface() ? interface->tableInterface()->summary() : 0;

        QObject *object = summary ? summary->object() : 0;
        if (!object) {
            QDBusMessage reply = message.createReply(QVariant::fromValue(QDBusVariant(QVariant::fromValue(QSpiObjectReference()))));
            qDebug() << "signature: " << reply.signature();
            connection.send(reply);
        } else {
            QDBusObjectPath path(pathForObject(object));
            QSpiObjectReference ref(connection, path);
            connection.send(message.createReply(QVariant::fromValue(QDBusVariant(QVariant::fromValue(ref)))));
        }

    // methods
    } else if (function == "GetAccessibleAt") {
        int row = message.arguments().at(0).toInt();
        int column = message.arguments().at(1).toInt();
        Q_ASSERT(interface->table2Interface());
        Q_ASSERT(row >= 0);
        Q_ASSERT(column >= 0);
        Q_ASSERT(row < interface->table2Interface()->rowCount());
        Q_ASSERT(column < interface->table2Interface()->columnCount());

        QSpiObjectReference ref;
        QAccessibleInterface* cell = interface->table2Interface()->cellAt(row, column);
        if (cell) {
            ref = QSpiObjectReference(connection, QDBusObjectPath(pathForInterface(cell, 0)));
            delete cell;
        } else {
            qWarning() << "WARNING: no cell interface returned for " << interface->object() << row << column;
            ref = QSpiObjectReference();
        }
        connection.send(message.createReply(QVariant::fromValue(ref)));

    } else if (function == "GetIndexAt") {
        int row = message.arguments().at(0).toInt();
        int column = message.arguments().at(1).toInt();
        QAccessibleInterface *cell = interface->table2Interface()->cellAt(row, column);
        int index = interface->indexOfChild(cell);
        qDebug() << "QSpiAdaptor::GetIndexAt" << row << column << index;
        Q_ASSERT(index > 0);
        delete cell;
        connection.send(message.createReply(index));

    } else if (function == "GetColumnAtIndex") {
        int index = message.arguments().at(0).toInt();
        int ret = 0;
        if (index > 1) {
            QAccessibleInterface *iface;
            interface->navigate(QAccessible::Child, index, &iface);
            if (iface) {
                qDebug() << "iface: " << iface->text(QAccessible::Name, 0);

                QAccessibleTable2CellInterface *cell = static_cast<QAccessibleTable2CellInterface*>(iface);
                ret = cell->columnIndex();
                delete cell;
            }
        }
        connection.send(message.createReply(ret));
    } else if (function == "GetRowAtIndex") {
        // FIXME merge with GetColumnAtIndex
        int index = message.arguments().at(0).toInt();
        int ret = 0;
        qDebug() << "QSpiAdaptor::GetRowAtIndex" << index;
        if (index > 1) {
            QAccessibleInterface *iface;
            interface->navigate(QAccessible::Child, index, &iface);
            if (iface) {
                qDebug() << "iface: " << iface->text(QAccessible::Name, 0);

                QAccessibleTable2CellInterface *cell = static_cast<QAccessibleTable2CellInterface*>(iface);
                ret = cell->rowIndex();
                delete cell;
            }
        }
        connection.send(message.createReply(ret));

    } else if (function == "GetColumnDescription") {
        int column = message.arguments().at(0).toInt();
        connection.send(message.createReply(interface->table2Interface()->columnDescription(column)));
    } else if (function == "GetRowDescription") {
        int row = message.arguments().at(0).toInt();
        connection.send(message.createReply(interface->table2Interface()->rowDescription(row)));



    } else if (function == "GetRowColumnExtentsAtIndex") {
        int index = message.arguments().at(0).toInt();
        bool success = false;

        int row, col, rowExtents, colExtents;
        bool isSelected;

        int cols = interface->table2Interface()->columnCount();
        row = index/cols;
        col = index%cols;
        QAccessibleTable2CellInterface *cell = interface->table2Interface()->cellAt(row, col);
        if (cell) {
            cell->rowColumnExtents(&row, &col, &rowExtents, &colExtents, &isSelected);
            success = true;
            delete cell;
        }

        QVariantList list;
        list << success << row << col << rowExtents << colExtents << isSelected;
        connection.send(message.createReply(list));

    } else if (function == "GetColumnExtentAt") {
        int row = message.arguments().at(0).toInt();
        int column = message.arguments().at(1).toInt();
        connection.send(message.createReply(interface->table2Interface()->cellAt(row, column)->columnExtent()));

    } else if (function == "GetRowExtentAt") {
        int row = message.arguments().at(0).toInt();
        int column = message.arguments().at(1).toInt();
        connection.send(message.createReply(interface->table2Interface()->cellAt(row, column)->rowExtent()));

    } else if (function == "GetColumnHeader") {
        int column = message.arguments().at(0).toInt();
        QSpiObjectReference ref;

        QAccessibleTable2CellInterface *cell = interface->table2Interface()->cellAt(0, column);
        if (cell) {
            QList<QAccessibleInterface*> header = cell->columnHeaderCells();
            delete cell;
            if (header.size() > 0) {
                ref = QSpiObjectReference(connection, QDBusObjectPath(pathForInterface(header.at(0), 0)));
                qDeleteAll(header);
            }
        }
        connection.send(message.createReply(QVariant::fromValue(ref)));

    } else if (function == "GetRowHeader") {
        int row = message.arguments().at(0).toInt();
        QSpiObjectReference ref;
        QAccessibleTable2CellInterface *cell = interface->table2Interface()->cellAt(row, 0);
        if (cell) {
            QList<QAccessibleInterface*> header = cell->rowHeaderCells();
            delete cell;
            if (header.size() > 0) {
                ref = QSpiObjectReference(connection, QDBusObjectPath(pathForInterface(header.at(0), 0)));
                qDeleteAll(header);
            }
        }
        connection.send(message.createReply(QVariant::fromValue(ref)));

    } else if (function == "GetSelectedColumns") {
        connection.send(message.createReply(QVariant::fromValue(interface->table2Interface()->selectedColumns())));
    } else if (function == "GetSelectedRows") {
        connection.send(message.createReply(QVariant::fromValue(interface->table2Interface()->selectedRows())));
    } else if (function == "IsColumnSelected") {
        int column = message.arguments().at(0).toInt();
        connection.send(message.createReply(interface->table2Interface()->isColumnSelected(column)));
    } else if (function == "IsRowSelected") {
        int row = message.arguments().at(0).toInt();
        connection.send(message.createReply(interface->table2Interface()->isRowSelected(row)));
    } else if (function == "IsSelected") {
        int row = message.arguments().at(0).toInt();
        int column = message.arguments().at(1).toInt();
        QAccessibleTable2CellInterface* cell = interface->table2Interface()->cellAt(row, column);
        connection.send(message.createReply(cell->isSelected()));
        delete cell;
    } else if (function == "AddColumnSelection") {
        int column = message.arguments().at(0).toInt();
        connection.send(message.createReply(interface->table2Interface()->selectColumn(column)));
    } else if (function == "AddRowSelection") {
        int row = message.arguments().at(0).toInt();
        connection.send(message.createReply(interface->table2Interface()->selectRow(row)));
    } else if (function == "RemoveColumnSelection") {
        int column = message.arguments().at(0).toInt();
        connection.send(message.createReply(interface->table2Interface()->unselectColumn(column)));
    } else if (function == "RemoveRowSelection") {
        int row = message.arguments().at(0).toInt();
        connection.send(message.createReply(interface->table2Interface()->unselectRow(row)));
    } else {
        qWarning() << "WARNING: AtSpiAdaptor::handleMessage does not implement " << function << message.path();
        return false;
    }
    return true;
}
