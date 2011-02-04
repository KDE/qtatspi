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

#include "constant_mappings.h"

#define BITARRAY_SEQ_TERM 0xffffffff

#define BITARRAY_SET(p, n)   ( (p)[n>>5] |=  (1<<(n&31)) )
#define BITARRAY_UNSET(p, n) ( (p)[n>>5] &= ~(1<<(n&31)) )

QHash <QAccessible::Role, QPair<QSpiRole, QString> > qSpiRoleMapping;
typedef QPair<QSpiRole, QString> role;

static void initialize_role_mapping ()
{
       qSpiRoleMapping.insert (QAccessible::NoRole, role(ROLE_INVALID, QObject::tr("no role")));
       qSpiRoleMapping.insert (QAccessible::TitleBar, role(ROLE_TEXT, QObject::tr("title bar")));
       qSpiRoleMapping.insert (QAccessible::MenuBar, role(ROLE_MENU_BAR, QObject::tr("menu bar")));
       qSpiRoleMapping.insert (QAccessible::ScrollBar, role(ROLE_SCROLL_BAR, QObject::tr("scroll bar")));
       qSpiRoleMapping.insert (QAccessible::Grip, role(ROLE_UNKNOWN, QObject::tr("grip")));
       qSpiRoleMapping.insert (QAccessible::Sound, role(ROLE_UNKNOWN, QObject::tr("sound")));
       qSpiRoleMapping.insert (QAccessible::Cursor, role(ROLE_ARROW, QObject::tr("cursor")));
       qSpiRoleMapping.insert (QAccessible::Caret, role(ROLE_UNKNOWN, QObject::tr("caret")));
       qSpiRoleMapping.insert (QAccessible::AlertMessage, role(ROLE_ALERT, QObject::tr("alert message")));
       qSpiRoleMapping.insert (QAccessible::Window, role(ROLE_WINDOW, QObject::tr("window")));
       qSpiRoleMapping.insert (QAccessible::Client, role(ROLE_FILLER, QObject::tr("client")));
       qSpiRoleMapping.insert (QAccessible::PopupMenu, role(ROLE_POPUP_MENU, QObject::tr("popup menu")));
       qSpiRoleMapping.insert (QAccessible::MenuItem, role(ROLE_MENU_ITEM, QObject::tr("menu item")));
       qSpiRoleMapping.insert (QAccessible::ToolTip, role(ROLE_TOOL_TIP, QObject::tr("tool tip")));
       qSpiRoleMapping.insert (QAccessible::Application, role(ROLE_APPLICATION, QObject::tr("application")));
       qSpiRoleMapping.insert (QAccessible::Document, role(ROLE_DOCUMENT_FRAME, QObject::tr("document")));
       qSpiRoleMapping.insert (QAccessible::Pane, role(ROLE_PANEL, QObject::tr("pane")));
       qSpiRoleMapping.insert (QAccessible::Chart, role(ROLE_CHART, QObject::tr("chart")));
       qSpiRoleMapping.insert (QAccessible::Dialog, role(ROLE_DIALOG, QObject::tr("dialog")));
       qSpiRoleMapping.insert (QAccessible::Border, role(ROLE_FRAME, QObject::tr("border")));
       qSpiRoleMapping.insert (QAccessible::Grouping, role(ROLE_PANEL, QObject::tr("grouping")));
       qSpiRoleMapping.insert (QAccessible::Separator, role(ROLE_SEPARATOR, QObject::tr("separator")));
       qSpiRoleMapping.insert (QAccessible::ToolBar, role(ROLE_TOOL_BAR, QObject::tr("tool bar")));
       qSpiRoleMapping.insert (QAccessible::StatusBar, role(ROLE_STATUS_BAR, QObject::tr("status bar")));
       qSpiRoleMapping.insert (QAccessible::Table, role(ROLE_TABLE, QObject::tr("table")));
       qSpiRoleMapping.insert (QAccessible::ColumnHeader, role(ROLE_TABLE_COLUMN_HEADER, QObject::tr("column header")));
       qSpiRoleMapping.insert (QAccessible::RowHeader, role(ROLE_TABLE_ROW_HEADER, QObject::tr("row header")));
       qSpiRoleMapping.insert (QAccessible::Column, role(ROLE_TABLE_CELL, QObject::tr("column")));
       qSpiRoleMapping.insert (QAccessible::Row, role(ROLE_TABLE_CELL, QObject::tr("row")));
       qSpiRoleMapping.insert (QAccessible::Cell, role(ROLE_TABLE_CELL, QObject::tr("cell")));
       qSpiRoleMapping.insert (QAccessible::Link, role(ROLE_LINK, QObject::tr("link")));
       qSpiRoleMapping.insert (QAccessible::HelpBalloon, role(ROLE_DIALOG, QObject::tr("help balloon")));
       qSpiRoleMapping.insert (QAccessible::Assistant, role(ROLE_DIALOG, QObject::tr("assistant")));
       qSpiRoleMapping.insert (QAccessible::List, role(ROLE_LIST, QObject::tr("list")));
       qSpiRoleMapping.insert (QAccessible::ListItem, role(ROLE_LIST_ITEM, QObject::tr("list item")));
       qSpiRoleMapping.insert (QAccessible::Tree, role(ROLE_TREE, QObject::tr("tree")));
       qSpiRoleMapping.insert (QAccessible::TreeItem, role(ROLE_TABLE_CELL, QObject::tr("tree item")));
       qSpiRoleMapping.insert (QAccessible::PageTab, role(ROLE_PAGE_TAB, QObject::tr("page tab")));
       qSpiRoleMapping.insert (QAccessible::PropertyPage, role(ROLE_PAGE_TAB, QObject::tr("property page")));
       qSpiRoleMapping.insert (QAccessible::Indicator, role(ROLE_UNKNOWN, QObject::tr("indicator")));
       qSpiRoleMapping.insert (QAccessible::Graphic, role(ROLE_IMAGE, QObject::tr("graphic")));
       qSpiRoleMapping.insert (QAccessible::StaticText, role(ROLE_LABEL, QObject::tr("static text")));
       qSpiRoleMapping.insert (QAccessible::EditableText, role(ROLE_TEXT, QObject::tr("editable text")));
       qSpiRoleMapping.insert (QAccessible::PushButton, role(ROLE_PUSH_BUTTON, QObject::tr("push button")));
       qSpiRoleMapping.insert (QAccessible::CheckBox, role(ROLE_CHECK_BOX, QObject::tr("check box")));
       qSpiRoleMapping.insert (QAccessible::RadioButton, role(ROLE_RADIO_BUTTON, QObject::tr("radio box")));
       qSpiRoleMapping.insert (QAccessible::ComboBox, role(ROLE_COMBO_BOX, QObject::tr("combo box")));
       qSpiRoleMapping.insert (QAccessible::ProgressBar, role(ROLE_PROGRESS_BAR, QObject::tr("progress bar")));
       qSpiRoleMapping.insert (QAccessible::Dial, role(ROLE_DIAL, QObject::tr("dial")));
       qSpiRoleMapping.insert (QAccessible::HotkeyField, role(ROLE_TEXT, QObject::tr("hotkey field")));
       qSpiRoleMapping.insert (QAccessible::Slider, role(ROLE_SLIDER, QObject::tr("slider")));
       qSpiRoleMapping.insert (QAccessible::SpinBox, role(ROLE_SPIN_BUTTON, QObject::tr("spin box")));
       qSpiRoleMapping.insert (QAccessible::Canvas, role(ROLE_CANVAS, QObject::tr("canvas")));
       qSpiRoleMapping.insert (QAccessible::Animation, role(ROLE_ANIMATION, QObject::tr("animation")));
       qSpiRoleMapping.insert (QAccessible::Equation, role(ROLE_TEXT, QObject::tr("equation")));
       qSpiRoleMapping.insert (QAccessible::ButtonDropDown, role(ROLE_PUSH_BUTTON, QObject::tr("button drop down")));
       qSpiRoleMapping.insert (QAccessible::ButtonMenu, role(ROLE_PUSH_BUTTON, QObject::tr("button menu")));
       qSpiRoleMapping.insert (QAccessible::ButtonDropGrid, role(ROLE_PUSH_BUTTON, QObject::tr("button drop grid")));
       qSpiRoleMapping.insert (QAccessible::Whitespace, role(ROLE_FILLER, QObject::tr("whitespace")));
       qSpiRoleMapping.insert (QAccessible::PageTabList, role(ROLE_PAGE_TAB_LIST, QObject::tr("page tab list")));
       qSpiRoleMapping.insert (QAccessible::Clock, role(ROLE_UNKNOWN, QObject::tr("clock")));
       qSpiRoleMapping.insert (QAccessible::Splitter, role(ROLE_SPLIT_PANE, QObject::tr("splitter")));
       qSpiRoleMapping.insert (QAccessible::LayeredPane, role(ROLE_LAYERED_PANE, QObject::tr("layered pane")));
       qSpiRoleMapping.insert (QAccessible::UserRole, role(ROLE_UNKNOWN, QObject::tr("user role")));
}

