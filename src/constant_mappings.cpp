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

#include <QtCore/QHash>
#include <QAccessible>

#include "constant_mappings.h"

#define BITARRAY_SEQ_TERM 0xffffffff

#define BITARRAY_SET(p, n) ((p)[n>>5] |= (1<<(n&31)))

/*---------------------------------------------------------------------------*/

QHash <QAccessible::Role, QSpiRole> qSpiRoleMapping;

static void initialize_role_mapping ()
{
       qSpiRoleMapping.insert (QAccessible::NoRole, ROLE_INVALID);
       qSpiRoleMapping.insert (QAccessible::TitleBar, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::MenuBar, ROLE_MENU_BAR);
       qSpiRoleMapping.insert (QAccessible::ScrollBar, ROLE_SCROLL_BAR);
       qSpiRoleMapping.insert (QAccessible::Grip, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::Sound, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::Cursor, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::Caret, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::AlertMessage, ROLE_ALERT);
       qSpiRoleMapping.insert (QAccessible::Window, ROLE_WINDOW);
       qSpiRoleMapping.insert (QAccessible::Client, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::PopupMenu, ROLE_POPUP_MENU);
       qSpiRoleMapping.insert (QAccessible::MenuItem, ROLE_MENU_ITEM);
       qSpiRoleMapping.insert (QAccessible::ToolTip, ROLE_TOOL_TIP);
       qSpiRoleMapping.insert (QAccessible::Application, ROLE_APPLICATION);
       qSpiRoleMapping.insert (QAccessible::Document, ROLE_DOCUMENT_FRAME);
       qSpiRoleMapping.insert (QAccessible::Pane, ROLE_PANEL);
       qSpiRoleMapping.insert (QAccessible::Chart, ROLE_CHART);
       qSpiRoleMapping.insert (QAccessible::Dialog, ROLE_DIALOG);
       qSpiRoleMapping.insert (QAccessible::Border, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::Grouping, ROLE_PANEL);
       qSpiRoleMapping.insert (QAccessible::Separator, ROLE_SEPARATOR);
       qSpiRoleMapping.insert (QAccessible::ToolBar, ROLE_TOOL_BAR);
       qSpiRoleMapping.insert (QAccessible::StatusBar, ROLE_STATUS_BAR);
       qSpiRoleMapping.insert (QAccessible::Table, ROLE_TABLE);
       qSpiRoleMapping.insert (QAccessible::ColumnHeader, ROLE_TABLE_COLUMN_HEADER);
       qSpiRoleMapping.insert (QAccessible::RowHeader, ROLE_TABLE_ROW_HEADER);
       qSpiRoleMapping.insert (QAccessible::Column, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::Row, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::Cell, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::Link, ROLE_LINK);
       qSpiRoleMapping.insert (QAccessible::HelpBalloon, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::Assistant, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::List, ROLE_LIST);
       qSpiRoleMapping.insert (QAccessible::ListItem, ROLE_LIST_ITEM);
       qSpiRoleMapping.insert (QAccessible::Tree, ROLE_TREE);
       qSpiRoleMapping.insert (QAccessible::TreeItem, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::PageTab, ROLE_PAGE_TAB);
       qSpiRoleMapping.insert (QAccessible::PropertyPage, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::Indicator, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::Graphic, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::StaticText, ROLE_TEXT);
       qSpiRoleMapping.insert (QAccessible::EditableText, ROLE_TEXT);
       qSpiRoleMapping.insert (QAccessible::PushButton, ROLE_PUSH_BUTTON);
       qSpiRoleMapping.insert (QAccessible::CheckBox, ROLE_CHECK_BOX);
       qSpiRoleMapping.insert (QAccessible::RadioButton, ROLE_RADIO_BUTTON);
       qSpiRoleMapping.insert (QAccessible::ComboBox, ROLE_COMBO_BOX);
       qSpiRoleMapping.insert (QAccessible::ProgressBar, ROLE_PROGRESS_BAR);
       qSpiRoleMapping.insert (QAccessible::Dial, ROLE_DIAL);
       qSpiRoleMapping.insert (QAccessible::HotkeyField, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::Slider, ROLE_SLIDER);
       qSpiRoleMapping.insert (QAccessible::SpinBox, ROLE_SPIN_BUTTON);
       qSpiRoleMapping.insert (QAccessible::Canvas, ROLE_CANVAS);
       qSpiRoleMapping.insert (QAccessible::Animation, ROLE_ANIMATION);
       qSpiRoleMapping.insert (QAccessible::Equation, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::ButtonDropDown, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::ButtonMenu, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::ButtonDropGrid, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::Whitespace, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::PageTabList, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::Clock, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::Splitter, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::LayeredPane, ROLE_UNKNOWN);
       qSpiRoleMapping.insert (QAccessible::UserRole, ROLE_UNKNOWN);
}

