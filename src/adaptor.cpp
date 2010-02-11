/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2009      Nokia.
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

#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

#include <QAccessibleTextInterface>
#include <QAccessibleEditableTextInterface>
#include <QAccessibleTableInterface>
#include <QAccessibleActionInterface>
#include <QAccessibleValueInterface>

#include "object.h"
#include "adaptor.h"
#include "cache.h"
#include "constant_mappings.h"

/* --------------------------------------------------------------------------*/

#define ACCESSIBLE_INTERFACE    getInterface()
#define ACTION_INTERFACE        getInterface().actionInterface()
#define TEXT_INTERFACE          getInterface().textInterface()
#define EDITABLE_TEXT_INTERFACE getInterface().editableTextInterface()
#define TABLE_INTERFACE         getInterface().tableInterface()
#define VALUE_INTERFACE         getInterface().valueInterface()

/* AT-SPI Accessible interface ----------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* This is silly. Should talk about moving this mapping into client bindings */
static const char roles[] =
  "invalid\0"
  "accelerator label\0"
  "alert\0"
  "animation\0"
  "arrow\0"
  "calendar\0"
  "canvas\0"
  "check box\0"
  "check menu item\0"
  "color chooser\0"
  "column header\0"
  "combo box\0"
  "dateeditor\0"
  "desktop icon\0"
  "desktop frame\0"
  "dial\0"
  "dialog\0"
  "directory pane\0"
  "drawing area\0"
  "file chooser\0"
  "filler\0"
  "fontchooser\0"
  "frame\0"
  "glass pane\0"
  "html container\0"
  "icon\0"
  "image\0"
  "internal frame\0"
  "label\0"
  "layered pane\0"
  "list\0"
  "list item\0"
  "menu\0"
  "menu bar\0"
  "menu item\0"
  "option pane\0"
  "page tab\0"
  "page tab list\0"
  "panel\0"
  "password text\0"
  "popup menu\0"
  "progress bar\0"
  "push button\0"
  "radio button\0"
  "radio menu item\0"
  "root pane\0"
  "row header\0"
  "scroll bar\0"
  "scroll pane\0"
  "separator\0"
  "slider\0"
  "split pane\0"
  "spin button\0"
  "statusbar\0"
  "table\0"
  "table cell\0"
  "table column header\0"
  "table row header\0"
  "tear off menu item\0"
  "terminal\0"
  "text\0"
  "toggle button\0"
  "tool bar\0"
  "tool tip\0"
  "tree\0"
  "tree table\0"
  "unknown\0"
  "viewport\0"
  "window\0"
  "header\0"
  "footer\0"
  "paragraph\0"
  "ruler\0"
  "application\0"
  "autocomplete\0"
  "edit bar\0"
  "embedded component\0"
  "entry\0"
  "chart\0"
  "caption\0"
  "document frame\0"
  "heading\0"
  "page\0"
  "section\0"
  "redundant object\0"
  "form\0"
  "link\0"
  "input method window";

static const uint roles_offsets[] = {
  0, 8, 26, 32, 42, 48, 57, 64, 
  74, 90, 104, 118, 128, 139, 152, 166, 
  171, 178, 193, 206, 219, 226, 238, 244, 
  255, 270, 275, 281, 296, 302, 315, 320, 
  330, 335, 344, 354, 366, 375, 389, 395, 
  409, 420, 433, 445, 458, 474, 484, 495, 
  506, 518, 528, 535, 546, 558, 568, 574, 
  585, 605, 622, 641, 650, 655, 669, 678, 
  687, 692, 703, 711, 720, 727, 734, 741, 
  751, 757, 769, 782, 791, 810, 816, 822, 
  830, 845, 853, 858, 866, 883, 888, 893
};

int QSpiAdaptor::childCount() const
{
    return ACCESSIBLE_INTERFACE.childCount();
}

QString QSpiAdaptor::description() const
{
    return ACCESSIBLE_INTERFACE.text(QAccessible::Description, 0);
}

QString QSpiAdaptor::name() const
{
    return ACCESSIBLE_INTERFACE.text(QAccessible::Name, 0);
}

QSpiObjectReference QSpiAdaptor::parent() const
{
    return getParentReference();
}

QSpiObjectReference QSpiAdaptor::GetApplication()
{
    return cache->objectToAccessible (cache->getRoot())->getReference();
}

