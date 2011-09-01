/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2008-2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "constant_mappings.h"

#include <qobject.h>

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
    qSpiRoleMapping.insert(QAccessible::NoRole, RoleNames(ATSPI_ROLE_INVALID, "invalid", QObject::tr("invalid role")));
    qSpiRoleMapping.insert(QAccessible::TitleBar, RoleNames(ATSPI_ROLE_TEXT, "text", QObject::tr("title bar")));
    qSpiRoleMapping.insert(QAccessible::MenuBar, RoleNames(ATSPI_ROLE_MENU_BAR, "menu bar", QObject::tr("menu bar")));
    qSpiRoleMapping.insert(QAccessible::ScrollBar, RoleNames(ATSPI_ROLE_SCROLL_BAR, "scroll bar", QObject::tr("scroll bar")));
    qSpiRoleMapping.insert(QAccessible::Grip, RoleNames(ATSPI_ROLE_UNKNOWN, "unknown", QObject::tr("grip")));
    qSpiRoleMapping.insert(QAccessible::Sound, RoleNames(ATSPI_ROLE_UNKNOWN, "unknown", QObject::tr("sound")));
    qSpiRoleMapping.insert(QAccessible::Cursor, RoleNames(ATSPI_ROLE_ARROW, "arrow", QObject::tr("cursor")));
    qSpiRoleMapping.insert(QAccessible::Caret, RoleNames(ATSPI_ROLE_UNKNOWN, "unknown", QObject::tr("caret")));
    qSpiRoleMapping.insert(QAccessible::AlertMessage, RoleNames(ATSPI_ROLE_ALERT, "alert", QObject::tr("alert message")));
    qSpiRoleMapping.insert(QAccessible::Window, RoleNames(ATSPI_ROLE_WINDOW, "window", QObject::tr("window")));
    qSpiRoleMapping.insert(QAccessible::Client, RoleNames(ATSPI_ROLE_FILLER, "filler", QObject::tr("filler")));
    qSpiRoleMapping.insert(QAccessible::PopupMenu, RoleNames(ATSPI_ROLE_POPUP_MENU, "popup menu", QObject::tr("popup menu")));
    qSpiRoleMapping.insert(QAccessible::MenuItem, RoleNames(ATSPI_ROLE_MENU_ITEM, "menu item", QObject::tr("menu item")));
    qSpiRoleMapping.insert(QAccessible::ToolTip, RoleNames(ATSPI_ROLE_TOOL_TIP, "tool tip", QObject::tr("tool tip")));
    qSpiRoleMapping.insert(QAccessible::Application, RoleNames(ATSPI_ROLE_APPLICATION, "application", QObject::tr("application")));
    qSpiRoleMapping.insert(QAccessible::Document, RoleNames(ATSPI_ROLE_DOCUMENT_FRAME, "document frame", QObject::tr("document")));
    qSpiRoleMapping.insert(QAccessible::Pane, RoleNames(ATSPI_ROLE_PANEL, "panel", QObject::tr("pane")));
    qSpiRoleMapping.insert(QAccessible::Chart, RoleNames(ATSPI_ROLE_CHART, "chart", QObject::tr("chart")));
    qSpiRoleMapping.insert(QAccessible::Dialog, RoleNames(ATSPI_ROLE_DIALOG, "dialog", QObject::tr("dialog")));
    qSpiRoleMapping.insert(QAccessible::Border, RoleNames(ATSPI_ROLE_FRAME, "frame", QObject::tr("border")));
    qSpiRoleMapping.insert(QAccessible::Grouping, RoleNames(ATSPI_ROLE_PANEL, "panel", QObject::tr("grouping")));
    qSpiRoleMapping.insert(QAccessible::Separator, RoleNames(ATSPI_ROLE_SEPARATOR, "separator", QObject::tr("separator")));
    qSpiRoleMapping.insert(QAccessible::ToolBar, RoleNames(ATSPI_ROLE_TOOL_BAR, "tool bar", QObject::tr("tool bar")));
    qSpiRoleMapping.insert(QAccessible::StatusBar, RoleNames(ATSPI_ROLE_STATUS_BAR, "statusbar", QObject::tr("status bar")));
    qSpiRoleMapping.insert(QAccessible::Table, RoleNames(ATSPI_ROLE_TABLE, "table", QObject::tr("table")));
    qSpiRoleMapping.insert(QAccessible::ColumnHeader, RoleNames(ATSPI_ROLE_TABLE_COLUMN_HEADER, "column header", QObject::tr("column header")));
    qSpiRoleMapping.insert(QAccessible::RowHeader, RoleNames(ATSPI_ROLE_TABLE_ROW_HEADER, "row header", QObject::tr("row header")));
    qSpiRoleMapping.insert(QAccessible::Column, RoleNames(ATSPI_ROLE_TABLE_CELL, "table cell", QObject::tr("column")));
    qSpiRoleMapping.insert(QAccessible::Row, RoleNames(ATSPI_ROLE_TABLE_CELL, "table cell", QObject::tr("row")));
    qSpiRoleMapping.insert(QAccessible::Cell, RoleNames(ATSPI_ROLE_TABLE_CELL, "table cell", QObject::tr("cell")));
    qSpiRoleMapping.insert(QAccessible::Link, RoleNames(ATSPI_ROLE_LINK, "link" , QObject::tr("link")));
    qSpiRoleMapping.insert(QAccessible::HelpBalloon, RoleNames(ATSPI_ROLE_DIALOG, "dialog", QObject::tr("help balloon")));
    qSpiRoleMapping.insert(QAccessible::Assistant, RoleNames(ATSPI_ROLE_DIALOG, "dialog", QObject::tr("assistant")));
    qSpiRoleMapping.insert(QAccessible::List, RoleNames(ATSPI_ROLE_LIST, "list", QObject::tr("list")));
    qSpiRoleMapping.insert(QAccessible::ListItem, RoleNames(ATSPI_ROLE_LIST_ITEM, "list item", QObject::tr("list item")));
    qSpiRoleMapping.insert(QAccessible::Tree, RoleNames(ATSPI_ROLE_TREE, "tree", QObject::tr("tree")));
    qSpiRoleMapping.insert(QAccessible::TreeItem, RoleNames(ATSPI_ROLE_TABLE_CELL, "tree item", QObject::tr("tree item")));
    qSpiRoleMapping.insert(QAccessible::PageTab, RoleNames(ATSPI_ROLE_PAGE_TAB, "page tab", QObject::tr("page tab")));
    qSpiRoleMapping.insert(QAccessible::PropertyPage, RoleNames(ATSPI_ROLE_PAGE_TAB, "page tab", QObject::tr("property page")));
    qSpiRoleMapping.insert(QAccessible::Indicator, RoleNames(ATSPI_ROLE_UNKNOWN, "unknown", QObject::tr("indicator")));
    qSpiRoleMapping.insert(QAccessible::Graphic, RoleNames(ATSPI_ROLE_IMAGE, "image", QObject::tr("graphic")));
    qSpiRoleMapping.insert(QAccessible::StaticText, RoleNames(ATSPI_ROLE_LABEL, "label", QObject::tr("label")));
    qSpiRoleMapping.insert(QAccessible::EditableText, RoleNames(ATSPI_ROLE_TEXT, "text", QObject::tr("text")));
    qSpiRoleMapping.insert(QAccessible::PushButton, RoleNames(ATSPI_ROLE_PUSH_BUTTON, "push button", QObject::tr("push button")));
    qSpiRoleMapping.insert(QAccessible::CheckBox, RoleNames(ATSPI_ROLE_CHECK_BOX, "check box", QObject::tr("check box")));
    qSpiRoleMapping.insert(QAccessible::RadioButton, RoleNames(ATSPI_ROLE_RADIO_BUTTON, "radio button", QObject::tr("radio box")));
    qSpiRoleMapping.insert(QAccessible::ComboBox, RoleNames(ATSPI_ROLE_COMBO_BOX, "combo box", QObject::tr("combo box")));
    qSpiRoleMapping.insert(QAccessible::ProgressBar, RoleNames(ATSPI_ROLE_PROGRESS_BAR, "progress bar", QObject::tr("progress bar")));
    qSpiRoleMapping.insert(QAccessible::Dial, RoleNames(ATSPI_ROLE_DIAL, "accelerator label", QObject::tr("dial")));
    qSpiRoleMapping.insert(QAccessible::HotkeyField, RoleNames(ATSPI_ROLE_TEXT, "text", QObject::tr("hotkey field"))); //FIXME text?
    qSpiRoleMapping.insert(QAccessible::Slider, RoleNames(ATSPI_ROLE_SLIDER, "slider", QObject::tr("slider")));
    qSpiRoleMapping.insert(QAccessible::SpinBox, RoleNames(ATSPI_ROLE_SPIN_BUTTON, "spin button", QObject::tr("spin box")));
    qSpiRoleMapping.insert(QAccessible::Canvas, RoleNames(ATSPI_ROLE_CANVAS, "canvas", QObject::tr("canvas")));
    qSpiRoleMapping.insert(QAccessible::Animation, RoleNames(ATSPI_ROLE_ANIMATION, "animation", QObject::tr("animation")));
    qSpiRoleMapping.insert(QAccessible::Equation, RoleNames(ATSPI_ROLE_TEXT, "text", QObject::tr("equation")));
    qSpiRoleMapping.insert(QAccessible::ButtonDropDown, RoleNames(ATSPI_ROLE_PUSH_BUTTON, "push button", QObject::tr("button drop down")));
    qSpiRoleMapping.insert(QAccessible::ButtonMenu, RoleNames(ATSPI_ROLE_PUSH_BUTTON, "push button", QObject::tr("button menu")));
    qSpiRoleMapping.insert(QAccessible::ButtonDropGrid, RoleNames(ATSPI_ROLE_PUSH_BUTTON, "push button", QObject::tr("button drop grid")));
    qSpiRoleMapping.insert(QAccessible::Whitespace, RoleNames(ATSPI_ROLE_FILLER, "filler", QObject::tr("whitespace")));
    qSpiRoleMapping.insert(QAccessible::PageTabList, RoleNames(ATSPI_ROLE_PAGE_TAB_LIST, "page tab list", QObject::tr("page tab list")));
    qSpiRoleMapping.insert(QAccessible::Clock, RoleNames(ATSPI_ROLE_UNKNOWN, "unknown", QObject::tr("clock")));
    qSpiRoleMapping.insert(QAccessible::Splitter, RoleNames(ATSPI_ROLE_SPLIT_PANE, "split pane", QObject::tr("splitter")));
    qSpiRoleMapping.insert(QAccessible::LayeredPane, RoleNames(ATSPI_ROLE_LAYERED_PANE, "layered pane", QObject::tr("layered pane")));
    qSpiRoleMapping.insert(QAccessible::UserRole, RoleNames(ATSPI_ROLE_UNKNOWN, "unknown", QObject::tr("user role")));
}