/*---------------------------------------------------------------------------*/

QHash <int, QSpiState> qSpiStateMapping;

static void initialize_state_mapping ()
{
       qSpiStateMapping.insert (int(QAccessible::Normal), STATE_ENABLED); /*STATE_ENABLED/SHOWING/VISIBLE*/
       qSpiStateMapping.insert (int(QAccessible::Unavailable), STATE_UNKNOWN); /*STATE_INVALID?*/
       qSpiStateMapping.insert (int(QAccessible::Selected), STATE_SELECTED);
       qSpiStateMapping.insert (int(QAccessible::Focused), STATE_FOCUSED);
       qSpiStateMapping.insert (int(QAccessible::Pressed), STATE_PRESSED);
       qSpiStateMapping.insert (int(QAccessible::Checked), STATE_CHECKED);
       qSpiStateMapping.insert (int(QAccessible::Mixed), STATE_UNKNOWN);
       qSpiStateMapping.insert (int(QAccessible::ReadOnly), STATE_UNKNOWN); /*STATE_EDITABLE if not present?*/
       qSpiStateMapping.insert (int(QAccessible::HotTracked), STATE_UNKNOWN);
       qSpiStateMapping.insert (int(QAccessible::DefaultButton), STATE_UNKNOWN);
       qSpiStateMapping.insert (int(QAccessible::Expanded), STATE_EXPANDED);
       qSpiStateMapping.insert (int(QAccessible::Collapsed), STATE_COLLAPSED);
       qSpiStateMapping.insert (int(QAccessible::Busy), STATE_BUSY);
       qSpiStateMapping.insert (int(QAccessible::Marqueed), STATE_UNKNOWN);
       qSpiStateMapping.insert (int(QAccessible::Animated), STATE_ANIMATED);
       qSpiStateMapping.insert (int(QAccessible::Invisible), STATE_UNKNOWN); /*!STATE_VISIBLE if present?*/
       qSpiStateMapping.insert (int(QAccessible::Offscreen), STATE_UNKNOWN); /*!STATE_VISIBLE if present?*/
       qSpiStateMapping.insert (int(QAccessible::Sizeable), STATE_UNKNOWN);
       qSpiStateMapping.insert (int(QAccessible::Movable), STATE_UNKNOWN);
       qSpiStateMapping.insert (int(QAccessible::SelfVoicing), STATE_UNKNOWN);
       qSpiStateMapping.insert (int(QAccessible::Focusable), STATE_FOCUSABLE);
       qSpiStateMapping.insert (int(QAccessible::Selectable), STATE_SELECTABLE);
       qSpiStateMapping.insert (int(QAccessible::Linked), STATE_UNKNOWN);
       qSpiStateMapping.insert (int(QAccessible::Traversed), STATE_VISITED);
       qSpiStateMapping.insert (int(QAccessible::MultiSelectable), STATE_MULTISELECTABLE);
       qSpiStateMapping.insert (int(QAccessible::ExtSelectable), STATE_UNKNOWN);
       qSpiStateMapping.insert (int(QAccessible::Protected), STATE_UNKNOWN);
       qSpiStateMapping.insert (int(QAccessible::HasPopup), STATE_UNKNOWN);
       qSpiStateMapping.insert (int(QAccessible::Modal), STATE_MODAL);
       qSpiStateMapping.insert (int(QAccessible::HasInvokeExtension), STATE_UNKNOWN);
}

void qspi_stateset_from_qstate (QAccessible::State state, QSpiStateSet &set)
{
       int array[2] = {0, 0};

       for (int mask = 1; mask <= QAccessible::HasInvokeExtension; mask <<= 1)
       {
           if (state & mask)
           {
               int a = qSpiStateMapping.value(state & mask);
               BITARRAY_SET (array, a);
           }
       }
       set.one = array[0];
       set.two = array[1];
}

/*---------------------------------------------------------------------------*/

void qspi_initialize_constant_mappings ()
{
       initialize_role_mapping ();
       initialize_state_mapping ();
}

/*END------------------------------------------------------------------------*/