/*---------------------------------------------------------------------------*/

void qspi_stateset_from_qstate (QAccessible::State state, QSpiIntList &set)
{
       int array[2] = {0, 0};

       /* We may need to take the role of the object into account when
        * mapping between the state sets
        */
       BITARRAY_SET (array, STATE_EDITABLE);
       BITARRAY_SET (array, STATE_ENABLED);
       BITARRAY_SET (array, STATE_SHOWING);
       BITARRAY_SET (array, STATE_VISIBLE);
       BITARRAY_SET (array, STATE_SENSITIVE);

       for (int mask = 1; mask <= int(QAccessible::HasInvokeExtension); mask <<= 1)
       {
           switch (state & mask)
           {
                 case QAccessible::Unavailable:
                 {
                         BITARRAY_UNSET (array, STATE_ENABLED);
                         BITARRAY_UNSET (array, STATE_SHOWING);
                         BITARRAY_UNSET (array, STATE_VISIBLE);
                         BITARRAY_UNSET (array, STATE_SENSITIVE);
                         break;
                 }
                 case QAccessible::Selected:
                 {
                         BITARRAY_SET (array, STATE_SELECTED);
                         break;
                 }
                 case QAccessible::Focused:
                 {
                         BITARRAY_SET (array, STATE_FOCUSED);
                         break;
                 }
                 case QAccessible::Pressed:
                 {
                         BITARRAY_SET (array, STATE_PRESSED);
                         break;
                 }
                 case QAccessible::Checked:
                 {
                         BITARRAY_SET (array, STATE_CHECKED);
                         break;
                 }
                 case QAccessible::Mixed:
                 {
                         BITARRAY_SET (array, STATE_INDETERMINATE);
                         break;
                 }
                 case QAccessible::ReadOnly:
                 {
                         BITARRAY_UNSET (array, STATE_EDITABLE);
                         break;
                 }
                 case QAccessible::HotTracked:
                 {
                         break;
                 }
                 case QAccessible::DefaultButton:
                 {
                         BITARRAY_SET (array, STATE_IS_DEFAULT);
                         break;
                 }
                 case QAccessible::Expanded:
                 {
                         BITARRAY_SET (array, STATE_EXPANDED);
                         break;
                 }
                 case QAccessible::Collapsed:
                 {
                         BITARRAY_SET (array, STATE_COLLAPSED);
                         break;
                 }
                 case QAccessible::Busy:
                 {
                         BITARRAY_SET (array, STATE_BUSY);
                         break;
                 }
                 case QAccessible::Marqueed:
                 case QAccessible::Animated:
                 {
                         BITARRAY_SET (array, STATE_ANIMATED);
                         break;
                 }
                 case QAccessible::Invisible:
                 case QAccessible::Offscreen:
                 {
                         BITARRAY_UNSET (array, STATE_SHOWING);
                         break;
                 }
                 case QAccessible::Sizeable:
                 {
                         BITARRAY_SET (array, STATE_RESIZABLE);
                         break;
                 }
                 case QAccessible::Movable:
                 case QAccessible::SelfVoicing:
                 {
                         break;
                 }
                 case QAccessible::Focusable:
                 {
                         BITARRAY_SET (array, STATE_FOCUSABLE);
                         break;
                 }
                 case QAccessible::Selectable:
                 {
                         BITARRAY_SET (array, STATE_SELECTABLE);
                         break;
                 }
                 case QAccessible::Linked:
                 {
                         break;
                 }
                 case QAccessible::Traversed:
                 {
                         BITARRAY_SET (array, STATE_VISITED);
                         break;
                 }
                 case QAccessible::MultiSelectable:
                 {
                         BITARRAY_SET (array, STATE_MULTISELECTABLE);
                         break;
                 }
                 case QAccessible::ExtSelectable:
                 {
                         BITARRAY_SET (array, STATE_SELECTABLE);
                         break;
                 }
                 case QAccessible::Protected:
                 case QAccessible::HasPopup:
                 {
                         break;
                 }
                 case QAccessible::Modal:
                 {
                         BITARRAY_SET (array, STATE_MODAL);
                         break;
                 }
                 case QAccessible::HasInvokeExtension:
                 {
                         break;
                 }
                 default:
                 {
                         break;
                 }
           }
       }
       set << array[0];
       set << array[1];
}

/*---------------------------------------------------------------------------*/

void qspi_initialize_constant_mappings ()
{
       initialize_role_mapping ();
}

/*END------------------------------------------------------------------------*/
