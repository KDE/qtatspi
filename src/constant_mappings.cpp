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

#define BITARRAY_SET(p, n)   ( (p)[n>>5] |=  (1<<(n&31)) )
#define BITARRAY_UNSET(p, n) ( (p)[n>>5] &= ~(1<<(n&31)) )

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
       qSpiRoleMapping.insert (QAccessible::Border, ROLE_FRAME);
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
       qSpiRoleMapping.insert (QAccessible::LayeredPane, ROLE_FRAME);
       qSpiRoleMapping.insert (QAccessible::UserRole, ROLE_UNKNOWN);
}

/*---------------------------------------------------------------------------*/

void qspi_stateset_from_qstate (QAccessible::State state, QSpiIntList &set)
{
       int array[2] = {0, 0};

       for (int mask = 1; mask <= int(QAccessible::HasInvokeExtension); mask <<= 1)
       {
           /* We may need to take the role of the object into account when
            * mapping between the state sets
            */
           BITARRAY_SET (array, STATE_EDITABLE);

           switch (state & mask)
           {
                 case QAccessible::Normal:
                 {
                         BITARRAY_SET (array, STATE_ENABLED);
                         BITARRAY_SET (array, STATE_SHOWING);
                         BITARRAY_SET (array, STATE_VISIBLE);
                         BITARRAY_SET (array, STATE_SENSITIVE);
                         break;
                 }
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
                         BITARRAY_UNSET (array, STATE_VISIBLE);
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
