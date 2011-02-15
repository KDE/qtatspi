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

#include "object.h"

#define BITARRAY_SEQ_TERM 0xffffffff

#define BITARRAY_SET(p, n)   ( (p)[n>>5] |=  (1<<(n&31)) )
#define BITARRAY_UNSET(p, n) ( (p)[n>>5] &= ~(1<<(n&31)) )



// FIXME the assignment of roles is quite arbitrary, at some point go through this list and sort and check that it makes sense
//  "calendar" "check menu item"  "color chooser" "column header"    "dateeditor"  "desktop icon"  "desktop frame"
//  "directory pane"  "drawing area"  "file chooser" "fontchooser"  "frame"  "glass pane"  "html container"  "icon"
//  "internal frame"  "option pane"  "password text" "radio menu item"  "root pane"  "row header"    "scroll pane"
//  "tear off menu item"  "terminal" "toggle button" "tree table"  "unknown"  "viewport" "header"  "footer"  "paragraph"
//  "ruler"    "autocomplete"  "edit bar" "embedded component"  "entry"    "caption"
//  "heading"  "page"  "section"  "redundant object"  "form"  "input method window"  "menu"

QHash <QAccessible::Role, RoleNames> qSpiRoleMapping;

static void initialize_role_mapping ()
{
    qSpiRoleMapping.insert(QAccessible::NoRole, RoleNames(ROLE_INVALID, "invalid", QSpiObject::tr("invalid role")));
    qSpiRoleMapping.insert(QAccessible::TitleBar, RoleNames(ROLE_TEXT, "text", QSpiObject::tr("title bar")));
    qSpiRoleMapping.insert(QAccessible::MenuBar, RoleNames(ROLE_MENU_BAR, "menu bar", QSpiObject::tr("menu bar")));
    qSpiRoleMapping.insert(QAccessible::ScrollBar, RoleNames(ROLE_SCROLL_BAR, "scroll bar", QSpiObject::tr("scroll bar")));
    qSpiRoleMapping.insert(QAccessible::Grip, RoleNames(ROLE_UNKNOWN, "unknown", QSpiObject::tr("grip")));
    qSpiRoleMapping.insert(QAccessible::Sound, RoleNames(ROLE_UNKNOWN, "unknown", QSpiObject::tr("sound")));
    qSpiRoleMapping.insert(QAccessible::Cursor, RoleNames(ROLE_ARROW, "arrow", QSpiObject::tr("cursor")));
    qSpiRoleMapping.insert(QAccessible::Caret, RoleNames(ROLE_UNKNOWN, "unknown", QSpiObject::tr("caret")));
    qSpiRoleMapping.insert(QAccessible::AlertMessage, RoleNames(ROLE_ALERT, "alert", QSpiObject::tr("alert message")));
    qSpiRoleMapping.insert(QAccessible::Window, RoleNames(ROLE_WINDOW, "window", QSpiObject::tr("window")));
    qSpiRoleMapping.insert(QAccessible::Client, RoleNames(ROLE_FILLER, "filler", QSpiObject::tr("client")));
    qSpiRoleMapping.insert(QAccessible::PopupMenu, RoleNames(ROLE_POPUP_MENU, "popup menu", QSpiObject::tr("popup menu")));
    qSpiRoleMapping.insert(QAccessible::MenuItem, RoleNames(ROLE_MENU_ITEM, "menu item", QSpiObject::tr("menu item")));
    qSpiRoleMapping.insert(QAccessible::ToolTip, RoleNames(ROLE_TOOL_TIP, "tool tip", QSpiObject::tr("tool tip")));
    qSpiRoleMapping.insert(QAccessible::Application, RoleNames(ROLE_APPLICATION, "application", QSpiObject::tr("application")));
    qSpiRoleMapping.insert(QAccessible::Document, RoleNames(ROLE_DOCUMENT_FRAME, "document frame", QSpiObject::tr("document")));
    qSpiRoleMapping.insert(QAccessible::Pane, RoleNames(ROLE_PANEL, "panel", QSpiObject::tr("pane")));
    qSpiRoleMapping.insert(QAccessible::Chart, RoleNames(ROLE_CHART, "chart", QSpiObject::tr("chart")));
    qSpiRoleMapping.insert(QAccessible::Dialog, RoleNames(ROLE_DIALOG, "dialog", QSpiObject::tr("dialog")));
    qSpiRoleMapping.insert(QAccessible::Border, RoleNames(ROLE_FRAME, "frame", QSpiObject::tr("border")));
    qSpiRoleMapping.insert(QAccessible::Grouping, RoleNames(ROLE_PANEL, "panel", QSpiObject::tr("grouping")));
    qSpiRoleMapping.insert(QAccessible::Separator, RoleNames(ROLE_SEPARATOR, "separator", QSpiObject::tr("separator")));
    qSpiRoleMapping.insert(QAccessible::ToolBar, RoleNames(ROLE_TOOL_BAR, "tool bar", QSpiObject::tr("tool bar")));
    qSpiRoleMapping.insert(QAccessible::StatusBar, RoleNames(ROLE_STATUS_BAR, "statusbar", QSpiObject::tr("status bar")));
    qSpiRoleMapping.insert(QAccessible::Table, RoleNames(ROLE_TABLE, "table", QSpiObject::tr("table")));
    qSpiRoleMapping.insert(QAccessible::ColumnHeader, RoleNames(ROLE_TABLE_COLUMN_HEADER, "column header", QSpiObject::tr("column header")));
    qSpiRoleMapping.insert(QAccessible::RowHeader, RoleNames(ROLE_TABLE_ROW_HEADER, "row header", QSpiObject::tr("row header")));
    qSpiRoleMapping.insert(QAccessible::Column, RoleNames(ROLE_TABLE_CELL, "table cell", QSpiObject::tr("column")));
    qSpiRoleMapping.insert(QAccessible::Row, RoleNames(ROLE_TABLE_CELL, "table cell", QSpiObject::tr("row")));
    qSpiRoleMapping.insert(QAccessible::Cell, RoleNames(ROLE_TABLE_CELL, "table cell", QSpiObject::tr("cell")));
    qSpiRoleMapping.insert(QAccessible::Link, RoleNames(ROLE_LINK, "link" , QSpiObject::tr("link")));
    qSpiRoleMapping.insert(QAccessible::HelpBalloon, RoleNames(ROLE_DIALOG, "dialog", QSpiObject::tr("help balloon")));
    qSpiRoleMapping.insert(QAccessible::Assistant, RoleNames(ROLE_DIALOG, "dialog", QSpiObject::tr("assistant")));
    qSpiRoleMapping.insert(QAccessible::List, RoleNames(ROLE_LIST, "list", QSpiObject::tr("list")));
    qSpiRoleMapping.insert(QAccessible::ListItem, RoleNames(ROLE_LIST_ITEM, "list item", QSpiObject::tr("list item")));
    qSpiRoleMapping.insert(QAccessible::Tree, RoleNames(ROLE_TREE, "tree", QSpiObject::tr("tree")));
    qSpiRoleMapping.insert(QAccessible::TreeItem, RoleNames(ROLE_TABLE_CELL, "table cell", QSpiObject::tr("tree item")));
    qSpiRoleMapping.insert(QAccessible::PageTab, RoleNames(ROLE_PAGE_TAB, "page tab", QSpiObject::tr("page tab")));
    qSpiRoleMapping.insert(QAccessible::PropertyPage, RoleNames(ROLE_PAGE_TAB, "page tab", QSpiObject::tr("property page")));
    qSpiRoleMapping.insert(QAccessible::Indicator, RoleNames(ROLE_UNKNOWN, "unknown", QSpiObject::tr("indicator")));
    qSpiRoleMapping.insert(QAccessible::Graphic, RoleNames(ROLE_IMAGE, "image", QSpiObject::tr("graphic")));
    qSpiRoleMapping.insert(QAccessible::StaticText, RoleNames(ROLE_LABEL, "label", QSpiObject::tr("static text")));
    qSpiRoleMapping.insert(QAccessible::EditableText, RoleNames(ROLE_TEXT, "text", QSpiObject::tr("editable text")));
    qSpiRoleMapping.insert(QAccessible::PushButton, RoleNames(ROLE_PUSH_BUTTON, "push button", QSpiObject::tr("push button")));
    qSpiRoleMapping.insert(QAccessible::CheckBox, RoleNames(ROLE_CHECK_BOX, "check box", QSpiObject::tr("check box")));
    qSpiRoleMapping.insert(QAccessible::RadioButton, RoleNames(ROLE_RADIO_BUTTON, "radio button", QSpiObject::tr("radio box")));
    qSpiRoleMapping.insert(QAccessible::ComboBox, RoleNames(ROLE_COMBO_BOX, "combo box", QSpiObject::tr("combo box")));
    qSpiRoleMapping.insert(QAccessible::ProgressBar, RoleNames(ROLE_PROGRESS_BAR, "progress bar", QSpiObject::tr("progress bar")));
    qSpiRoleMapping.insert(QAccessible::Dial, RoleNames(ROLE_DIAL, "accelerator label", QSpiObject::tr("dial")));
    qSpiRoleMapping.insert(QAccessible::HotkeyField, RoleNames(ROLE_TEXT, "text", QSpiObject::tr("hotkey field"))); //FIXME text?
    qSpiRoleMapping.insert(QAccessible::Slider, RoleNames(ROLE_SLIDER, "slider", QSpiObject::tr("slider")));
    qSpiRoleMapping.insert(QAccessible::SpinBox, RoleNames(ROLE_SPIN_BUTTON, "spin button", QSpiObject::tr("spin box")));
    qSpiRoleMapping.insert(QAccessible::Canvas, RoleNames(ROLE_CANVAS, "canvas", QSpiObject::tr("canvas")));
    qSpiRoleMapping.insert(QAccessible::Animation, RoleNames(ROLE_ANIMATION, "animation", QSpiObject::tr("animation")));
    qSpiRoleMapping.insert(QAccessible::Equation, RoleNames(ROLE_TEXT, "text", QSpiObject::tr("equation")));
    qSpiRoleMapping.insert(QAccessible::ButtonDropDown, RoleNames(ROLE_PUSH_BUTTON, "push button", QSpiObject::tr("button drop down")));
    qSpiRoleMapping.insert(QAccessible::ButtonMenu, RoleNames(ROLE_PUSH_BUTTON, "push button", QSpiObject::tr("button menu")));
    qSpiRoleMapping.insert(QAccessible::ButtonDropGrid, RoleNames(ROLE_PUSH_BUTTON, "push button", QSpiObject::tr("button drop grid")));
    qSpiRoleMapping.insert(QAccessible::Whitespace, RoleNames(ROLE_FILLER, "filler", QSpiObject::tr("whitespace")));
    qSpiRoleMapping.insert(QAccessible::PageTabList, RoleNames(ROLE_PAGE_TAB_LIST, "page tab list", QSpiObject::tr("page tab list")));
    qSpiRoleMapping.insert(QAccessible::Clock, RoleNames(ROLE_UNKNOWN, "unknown", QSpiObject::tr("clock")));
    qSpiRoleMapping.insert(QAccessible::Splitter, RoleNames(ROLE_SPLIT_PANE, "split pane", QSpiObject::tr("splitter")));
    qSpiRoleMapping.insert(QAccessible::LayeredPane, RoleNames(ROLE_LAYERED_PANE, "layered pane", QSpiObject::tr("layered pane")));
    qSpiRoleMapping.insert(QAccessible::UserRole, RoleNames(ROLE_UNKNOWN, "unknown", QSpiObject::tr("user role")));
}

/*---------------------------------------------------------------------------*/

void qspi_stateset_from_qstate(QAccessible::State state, QSpiUIntList &set)
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
