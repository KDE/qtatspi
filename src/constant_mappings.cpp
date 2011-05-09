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
#include "adaptor.h"

// FIXME the assignment of roles is quite arbitrary, at some point go through this list and sort and check that it makes sense
//  "calendar" "check menu item"  "color chooser" "column header"    "dateeditor"  "desktop icon"  "desktop frame"
//  "directory pane"  "drawing area"  "file chooser" "fontchooser"  "frame"  "glass pane"  "html container"  "icon"
//  "internal frame"  "option pane"  "password text" "radio menu item"  "root pane"  "row header"    "scroll pane"
//  "tear off menu item"  "terminal" "toggle button" "tree table"  "unknown"  "viewport" "header"  "footer"  "paragraph"
//  "ruler"    "autocomplete"  "edit bar" "embedded component"  "entry"    "caption"
//  "heading"  "page"  "section"  "redundant object"  "form"  "input method window"  "menu"

QHash <QAccessible::Role, RoleNames> qSpiRoleMapping;

static void initializeRoleMapping ()
{
    qSpiRoleMapping.insert(QAccessible::NoRole, RoleNames(ROLE_INVALID, "invalid", QSpiAdaptor::tr("invalid role")));
    qSpiRoleMapping.insert(QAccessible::TitleBar, RoleNames(ROLE_TEXT, "text", QSpiAdaptor::tr("title bar")));
    qSpiRoleMapping.insert(QAccessible::MenuBar, RoleNames(ROLE_MENU_BAR, "menu bar", QSpiAdaptor::tr("menu bar")));
    qSpiRoleMapping.insert(QAccessible::ScrollBar, RoleNames(ROLE_SCROLL_BAR, "scroll bar", QSpiAdaptor::tr("scroll bar")));
    qSpiRoleMapping.insert(QAccessible::Grip, RoleNames(ROLE_UNKNOWN, "unknown", QSpiAdaptor::tr("grip")));
    qSpiRoleMapping.insert(QAccessible::Sound, RoleNames(ROLE_UNKNOWN, "unknown", QSpiAdaptor::tr("sound")));
    qSpiRoleMapping.insert(QAccessible::Cursor, RoleNames(ROLE_ARROW, "arrow", QSpiAdaptor::tr("cursor")));
    qSpiRoleMapping.insert(QAccessible::Caret, RoleNames(ROLE_UNKNOWN, "unknown", QSpiAdaptor::tr("caret")));
    qSpiRoleMapping.insert(QAccessible::AlertMessage, RoleNames(ROLE_ALERT, "alert", QSpiAdaptor::tr("alert message")));
    qSpiRoleMapping.insert(QAccessible::Window, RoleNames(ROLE_WINDOW, "window", QSpiAdaptor::tr("window")));
    qSpiRoleMapping.insert(QAccessible::Client, RoleNames(ROLE_FILLER, "filler", QSpiAdaptor::tr("client")));
    qSpiRoleMapping.insert(QAccessible::PopupMenu, RoleNames(ROLE_POPUP_MENU, "popup menu", QSpiAdaptor::tr("popup menu")));
    qSpiRoleMapping.insert(QAccessible::MenuItem, RoleNames(ROLE_MENU_ITEM, "menu item", QSpiAdaptor::tr("menu item")));
    qSpiRoleMapping.insert(QAccessible::ToolTip, RoleNames(ROLE_TOOL_TIP, "tool tip", QSpiAdaptor::tr("tool tip")));
    qSpiRoleMapping.insert(QAccessible::Application, RoleNames(ROLE_APPLICATION, "application", QSpiAdaptor::tr("application")));
    qSpiRoleMapping.insert(QAccessible::Document, RoleNames(ROLE_DOCUMENT_FRAME, "document frame", QSpiAdaptor::tr("document")));
    qSpiRoleMapping.insert(QAccessible::Pane, RoleNames(ROLE_PANEL, "panel", QSpiAdaptor::tr("pane")));
    qSpiRoleMapping.insert(QAccessible::Chart, RoleNames(ROLE_CHART, "chart", QSpiAdaptor::tr("chart")));
    qSpiRoleMapping.insert(QAccessible::Dialog, RoleNames(ROLE_DIALOG, "dialog", QSpiAdaptor::tr("dialog")));
    qSpiRoleMapping.insert(QAccessible::Border, RoleNames(ROLE_FRAME, "frame", QSpiAdaptor::tr("border")));
    qSpiRoleMapping.insert(QAccessible::Grouping, RoleNames(ROLE_PANEL, "panel", QSpiAdaptor::tr("grouping")));
    qSpiRoleMapping.insert(QAccessible::Separator, RoleNames(ROLE_SEPARATOR, "separator", QSpiAdaptor::tr("separator")));
    qSpiRoleMapping.insert(QAccessible::ToolBar, RoleNames(ROLE_TOOL_BAR, "tool bar", QSpiAdaptor::tr("tool bar")));
    qSpiRoleMapping.insert(QAccessible::StatusBar, RoleNames(ROLE_STATUS_BAR, "statusbar", QSpiAdaptor::tr("status bar")));
    qSpiRoleMapping.insert(QAccessible::Table, RoleNames(ROLE_TABLE, "table", QSpiAdaptor::tr("table")));
    qSpiRoleMapping.insert(QAccessible::ColumnHeader, RoleNames(ROLE_TABLE_COLUMN_HEADER, "column header", QSpiAdaptor::tr("column header")));
    qSpiRoleMapping.insert(QAccessible::RowHeader, RoleNames(ROLE_TABLE_ROW_HEADER, "row header", QSpiAdaptor::tr("row header")));
    qSpiRoleMapping.insert(QAccessible::Column, RoleNames(ROLE_TABLE_CELL, "table cell", QSpiAdaptor::tr("column")));
    qSpiRoleMapping.insert(QAccessible::Row, RoleNames(ROLE_TABLE_CELL, "table cell", QSpiAdaptor::tr("row")));
    qSpiRoleMapping.insert(QAccessible::Cell, RoleNames(ROLE_TABLE_CELL, "table cell", QSpiAdaptor::tr("cell")));
    qSpiRoleMapping.insert(QAccessible::Link, RoleNames(ROLE_LINK, "link" , QSpiAdaptor::tr("link")));
    qSpiRoleMapping.insert(QAccessible::HelpBalloon, RoleNames(ROLE_DIALOG, "dialog", QSpiAdaptor::tr("help balloon")));
    qSpiRoleMapping.insert(QAccessible::Assistant, RoleNames(ROLE_DIALOG, "dialog", QSpiAdaptor::tr("assistant")));
    qSpiRoleMapping.insert(QAccessible::List, RoleNames(ROLE_LIST, "list", QSpiAdaptor::tr("list")));
    qSpiRoleMapping.insert(QAccessible::ListItem, RoleNames(ROLE_LIST_ITEM, "list item", QSpiAdaptor::tr("list item")));
    qSpiRoleMapping.insert(QAccessible::Tree, RoleNames(ROLE_TREE, "tree", QSpiAdaptor::tr("tree")));
    qSpiRoleMapping.insert(QAccessible::TreeItem, RoleNames(ROLE_TABLE_CELL, "table cell", QSpiAdaptor::tr("tree item")));
    qSpiRoleMapping.insert(QAccessible::PageTab, RoleNames(ROLE_PAGE_TAB, "page tab", QSpiAdaptor::tr("page tab")));
    qSpiRoleMapping.insert(QAccessible::PropertyPage, RoleNames(ROLE_PAGE_TAB, "page tab", QSpiAdaptor::tr("property page")));
    qSpiRoleMapping.insert(QAccessible::Indicator, RoleNames(ROLE_UNKNOWN, "unknown", QSpiAdaptor::tr("indicator")));
    qSpiRoleMapping.insert(QAccessible::Graphic, RoleNames(ROLE_IMAGE, "image", QSpiAdaptor::tr("graphic")));
    qSpiRoleMapping.insert(QAccessible::StaticText, RoleNames(ROLE_LABEL, "label", QSpiAdaptor::tr("static text")));
    qSpiRoleMapping.insert(QAccessible::EditableText, RoleNames(ROLE_TEXT, "text", QSpiAdaptor::tr("editable text")));
    qSpiRoleMapping.insert(QAccessible::PushButton, RoleNames(ROLE_PUSH_BUTTON, "push button", QSpiAdaptor::tr("push button")));
    qSpiRoleMapping.insert(QAccessible::CheckBox, RoleNames(ROLE_CHECK_BOX, "check box", QSpiAdaptor::tr("check box")));
    qSpiRoleMapping.insert(QAccessible::RadioButton, RoleNames(ROLE_RADIO_BUTTON, "radio button", QSpiAdaptor::tr("radio box")));
    qSpiRoleMapping.insert(QAccessible::ComboBox, RoleNames(ROLE_COMBO_BOX, "combo box", QSpiAdaptor::tr("combo box")));
    qSpiRoleMapping.insert(QAccessible::ProgressBar, RoleNames(ROLE_PROGRESS_BAR, "progress bar", QSpiAdaptor::tr("progress bar")));
    qSpiRoleMapping.insert(QAccessible::Dial, RoleNames(ROLE_DIAL, "accelerator label", QSpiAdaptor::tr("dial")));
    qSpiRoleMapping.insert(QAccessible::HotkeyField, RoleNames(ROLE_TEXT, "text", QSpiAdaptor::tr("hotkey field"))); //FIXME text?
    qSpiRoleMapping.insert(QAccessible::Slider, RoleNames(ROLE_SLIDER, "slider", QSpiAdaptor::tr("slider")));
    qSpiRoleMapping.insert(QAccessible::SpinBox, RoleNames(ROLE_SPIN_BUTTON, "spin button", QSpiAdaptor::tr("spin box")));
    qSpiRoleMapping.insert(QAccessible::Canvas, RoleNames(ROLE_CANVAS, "canvas", QSpiAdaptor::tr("canvas")));
    qSpiRoleMapping.insert(QAccessible::Animation, RoleNames(ROLE_ANIMATION, "animation", QSpiAdaptor::tr("animation")));
    qSpiRoleMapping.insert(QAccessible::Equation, RoleNames(ROLE_TEXT, "text", QSpiAdaptor::tr("equation")));
    qSpiRoleMapping.insert(QAccessible::ButtonDropDown, RoleNames(ROLE_PUSH_BUTTON, "push button", QSpiAdaptor::tr("button drop down")));
    qSpiRoleMapping.insert(QAccessible::ButtonMenu, RoleNames(ROLE_PUSH_BUTTON, "push button", QSpiAdaptor::tr("button menu")));
    qSpiRoleMapping.insert(QAccessible::ButtonDropGrid, RoleNames(ROLE_PUSH_BUTTON, "push button", QSpiAdaptor::tr("button drop grid")));
    qSpiRoleMapping.insert(QAccessible::Whitespace, RoleNames(ROLE_FILLER, "filler", QSpiAdaptor::tr("whitespace")));
    qSpiRoleMapping.insert(QAccessible::PageTabList, RoleNames(ROLE_PAGE_TAB_LIST, "page tab list", QSpiAdaptor::tr("page tab list")));
    qSpiRoleMapping.insert(QAccessible::Clock, RoleNames(ROLE_UNKNOWN, "unknown", QSpiAdaptor::tr("clock")));
    qSpiRoleMapping.insert(QAccessible::Splitter, RoleNames(ROLE_SPLIT_PANE, "split pane", QSpiAdaptor::tr("splitter")));
    qSpiRoleMapping.insert(QAccessible::LayeredPane, RoleNames(ROLE_LAYERED_PANE, "layered pane", QSpiAdaptor::tr("layered pane")));
    qSpiRoleMapping.insert(QAccessible::UserRole, RoleNames(ROLE_UNKNOWN, "unknown", QSpiAdaptor::tr("user role")));
}