QSpiAttributeSet QSpiAdaptor::GetAttributes()
{
    // No attributes interface in QAccessible so a blank list seems the sensible option.
    QSpiAttributeSet out0;
    return out0;
}

QSpiObjectReference QSpiAdaptor::GetChildAtIndex(int index)
{
    QList<QSpiObject *> children;

    for (int i = 1; i <= interface->childCount (); i++)
    {
        QAccessibleInterface *child = NULL;
        interface->navigate(QAccessible::Child, i, &child);
        if (child)
        {
            QSpiObject *current;
            current = cache->objectToAccessible (child->object ());
            if (current)
                children << current;
            delete child;
        }
    }
    return children.value(index)->getReference();
}

int QSpiAdaptor::GetIndexInParent()
{
    // TODO
    // Not handling for now. indexInParent can now be calculated just as
    // easily on the client side.
    return 0;
}

QString QSpiAdaptor::GetLocalizedRoleName()
{
    // TODO
    QString out0;
    return out0;
}

QSpiRelationArray QSpiAdaptor::GetRelationSet()
{
    // TODO
    QSpiRelationArray out0;
    return out0;
}

uint QSpiAdaptor::GetRole()
{
    QAccessible::Role role = ACCESSIBLE_INTERFACE.role(0);
    return qSpiRoleMapping[role];
}

QString QSpiAdaptor::GetRoleName()
{
    QString roleName (roles + roles_offsets[GetRole()]);
    return roleName;
}

QSpiIntList QSpiAdaptor::GetState()
{
    QSpiIntList set;
    qspi_stateset_from_qstate (ACCESSIBLE_INTERFACE.state(0), set);
    return set;
}

int QSpiAdaptor::nActions() const
{
    return ACTION_INTERFACE->actionCount ();
}

bool QSpiAdaptor::DoAction(int index)
{
    ACTION_INTERFACE->doAction (index);
    return TRUE;
}

/* AT-SPI Action interface --------------------------------------------------*/
/*---------------------------------------------------------------------------*/

QSpiActionArray QSpiAdaptor::GetActions()
{
    QSpiActionArray index;
    for (int i = 0; i < ACTION_INTERFACE->actionCount(); i++)
    {
        QSpiAction action;
        QStringList keyBindings;

        action.name = ACTION_INTERFACE->name (i);
        action.description = ACTION_INTERFACE->description (i);

        keyBindings = ACTION_INTERFACE->keyBindings (i);

        if (keyBindings.length() > 0)
                action.keyBinding = keyBindings[0];
        else
                action.keyBinding = "";

        index << action;
    }
    return index;
}

QString QSpiAdaptor::GetDescription(int index)
{
    return ACTION_INTERFACE->description (index);
}

QString QSpiAdaptor::GetKeyBinding(int index)
{
    QStringList keyBindings;

    keyBindings = ACTION_INTERFACE->keyBindings (index);
    /* Might as well return the first key binding, what are the other options? */
    if (keyBindings.length() > 0)
        return keyBindings[0];
    else
        return "";
}

QString QSpiAdaptor::GetName(int index)
{
    return ACTION_INTERFACE->name (index);
}

/* AT-SPI Application interface ---------------------------------------------*/
/*---------------------------------------------------------------------------*/

int QSpiAdaptor::id() const
{
    return 0;
}

QString QSpiAdaptor::toolkitName() const
{
    return QLatin1String("Qt");
}

QString QSpiAdaptor::version() const
{
    return QLatin1String(QT_VERSION_STR);
}

QString QSpiAdaptor::GetLocale(uint lctype)
{
    Q_UNUSED (lctype);
    QLocale currentLocale;
    return currentLocale.languageToString (currentLocale.language());
}

/* AT-SPI Component interface -----------------------------------------------*/
/*---------------------------------------------------------------------------*/

static QAccessibleInterface *getWindow (QAccessibleInterface &interface)
{
    QAccessibleInterface *current=NULL, *tmp=NULL;

    interface.navigate (QAccessible::Ancestor, 0, &current);

    while (current &&
           current->role(0) != QAccessible::Window &&
           current->role(0) != QAccessible::Application)
    {
        tmp = NULL;
        current->navigate (QAccessible::Ancestor, 1, &tmp);
        delete current;
        current = tmp;
    }

    if (current)
    {
        return current;
    }
    else
    {
        return NULL;
    }
}