quint64 spiStatesFromQState(QAccessible::State state)
{
    quint64 spiState = 0;

    setSpiStateBit(&spiState, ATSPI_STATE_EDITABLE);
    setSpiStateBit(&spiState, ATSPI_STATE_ENABLED);
    setSpiStateBit(&spiState, ATSPI_STATE_SHOWING);
    setSpiStateBit(&spiState, ATSPI_STATE_VISIBLE);
    setSpiStateBit(&spiState, ATSPI_STATE_SENSITIVE);

    if (state & QAccessible::Unavailable) {
        unsetSpiStateBit(&spiState, ATSPI_STATE_ENABLED);
        unsetSpiStateBit(&spiState, ATSPI_STATE_SHOWING);
        unsetSpiStateBit(&spiState, ATSPI_STATE_VISIBLE);
        unsetSpiStateBit(&spiState, ATSPI_STATE_SENSITIVE);
    }

    if (state & QAccessible::Selected)
        setSpiStateBit(&spiState, ATSPI_STATE_SELECTED);
    if (state & QAccessible::Focused)
        setSpiStateBit(&spiState, ATSPI_STATE_FOCUSED);
    if (state & QAccessible::Pressed)
        setSpiStateBit(&spiState, ATSPI_STATE_PRESSED);
    if (state & QAccessible::Checked)
        setSpiStateBit(&spiState, ATSPI_STATE_CHECKED);
    if (state & QAccessible::Mixed)
        setSpiStateBit(&spiState, ATSPI_STATE_INDETERMINATE);
    if (state & QAccessible::ReadOnly)
        unsetSpiStateBit(&spiState, ATSPI_STATE_EDITABLE);
    //        if (state & QAccessible::HotTracked)
    if (state & QAccessible::DefaultButton)
        setSpiStateBit(&spiState, ATSPI_STATE_IS_DEFAULT);
    if (state & QAccessible::Expanded)
        setSpiStateBit(&spiState, ATSPI_STATE_EXPANDED);
    if (state & QAccessible::Collapsed)
        setSpiStateBit(&spiState, ATSPI_STATE_COLLAPSED);
    if (state & QAccessible::Busy)
        setSpiStateBit(&spiState, ATSPI_STATE_BUSY);
    if ((state & QAccessible::Marqueed)
            || (state & QAccessible::Animated))
        setSpiStateBit(&spiState, ATSPI_STATE_ANIMATED);
    if ((state & QAccessible::Invisible)
            || (state & QAccessible::Offscreen))
        unsetSpiStateBit(&spiState, ATSPI_STATE_SHOWING);
    if (state & QAccessible::Sizeable)
        setSpiStateBit(&spiState, ATSPI_STATE_RESIZABLE);
    //        if (state & QAccessible::Movable)
    //        if (state & QAccessible::SelfVoicing)
    if (state & QAccessible::Focusable)
        setSpiStateBit(&spiState, ATSPI_STATE_FOCUSABLE);
    if (state & QAccessible::Selectable)
        setSpiStateBit(&spiState, ATSPI_STATE_SELECTABLE);
    //        if (state & QAccessible::Linked)
    if (state & QAccessible::Traversed)
        setSpiStateBit(&spiState, ATSPI_STATE_VISITED);
    if (state & QAccessible::MultiSelectable)
        setSpiStateBit(&spiState, ATSPI_STATE_MULTISELECTABLE);
    if (state & QAccessible::ExtSelectable)
        setSpiStateBit(&spiState, ATSPI_STATE_SELECTABLE);
    //        if (state & QAccessible::Protected)
    //        if (state & QAccessible::HasPopup)
    if (state & QAccessible::Modal)
        setSpiStateBit(&spiState, ATSPI_STATE_MODAL);

    // Not implemented in Qt
    //    if (state & QAccessible::SingleLine)
    //        setSpiStateBit(&spiState, ATSPI_STATE_SINGLE_LINE);

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