quint64 spiStatesFromQState(QAccessible::State state)
{
    quint64 spiState = 0;

    setSpiStateBit(&spiState, STATE_EDITABLE);
    setSpiStateBit(&spiState, STATE_ENABLED);
    setSpiStateBit(&spiState, STATE_SHOWING);
    setSpiStateBit(&spiState, STATE_VISIBLE);
    setSpiStateBit(&spiState, STATE_SENSITIVE);

    for (int mask = 1; mask <= int(QAccessible::HasInvokeExtension); mask <<= 1)
    {
        switch (state & mask)
        {
        case QAccessible::Unavailable:
        {
            unsetSpiStateBit(&spiState, STATE_ENABLED);
            unsetSpiStateBit(&spiState, STATE_SHOWING);
            unsetSpiStateBit(&spiState, STATE_VISIBLE);
            unsetSpiStateBit(&spiState, STATE_SENSITIVE);
            break;
        }
        case QAccessible::Selected:
        {
            setSpiStateBit(&spiState, STATE_SELECTED);
            break;
        }
        case QAccessible::Focused:
        {
            setSpiStateBit(&spiState, STATE_FOCUSED);
            break;
        }
        case QAccessible::Pressed:
        {
            setSpiStateBit(&spiState, STATE_PRESSED);
            break;
        }
        case QAccessible::Checked:
        {
            setSpiStateBit(&spiState, STATE_CHECKED);
            break;
        }
        case QAccessible::Mixed:
        {
            setSpiStateBit(&spiState, STATE_INDETERMINATE);
            break;
        }
        case QAccessible::ReadOnly:
        {
            unsetSpiStateBit(&spiState, STATE_EDITABLE);
            break;
        }
        case QAccessible::HotTracked:
        {
            break;
        }
        case QAccessible::DefaultButton:
        {
            setSpiStateBit(&spiState, STATE_IS_DEFAULT);
            break;
        }
        case QAccessible::Expanded:
        {
            setSpiStateBit(&spiState, STATE_EXPANDED);
            break;
        }
        case QAccessible::Collapsed:
        {
            setSpiStateBit(&spiState, STATE_COLLAPSED);
            break;
        }
        case QAccessible::Busy:
        {
            setSpiStateBit(&spiState, STATE_BUSY);
            break;
        }
        case QAccessible::Marqueed:
        case QAccessible::Animated:
        {
            setSpiStateBit(&spiState, STATE_ANIMATED);
            break;
        }
        case QAccessible::Invisible:
        case QAccessible::Offscreen:
        {
            unsetSpiStateBit(&spiState, STATE_SHOWING);
            break;
        }
        case QAccessible::Sizeable:
        {
            setSpiStateBit(&spiState, STATE_RESIZABLE);
            break;
        }
        case QAccessible::Movable:
        case QAccessible::SelfVoicing:
        {
            break;
        }
        case QAccessible::Focusable:
        {
            setSpiStateBit(&spiState, STATE_FOCUSABLE);
            break;
        }
        case QAccessible::Selectable:
        {
            setSpiStateBit(&spiState, STATE_SELECTABLE);
            break;
        }
        case QAccessible::Linked:
        {
            break;
        }
        case QAccessible::Traversed:
        {
            setSpiStateBit(&spiState, STATE_VISITED);
            break;
        }
        case QAccessible::MultiSelectable:
        {
            setSpiStateBit(&spiState, STATE_MULTISELECTABLE);
            break;
        }
        case QAccessible::ExtSelectable:
        {
            setSpiStateBit(&spiState, STATE_SELECTABLE);
            break;
        }
        case QAccessible::Protected:
        case QAccessible::HasPopup:
        {
            break;
        }
        case QAccessible::Modal:
        {
            setSpiStateBit(&spiState, STATE_MODAL);
            break;
        }
        case QAccessible::HasInvokeExtension:
        {
            break;
        }
//        case QAccessible::SingleLine:
//        {
//            setSpiStateBit(&spiState, STATE_SINGLE_LINE);
//            break;
//        }
        default:
        {
            break;
        }
        }
    }
    return spiState;
}

QSpiUIntList spiStateSetFromSpiStates(quint64 states)
{
    uint low = states & 0xFFFFFFFF;
    uint high = (states >> 32) & 0xFFFFFFFF;

    QSpiUIntList stateList;
    stateList.append(low);
    stateList.append(high);
    return stateList;
}

void qSpiInitializeConstantMappings()
{
    initializeRoleMapping();
}