static QRect getRelativeRect (QAccessibleInterface &interface)
{
    QAccessibleInterface *window;
    QRect wr, cr;

    cr = interface.rect (0);

    window = getWindow (interface);
    if (window)
    {
        wr = window->rect (0);

        cr.setX(cr.x() - wr.x());
        cr.setY(cr.x() - wr.y());
        delete window;
    }
    return cr;
}

bool QSpiAdaptor::Contains(int x, int y, uint coord_type)
{
    if (coord_type == 0)
        return ACCESSIBLE_INTERFACE.rect(0).contains(x, y);
    else
        return getRelativeRect(ACCESSIBLE_INTERFACE).contains(x, y);
}

QSpiObjectReference QSpiAdaptor::GetAccessibleAtPoint(int x, int y, uint coord_type)
{
    // TODO Return a null path. This is a silly function, may take some time to work out.
    // It should be done on the client side.
    Q_UNUSED (x);
    Q_UNUSED (y);
    Q_UNUSED (coord_type);
    QSpiObjectReference ref;
    ref.name = QDBusConnection::sessionBus().baseService();
    ref.path = QDBusObjectPath (QSPI_OBJECT_PATH_NULL);
    return ref;
}

double QSpiAdaptor::GetAlpha()
{
    // TODO Find out if the QAccessible iterface needs extending to provide an alpha value.
    return 1.0;
}

QSpiRect QSpiAdaptor::GetExtents(uint coord_type)
{
    QRect rect;
    QSpiRect val;

    if (coord_type == 0)
        rect = ACCESSIBLE_INTERFACE.rect(0);
    else
        rect = getRelativeRect (ACCESSIBLE_INTERFACE);

    val.x = rect.x ();
    val.y = rect.y ();
    val.width = rect.width ();
    val.height = rect.height ();
    return val;
}

uint QSpiAdaptor::GetLayer()
{
    // TODO Find out if QT has any concept of 'Layers'
    return 1; // Corresponds to LAYER_WINDOW.
}

short QSpiAdaptor::GetMDIZOrder()
{
    // TODO Does Qt have any concept of Layers?
    return 0;
}

int QSpiAdaptor::GetPosition(uint coord_type, int &y)
{
    QRect rect;
    if (coord_type == 0)
        rect = ACCESSIBLE_INTERFACE.rect(0);
    else
        rect = getRelativeRect (ACCESSIBLE_INTERFACE);
    y = rect.y ();
    return rect.x ();
}

int QSpiAdaptor::GetSize(int &height)
{
    QRect rect = ACCESSIBLE_INTERFACE.rect(0);
    height = rect.height ();
    return rect.width ();
}

bool QSpiAdaptor::GrabFocus()
{
    // TODO This does not seem to be supported by QAccessibleInterface.
    // THIS COULD CAUSE PROBLEMS
    return false;
}

/* AT-SPI EditableText interface --------------------------------------------*/
/*---------------------------------------------------------------------------*/

void QSpiAdaptor::CopyText(int startPos, int endPos)
{
    return EDITABLE_TEXT_INTERFACE->copyText (startPos, endPos);
}

bool QSpiAdaptor::CutText(int startPos, int endPos)
{
    EDITABLE_TEXT_INTERFACE->cutText (startPos, endPos);
    return TRUE;
}

bool QSpiAdaptor::DeleteText(int startPos, int endPos)
{
    EDITABLE_TEXT_INTERFACE->deleteText (startPos, endPos);
    return TRUE;
}

bool QSpiAdaptor::InsertText(int position, const QString &text, int length)
{
    QString resized (text);
    resized.resize(length);
    EDITABLE_TEXT_INTERFACE->insertText (position, text);
    return TRUE;
}

bool QSpiAdaptor::PasteText(int position)
{
    EDITABLE_TEXT_INTERFACE->pasteText (position);
    return TRUE;
}

bool QSpiAdaptor::SetTextContents(const QString &newContents)
{
    EDITABLE_TEXT_INTERFACE->replaceText (0, TEXT_INTERFACE->characterCount(), newContents);
    return TRUE;
}

