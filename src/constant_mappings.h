/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2008, 2009      Nokia Corporation
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

/*
 * This file contains AT-SPI constants and mappings between QAccessible
 * and AT-SPI constants such as 'role' and 'state' enumerations.
 */

#ifndef Q_SPI_CONSTANT_MAPPINGS_H
#define Q_SPI_CONSTANT_MAPPINGS_H

#include "adaptor_marshallers.h"

/* Role mapping */
/*---------------------------------------------------------------------------*/

enum QSpiRole
{
    ROLE_INVALID,
    ROLE_ACCELERATOR_LABEL,
    ROLE_ALERT,
    ROLE_ANIMATION,
    ROLE_ARROW,
    ROLE_CALENDAR,
    ROLE_CANVAS,
    ROLE_CHECK_BOX,
    ROLE_CHECK_MENU_ITEM,
    ROLE_COLOR_CHOOSER,
    ROLE_COLUMN_HEADER,
    ROLE_COMBO_BOX,
    ROLE_DATE_EDITOR,
    ROLE_DESKTOP_ICON,
    ROLE_DESKTOP_FRAME,
    ROLE_DIAL,
    ROLE_DIALOG,
    ROLE_DIRECTORY_PANE,
    ROLE_DRAWING_AREA,
    ROLE_FILE_CHOOSER,
    ROLE_FILLER,
    ROLE_FOCUS_TRAVERSABLE,
    ROLE_FONT_CHOOSER,
    ROLE_FRAME,
    ROLE_GLASS_PANE,
    ROLE_HTML_CONTAINER,
    ROLE_ICON,
    ROLE_IMAGE,
    ROLE_INTERNAL_FRAME,
    ROLE_LABEL,
    ROLE_LAYERED_PANE,
    ROLE_LIST,
    ROLE_LIST_ITEM,
    ROLE_MENU,
    ROLE_MENU_BAR,
    ROLE_MENU_ITEM,
    ROLE_OPTION_PANE,
    ROLE_PAGE_TAB,
    ROLE_PAGE_TAB_LIST,
    ROLE_PANEL,
    ROLE_PASSWORD_TEXT,
    ROLE_POPUP_MENU,
    ROLE_PROGRESS_BAR,
    ROLE_PUSH_BUTTON,
    ROLE_RADIO_BUTTON,
    ROLE_RADIO_MENU_ITEM,
    ROLE_ROOT_PANE,
    ROLE_ROW_HEADER,
    ROLE_SCROLL_BAR,
    ROLE_SCROLL_PANE,
    ROLE_SEPARATOR,
    ROLE_SLIDER,
    ROLE_SPIN_BUTTON,
    ROLE_SPLIT_PANE,
    ROLE_STATUS_BAR,
    ROLE_TABLE,
    ROLE_TABLE_CELL,
    ROLE_TABLE_COLUMN_HEADER,
    ROLE_TABLE_ROW_HEADER,
    ROLE_TEAROFF_MENU_ITEM,
    ROLE_TERMINAL,
    ROLE_TEXT,
    ROLE_TOGGLE_BUTTON,
    ROLE_TOOL_BAR,
    ROLE_TOOL_TIP,
    ROLE_TREE,
    ROLE_TREE_TABLE,
    ROLE_UNKNOWN,
    ROLE_VIEWPORT,
    ROLE_WINDOW,
    ROLE_EXTENDED,
    ROLE_HEADER,
    ROLE_FOOTER,
    ROLE_PARAGRAPH,
    ROLE_RULER,
    ROLE_APPLICATION,
    ROLE_AUTOCOMPLETE,
    ROLE_EDITBAR,
    ROLE_EMBEDDED,
    ROLE_ENTRY,
    ROLE_CHART,
    ROLE_CAPTION,
    ROLE_DOCUMENT_FRAME,
    ROLE_HEADING,
    ROLE_PAGE,
    ROLE_SECTION,
    ROLE_REDUNDANT_OBJECT,
    ROLE_FORM,
    ROLE_LINK,
    ROLE_INPUT_METHOD_WINDOW,
    ROLE_LAST_DEFINED
};

extern QHash <QAccessible::Role, QSpiRole> qSpiRoleMapping;

/* State mapping */
/*---------------------------------------------------------------------------*/

enum QSpiState {
    STATE_INVALID,
    STATE_ACTIVE,
    STATE_ARMED,
    STATE_BUSY,
    STATE_CHECKED,
    STATE_COLLAPSED,
    STATE_DEFUNCT,
    STATE_EDITABLE,
    STATE_ENABLED,
    STATE_EXPANDABLE,
    STATE_EXPANDED,
    STATE_FOCUSABLE,
    STATE_FOCUSED,
    STATE_HAS_TOOLTIP,
    STATE_HORIZONTAL,
    STATE_ICONIFIED,
    STATE_MODAL,
    STATE_MULTI_LINE,
    STATE_MULTISELECTABLE,
    STATE_OPAQUE,
    STATE_PRESSED,
    STATE_RESIZABLE,
    STATE_SELECTABLE,
    STATE_SELECTED,
    STATE_SENSITIVE,
    STATE_SHOWING,
    STATE_SINGLE_LINE,
    STATE_STALE,
    STATE_TRANSIENT,
    STATE_VERTICAL,
    STATE_VISIBLE,
    STATE_MANAGES_DESCENDANTS,
    STATE_INDETERMINATE,
    STATE_REQUIRED,
    STATE_TRUNCATED,
    STATE_ANIMATED,
    STATE_INVALID_ENTRY,
    STATE_SUPPORTS_AUTOCOMPLETION,
    STATE_SELECTABLE_TEXT,
    STATE_IS_DEFAULT,
    STATE_VISITED,
    STATE_UNKNOWN,
    STATE_LAST_DEFINED
};

extern QHash <int, QSpiState> qSpiStateMapping;

void qspi_stateset_from_qstate (QAccessible::State state, QSpiStateSet &set);

/*---------------------------------------------------------------------------*/

void qspi_initialize_constant_mappings ();

#endif /* Q_SPI_CONSTANT_MAPPINGS_H */