/* AT-SPI Table interface ---------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#define MAX_SELECTED_COLUMNS 1000
#define MAX_SELECTED_ROWS    1000

QSpiObjectReference QSpiAdaptor::caption() const
{
    return cache->objectToAccessible (TABLE_INTERFACE->caption()->object())->getReference();
}

int QSpiAdaptor::nColumns() const
{
    return TABLE_INTERFACE->columnCount ();
}

int QSpiAdaptor::nRows() const
{
    return TABLE_INTERFACE->rowCount ();
}

int QSpiAdaptor::nSelectedColumns() const
{
    return TABLE_INTERFACE->selectedColumnCount ();
}

int QSpiAdaptor::nSelectedRows() const
{
    return TABLE_INTERFACE->selectedRowCount ();
}

QSpiObjectReference QSpiAdaptor::summary() const
{
    return cache->objectToAccessible (TABLE_INTERFACE->summary()->object())->getReference();
}

bool QSpiAdaptor::AddColumnSelection(int column)
{
    TABLE_INTERFACE->selectColumn (column);
    return TRUE;
}

bool QSpiAdaptor::AddRowSelection(int row)
{
    TABLE_INTERFACE->selectRow (row);
    return TRUE;
}

QSpiObjectReference QSpiAdaptor::GetAccessibleAt(int row, int column)
{
    return cache->objectToAccessible (TABLE_INTERFACE->accessibleAt(row, column)->object())->getReference();
}

int QSpiAdaptor::GetColumnAtIndex(int index)
{
    return TABLE_INTERFACE->columnIndex (index);
}

QString QSpiAdaptor::GetColumnDescription(int column)
{
    return TABLE_INTERFACE->columnDescription (column);
}

int QSpiAdaptor::GetColumnExtentAt(int row, int column)
{
    return TABLE_INTERFACE->columnSpan (row, column);
}

QSpiObjectReference QSpiAdaptor::GetColumnHeader(int column)
{
    Q_UNUSED (column);
    // TODO There should be a column param in this function right?
    return cache->objectToAccessible (TABLE_INTERFACE->columnHeader()->object())->getReference();
}

int QSpiAdaptor::GetIndexAt(int row, int column)
{
    return TABLE_INTERFACE->childIndex (row, column);
}

int QSpiAdaptor::GetRowAtIndex(int index)
{
    int row, column, rowSpan, columnSpan;
    bool isSelected;

    TABLE_INTERFACE->cellAtIndex (index, &row, &column, &rowSpan, &columnSpan, &isSelected);
    return row;
}

bool QSpiAdaptor::GetRowColumnExtentsAtIndex(int index,
						  int &row,
						  int &col,
						  int &row_extents,
						  int &col_extents,
						  bool &is_selected)
{
    int row0, column, rowSpan, columnSpan;
    bool isSelected;

    TABLE_INTERFACE->cellAtIndex (index, &row0, &column, &rowSpan, &columnSpan, &isSelected);
    row = row0;
    col = column;
    row_extents = rowSpan;
    col_extents = columnSpan;
    is_selected = isSelected;
    if (index < ACCESSIBLE_INTERFACE.childCount())
        return TRUE;
    else
        return FALSE;
}

QString QSpiAdaptor::GetRowDescription(int row)
{
    return TABLE_INTERFACE->rowDescription (row);
}

int QSpiAdaptor::GetRowExtentAt(int row, int column)
{
    return TABLE_INTERFACE->rowSpan (row, column);
}

QSpiObjectReference QSpiAdaptor::GetRowHeader(int row)
{
    Q_UNUSED (row);
    // TODO There should be a row param here right?
    return cache->objectToAccessible (TABLE_INTERFACE->rowHeader()->object())->getReference();
}

QSpiIntList QSpiAdaptor::GetSelectedColumns()
{
    QSpiIntList columns;
    TABLE_INTERFACE->selectedColumns (MAX_SELECTED_COLUMNS, &columns);
    return columns;
}

QSpiIntList QSpiAdaptor::GetSelectedRows()
{
    QSpiIntList rows;
    TABLE_INTERFACE->selectedRows (MAX_SELECTED_ROWS, &rows);
    return rows;
}

bool QSpiAdaptor::IsColumnSelected(int column)
{
    return TABLE_INTERFACE->isColumnSelected (column);
}

bool QSpiAdaptor::IsRowSelected(int row)
{
    return TABLE_INTERFACE->isRowSelected (row);
}

bool QSpiAdaptor::IsSelected(int row, int column)
{
    return TABLE_INTERFACE->isSelected (row, column);
}

bool QSpiAdaptor::RemoveColumnSelection(int column)
{
    TABLE_INTERFACE->unselectColumn (column);
    return TRUE;
}

bool QSpiAdaptor::RemoveRowSelection(int row)
{
    TABLE_INTERFACE->unselectRow (row);
    return TRUE;
}

/* AT-SPI Text interface ----------------------------------------------------*/
/*---------------------------------------------------------------------------*/

int QSpiAdaptor::caretOffset() const
{
    return TEXT_INTERFACE->cursorPosition();
}

int QSpiAdaptor::characterCount() const
{
    return TEXT_INTERFACE->cursorPosition();
}

bool QSpiAdaptor::AddSelection(int startOffset, int endOffset)
{
    int lastSelection = TEXT_INTERFACE->selectionCount ();
    TEXT_INTERFACE->setSelection (lastSelection, startOffset, endOffset);
    return true;
}

QSpiAttributeSet QSpiAdaptor::GetAttributeRun(int offset,
				              bool includeDefaults,
					      int &startOffset,
					      int &endOffset)
{
    Q_UNUSED (includeDefaults);
    return GetAttributes (offset, startOffset, endOffset);
}

QString QSpiAdaptor::GetAttributeValue(int offset,
		  		      const QString &attributeName,
				      int &startOffset,
				      int &endOffset,
				      bool &defined)
{
    int         startOffsetCopy, endOffsetCopy;
    QString     mapped;
    QString     joined;
    QStringList attributes;
    QSpiAttributeSet map;

    endOffsetCopy = endOffset;
    startOffsetCopy = startOffset;
    joined = TEXT_INTERFACE->attributes(offset, &startOffsetCopy, &endOffsetCopy);
    attributes = joined.split (';', QString::SkipEmptyParts, Qt::CaseSensitive);
    foreach (QString attr, attributes)
    {
        QStringList items;
        items = attr.split(':', QString::SkipEmptyParts, Qt::CaseSensitive);
        map[items[0]] = items[1];
    }
    endOffset = endOffsetCopy;
    endOffset = endOffsetCopy;
    mapped = map[attributeName];
    if (mapped == "")
       defined = TRUE;
    else
       defined = FALSE;
    return mapped;
}

QSpiAttributeSet QSpiAdaptor::GetAttributes(int offset, int &startOffset, int &endOffset)
{
    int         startOffsetCopy, endOffsetCopy;
    QString     joined;
    QStringList attributes;
    QSpiAttributeSet set;

    endOffsetCopy = endOffset;
    startOffsetCopy = startOffset;
    joined = TEXT_INTERFACE->attributes(offset, &startOffset, &endOffsetCopy);
    attributes = joined.split (';', QString::SkipEmptyParts, Qt::CaseSensitive);
    foreach (QString attr, attributes)
    {
        QStringList items;
        items = attr.split(':', QString::SkipEmptyParts, Qt::CaseSensitive);
        set[items[0]] = items[1];
    }
    endOffset = endOffsetCopy;
    startOffsetCopy = startOffset;
    return set;
}

QSpiRangeList QSpiAdaptor::GetBoundedRanges(int x,
					        int y,
					        int width,
					        int height,
					        uint coordType,
					        uint xClipType,
					        uint yClipType)
{
    // TODO
    Q_UNUSED (x);
    Q_UNUSED (y);
    Q_UNUSED (width);
    Q_UNUSED (height);
    Q_UNUSED (coordType);
    Q_UNUSED (xClipType);
    Q_UNUSED (yClipType);
    QSpiRangeList out0;
    return out0;
}

int QSpiAdaptor::GetCharacterAtOffset(int offset)
{
    int start=offset, end=offset+1;
    QString result;
    result = TEXT_INTERFACE->textAtOffset(offset, QAccessible2::CharBoundary, &start, &end);
    return *(qPrintable (result));
}

int QSpiAdaptor::GetCharacterExtents(int offset, uint coordType, int &y, int &width, int &height)
{
    // TODO use 'characterRect'
    Q_UNUSED (y);
    Q_UNUSED (width);
    Q_UNUSED (height);
    Q_UNUSED (offset);
    Q_UNUSED (coordType);
    return 0;
}

QSpiAttributeSet QSpiAdaptor::GetDefaultAttributeSet()
{
    // Empty set seems reasonable. There is no default attribute set.
    QSpiAttributeSet attributes;
    return attributes;
}

QSpiAttributeSet QSpiAdaptor::GetDefaultAttributes()
{
    // TODO This function should be removed. It is deprecated in favour of default attribute set.
    QSpiAttributeSet attributes;
    return attributes;
}

int QSpiAdaptor::GetNSelections()
{
    return TEXT_INTERFACE->selectionCount();
}

int QSpiAdaptor::GetOffsetAtPoint(int x, int y, uint coordType)
{
    return TEXT_INTERFACE->offsetAtPoint (QPoint (x, y), static_cast <QAccessible2::CoordinateType> (coordType));
}

int QSpiAdaptor::GetRangeExtents(int startOffset, int endOffset, uint coordType, int &y, int &width, int &height)
{
    // TODO
    Q_UNUSED (y);
    Q_UNUSED (width);
    Q_UNUSED (height);
    Q_UNUSED (coordType);
    Q_UNUSED (startOffset);
    Q_UNUSED (endOffset);
    return 0;
}

int QSpiAdaptor::GetSelection(int selectionNum, int &endOffset)
{
    int start, end;
    TEXT_INTERFACE->selection (selectionNum, &start, &end);
    endOffset = end;
    return start;
}

QString QSpiAdaptor::GetText(int startOffset, int endOffset)
{
    return TEXT_INTERFACE->text (startOffset, endOffset);
}

QString QSpiAdaptor::GetTextAfterOffset(int offset, uint type, int &startOffset, int &endOffset)
{
    // TODO
    Q_UNUSED (type);
    Q_UNUSED (startOffset);
    Q_UNUSED (offset)
    Q_UNUSED (endOffset);
    return 0;
}

QString QSpiAdaptor::GetTextAtOffset(int offset, uint type, int &startOffset, int &endOffset)
{
    // TODO
    Q_UNUSED (type);
    Q_UNUSED (startOffset);
    Q_UNUSED (offset)
    Q_UNUSED (endOffset);
    return 0;
}

QString QSpiAdaptor::GetTextBeforeOffset(int offset, uint type, int &startOffset, int &endOffset)
{
    // TODO
    Q_UNUSED (type);
    Q_UNUSED (startOffset);
    Q_UNUSED (offset)
    Q_UNUSED (endOffset);
    return 0;
}

bool QSpiAdaptor::RemoveSelection(int selectionNum)
{
    // TODO
    bool out0;
    return out0;
}

bool QSpiAdaptor::SetCaretOffset(int offset)
{
    // TODO
    bool out0;
    return out0;
}

bool QSpiAdaptor::SetSelection(int selectionNum, int startOffset, int endOffset)
{
    // TODO
    bool out0;
    return out0;
}

/* AT-SPI Value interface ---------------------------------------------------*/
/*---------------------------------------------------------------------------*/

double QSpiAdaptor::currentValue() const
{
    double val;
    bool success;
    val = VALUE_INTERFACE->currentValue().toDouble (&success);
    if (success)
    {
        return val;
    }
    else
    {
        qDebug ("QSpiAccessibleBridge: Could not convert current value to double");
        return 0.0;
    }
}

void QSpiAdaptor::setCurrentValue(double value)
{
    VALUE_INTERFACE->setCurrentValue(QVariant (value));
}

double QSpiAdaptor::maximumValue() const
{
    double val;
    bool success;
    val = VALUE_INTERFACE->maximumValue().toDouble (&success);
    if (success)
    {
        return val;
    }
    else
    {
        qDebug ("QSpiAccessibleBridge: Could not convert maximum value to double");
        return 0.0;
    }
}

double QSpiAdaptor::minimumIncrement() const
{
    return 0.0;
}

double QSpiAdaptor::minimumValue() const
{
    double val;
    bool success;
    val = VALUE_INTERFACE->minimumValue().toDouble (&success);
    if (success)
    {
        return val;
    }
    else
    {
        qDebug ("QSpiAccessibleBridge: Could not convert minimum value to double");
        return 0.0;
    }
}

/*END------------------------------------------------------------------------*/
