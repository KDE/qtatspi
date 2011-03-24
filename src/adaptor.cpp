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

#include <QtCore/QtDebug>
#include <QtGui/QApplication>
#include <QtGui/QWidget>

#include "adaptor.h"
#include "bridge.h"
#include "constant_mappings.h"


QSpiAdaptor::QSpiAdaptor(QAccessibleInterface *interface_, int child)
    : interface(interface_), child(child)
{
}

QSpiObjectReference QSpiAdaptor::getReference() const
{
    return reference;
}

QStringList QSpiAdaptor::getSupportedInterfaces() const
{
    return supportedInterfaces;
}

QSpiAccessibleCacheItem QSpiAdaptor::getCacheItem() const
{
    Q_ASSERT(interface);
    if (!interface->isValid()) {
        qWarning() << "QSpiObject::getCacheItem: invalid interface" << reference.path.path();
        return QSpiAccessibleCacheItem();
    }

    QSpiAccessibleCacheItem item;
    item.path = getReference();
    item.parent = getParentReference();
    item.application = spiBridge->getRootReference();

    /* Children */
    QList<QSpiObjectReference> childPaths;
    for (int i = 1; i <= interface->childCount(); i++) {
        QSpiAdaptor* child = getChild(i);
        if (child)
            childPaths << child->getReference();
    }
    item.children = childPaths;

    item.supportedInterfaces = getSupportedInterfaces();
    item.name = interface->text(QAccessible::Name, child);
    item.role = qSpiRoleMapping.value(interface->role(child)).spiRole();
    item.description = interface->text(QAccessible::Description, child);
    item.states = qSpiStatesetFromQState(interface->state(child));
    return item;
}

void QSpiAdaptor::signalChildrenChanged(const QString &type, int detail1, int detail2, const QDBusVariant &data)
{
    emit ChildrenChanged(type, detail1, detail2, data, spiBridge->getRootReference());
}

int QSpiAdaptor::childCount() const
{
    if (child)
        return 0;
    return interface->childCount();
}

QString QSpiAdaptor::description() const
{
    return interface->text(QAccessible::Description, child);
}

QString QSpiAdaptor::name() const
{
    Q_ASSERT(interface->isValid());
    return interface->text(QAccessible::Name, child);
}

QSpiObjectReference QSpiAdaptor::parent() const
{
    return getParentReference();
}

QSpiObjectReference QSpiAdaptor::GetApplication()
{
    return spiBridge->getRootReference();
}

QSpiAttributeSet QSpiAdaptor::GetAttributes()
{
    // No attributes interface in QAccessible so a blank list seems the sensible option.
    QSpiAttributeSet out0;
    return out0;
}

QSpiObjectReference QSpiAdaptor::GetChildAtIndex(int index)
{
    // if we are the child of a complex widget, we cannot have any children
    Q_ASSERT(child == 0);
    Q_ASSERT(index < interface->childCount());

    qDebug() << "QSpiAdaptor::GetChildAtIndex get child " << index << " of " << interface->childCount()
             << interface->text(QAccessible::Name, 0) << interface->object();

    if (interface->text(QAccessible::Name, 0) == "Fruits") {
        qDebug() << "fruit salad" << interface->object();
    }

    QSpiAdaptor* child = getChild(index+1);
//    Q_ASSERT(child);
    if (!child) {
        qWarning() << "QSpiAdaptor::GetChildAtIndex could not find child!";
        return QSpiObjectReference();
    }

    return child->getReference();
}

QSpiObjectReferenceArray QSpiAdaptor::GetChildren()
{
    // if we are the child of a complex widget, we cannot have any children
    Q_ASSERT(child == 0);

    QList<QSpiObjectReference> children;

    for (int i = 1; i <= interface->childCount(); ++i) {
        QSpiAdaptor* child = getChild(i);
        if (child)
            children << child->getReference();
    }
    return children;
}

QSpiAdaptor* QSpiAdaptor::getChild(int index) const
{
    Q_ASSERT(index > 0 && index <= interface->childCount());
    QAccessibleInterface *child = 0;
    int ret = interface->navigate(QAccessible::Child, index, &child);
    if (ret == 0) {
        return spiBridge->interfaceToAccessible(child, 0, true);
    } else if (ret > 0){
        Q_ASSERT(ret <= interface->childCount());
        return spiBridge->interfaceToAccessible(interface, ret, true);
    }
    qWarning() << "QSpiAdaptor::getChild INVALID CHILD: " << interface->object() << index;
    return 0;
}

int QSpiAdaptor::GetIndexInParent()
{
//    qDebug() << "QSpiAdaptor::GetIndexInParent" << interface->text(QAccessible::Name, 0);
//    qDebug() << "  obj: " << interface->object();
    if (child)
        return child;

    QAccessibleInterface* parent;
    interface->navigate(QAccessible::Ancestor, 1, &parent);
    if (parent) {
        qDebug() << "QSpiAdaptor::GetIndexInParent" << parent->text(QAccessible::Name, 0);
        int index = parent->indexOfChild(interface);
        qDebug() << "Index: " << index;
        return index;
    }
    return -1;
}

QString QSpiAdaptor::GetLocalizedRoleName()
{
    QString out0;
    out0 = qSpiRoleMapping.value(interface->role(child)).localizedName();
    return out0;
}

QSpiRelationArray QSpiAdaptor::GetRelationSet()
{
    qWarning("Not implemented: QSpiAdaptor::GetRelationSet");
    QSpiRelationArray out0;

    return out0;
}

uint QSpiAdaptor::GetRole()
{
    QAccessible::Role role = interface->role(child);
    return qSpiRoleMapping[role].spiRole();
}

QString QSpiAdaptor::GetRoleName()
{
    return qSpiRoleMapping[interface->role(child)].name();
}

QSpiUIntList QSpiAdaptor::GetState()
{
    return qSpiStatesetFromQState(interface->state(child));
}

int QSpiAdaptor::nActions() const
{
    return interface->actionInterface()->actionCount();
}

bool QSpiAdaptor::DoAction(int index)
{
    interface->actionInterface()->doAction(index);
    return TRUE;
}

/* AT-SPI Action interface --------------------------------------------------*/
/*---------------------------------------------------------------------------*/

QSpiActionArray QSpiAdaptor::GetActions()
{
    QSpiActionArray index;
    for (int i = 0; i < interface->actionInterface()->actionCount(); i++)
    {
        QSpiAction action;
        QStringList keyBindings;

        action.name = interface->actionInterface()->name(i);
        action.description = interface->actionInterface()->description(i);

        keyBindings = interface->actionInterface()->keyBindings(i);

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
    return interface->actionInterface()->description(index);
}

QString QSpiAdaptor::GetKeyBinding(int index)
{
    QStringList keyBindings;

    keyBindings = interface->actionInterface()->keyBindings(index);
    /* Might as well return the first key binding, what are the other options? */
    if (keyBindings.length() > 0)
        return keyBindings[0];
    else
        return "";
}

QString QSpiAdaptor::GetName(int index)
{
    return interface->actionInterface()->name(index);
}

/* AT-SPI Application interface ---------------------------------------------*/
/*---------------------------------------------------------------------------*/

int QSpiAdaptor::id() const
{
    return property("Id").toInt();
}

QString QSpiAdaptor::toolkitName() const
{
    qWarning() << "QSpiAdaptor::toolkitName FIXME: We pretend to be GAIL as toolkit. This is evil and needs fixing.";
//    return QLatin1String("Qt");
    return QLatin1String("GAIL");
}

QString QSpiAdaptor::version() const
{
    return QLatin1String(QT_VERSION_STR);
}

QString QSpiAdaptor::GetLocale(uint lctype)
{
    Q_UNUSED(lctype)
    QLocale currentLocale;
    return currentLocale.languageToString(currentLocale.language());
}

/* AT-SPI Component interface -----------------------------------------------*/
/*---------------------------------------------------------------------------*/

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

bool QSpiAdaptor::Contains(int x, int y, uint coord_type)
{
    if (coord_type == 0)
        return interface->rect(child).contains(x, y);
    else
        return getRelativeRect(interface, child).contains(x, y);
}

QSpiObjectReference QSpiAdaptor::GetAccessibleAtPoint(int x, int y, uint coord_type)
{
    Q_UNUSED (coord_type)

    QWidget* w = qApp->widgetAt(x,y);
    if (w) {
        return spiBridge->objectToAccessible(w)->getReference();
    } else {
        QSpiObjectReference ref;
        ref.name = QDBusConnection::sessionBus().baseService();
        ref.path = QDBusObjectPath(QSPI_OBJECT_PATH_NULL);
        return ref;
    }
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
        rect = interface->rect(child);
    else
        rect = getRelativeRect(interface, child);

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
        rect = interface->rect(child);
    else
        rect = getRelativeRect(interface, child);
    y = rect.y ();
    return rect.x ();
}

int QSpiAdaptor::GetSize(int &height)
{
    QRect rect = interface->rect(child);
    height = rect.height();
    return rect.width();
}

bool QSpiAdaptor::GrabFocus()
{
    // TODO This does not seem to be supported by QAccessibleInterface.
    // FIXME: raise the window to make it active also?
    // FIXME: graphics/qml items

    if (interface->object()->isWidgetType()) {
        QWidget* w = static_cast<QWidget*>(interface->object());
        w->setFocus(Qt::OtherFocusReason);
        return true;
    }

    return false;
}

/* AT-SPI EditableText interface --------------------------------------------*/
/*---------------------------------------------------------------------------*/

void QSpiAdaptor::CopyText(int startPos, int endPos)
{
    return interface->editableTextInterface()->copyText(startPos, endPos);
}

bool QSpiAdaptor::CutText(int startPos, int endPos)
{
    interface->editableTextInterface()->cutText(startPos, endPos);
    return TRUE;
}

bool QSpiAdaptor::DeleteText(int startPos, int endPos)
{
    interface->editableTextInterface()->deleteText(startPos, endPos);
    return TRUE;
}

bool QSpiAdaptor::InsertText(int position, const QString &text, int length)
{
    QString resized (text);
    resized.resize(length);
    interface->editableTextInterface()->insertText(position, text);
    return TRUE;
}

bool QSpiAdaptor::PasteText(int position)
{
    interface->editableTextInterface()->pasteText(position);
    return TRUE;
}

bool QSpiAdaptor::SetTextContents(const QString &newContents)
{
    interface->editableTextInterface()->replaceText(0, interface->textInterface()->characterCount(), newContents);
    return TRUE;
}

/* AT-SPI Table interface ---------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#define MAX_SELECTED_COLUMNS 1000
#define MAX_SELECTED_ROWS    1000

QSpiObjectReference QSpiAdaptor::caption() const
{
    return spiBridge->objectToAccessible (interface->tableInterface()->caption()->object())->getReference();
}

int QSpiAdaptor::nColumns() const
{
    return interface->tableInterface()->columnCount ();
}

int QSpiAdaptor::nRows() const
{
    return interface->tableInterface()->rowCount ();
}

int QSpiAdaptor::nSelectedColumns() const
{
    return interface->tableInterface()->selectedColumnCount ();
}

int QSpiAdaptor::nSelectedRows() const
{
    return interface->tableInterface()->selectedRowCount ();
}

QSpiObjectReference QSpiAdaptor::summary() const
{
    return spiBridge->objectToAccessible (interface->tableInterface()->summary()->object())->getReference();
}

bool QSpiAdaptor::AddColumnSelection(int column)
{
    interface->tableInterface()->selectColumn(column);
    return TRUE;
}

bool QSpiAdaptor::AddRowSelection(int row)
{
    interface->tableInterface()->selectRow(row);
    return TRUE;
}

QSpiObjectReference QSpiAdaptor::GetAccessibleAt(int row, int column)
{
    Q_ASSERT(interface->tableInterface());
    return spiBridge->objectToAccessible(interface->tableInterface()->accessibleAt(row, column)->object())->getReference();
}

int QSpiAdaptor::GetColumnAtIndex(int index)
{
    return interface->tableInterface()->columnIndex(index);
}

QString QSpiAdaptor::GetColumnDescription(int column)
{
    return interface->tableInterface()->columnDescription(column);
}

int QSpiAdaptor::GetColumnExtentAt(int row, int column)
{
    return interface->tableInterface()->columnSpan(row, column);
}

QSpiObjectReference QSpiAdaptor::GetColumnHeader(int column)
{
    Q_UNUSED (column);
    // TODO There should be a column param in this function right?
    return spiBridge->objectToAccessible(interface->tableInterface()->columnHeader()->object())->getReference();
}

int QSpiAdaptor::GetIndexAt(int row, int column)
{
    return interface->tableInterface()->childIndex(row, column);
}

int QSpiAdaptor::GetRowAtIndex(int index)
{
    int row, column, rowSpan, columnSpan;
    bool isSelected;

    interface->tableInterface()->cellAtIndex (index, &row, &column, &rowSpan, &columnSpan, &isSelected);
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

    interface->tableInterface()->cellAtIndex (index, &row0, &column, &rowSpan, &columnSpan, &isSelected);
    row = row0;
    col = column;
    row_extents = rowSpan;
    col_extents = columnSpan;
    is_selected = isSelected;
    if (index < interface->childCount())
        return TRUE;
    else
        return FALSE;
}

QString QSpiAdaptor::GetRowDescription(int row)
{
    return interface->tableInterface()->rowDescription (row);
}

int QSpiAdaptor::GetRowExtentAt(int row, int column)
{
    return interface->tableInterface()->rowSpan (row, column);
}

QSpiObjectReference QSpiAdaptor::GetRowHeader(int row)
{
    Q_UNUSED (row);
    qWarning() << "Implement: QSpiAdaptor::GetRowHeader";
    return QSpiObjectReference();

    // TODO There should be a row param here right?
//    return spiBridge->objectToAccessible(interface->tableInterface()->rowHeader()->object())->getReference();
}

QSpiIntList QSpiAdaptor::GetSelectedColumns()
{
    QSpiIntList columns;
    interface->tableInterface()->selectedColumns(MAX_SELECTED_COLUMNS, &columns);
    return columns;
}

QSpiIntList QSpiAdaptor::GetSelectedRows()
{
    QSpiIntList rows;
    interface->tableInterface()->selectedRows(MAX_SELECTED_ROWS, &rows);
    return rows;
}

bool QSpiAdaptor::IsColumnSelected(int column)
{
    return interface->tableInterface()->isColumnSelected (column);
}

bool QSpiAdaptor::IsRowSelected(int row)
{
    return interface->tableInterface()->isRowSelected (row);
}

bool QSpiAdaptor::IsSelected(int row, int column)
{
    return interface->tableInterface()->isSelected (row, column);
}

bool QSpiAdaptor::RemoveColumnSelection(int column)
{
    interface->tableInterface()->unselectColumn (column);
    return TRUE;
}

bool QSpiAdaptor::RemoveRowSelection(int row)
{
    interface->tableInterface()->unselectRow (row);
    return TRUE;
}

/* AT-SPI Text interface ----------------------------------------------------*/
/*---------------------------------------------------------------------------*/

int QSpiAdaptor::caretOffset() const
{
    return interface->textInterface()->cursorPosition();
}

int QSpiAdaptor::characterCount() const
{
    return interface->textInterface()->cursorPosition();
}

bool QSpiAdaptor::AddSelection(int startOffset, int endOffset)
{
    int lastSelection = interface->textInterface()->selectionCount ();
    interface->textInterface()->setSelection (lastSelection, startOffset, endOffset);
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
    joined = interface->textInterface()->attributes(offset, &startOffsetCopy, &endOffsetCopy);
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
    joined = interface->textInterface()->attributes(offset, &startOffset, &endOffsetCopy);
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
    qWarning("Not implemented: QSpiAdaptor::GetBoundedRanges");
    Q_UNUSED(x) Q_UNUSED (y) Q_UNUSED(width)
    Q_UNUSED(height) Q_UNUSED(coordType)
    Q_UNUSED(xClipType) Q_UNUSED(yClipType)
    QSpiRangeList out0;
    return out0;
}

int QSpiAdaptor::GetCharacterAtOffset(int offset)
{
    int start=offset, end=offset+1;
    QString result;
    result = interface->textInterface()->textAtOffset(offset, QAccessible2::CharBoundary, &start, &end);
    return *(qPrintable (result));
}

int QSpiAdaptor::GetCharacterExtents(int offset, uint coordType, int &y, int &width, int &height)
{
    int x;

    // QAccessible2 has RelativeToParent as a coordinate type instead of relative
    // to top-level window, which is an AT-SPI coordinate type.
    if (static_cast<QAccessible2::CoordinateType>(coordType) != QAccessible2::RelativeToScreen) {
        const QWidget *widget = qobject_cast<const QWidget*>(interface->object());
        if (!widget) {
            y = 0;
            width = 0;
            height = 0;
            return 0;
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
    y += rect.y();
    return x+rect.x();
}

QSpiAttributeSet QSpiAdaptor::GetDefaultAttributeSet()
{
    // Empty set seems reasonable. There is no default attribute set.
    QSpiAttributeSet attributes;
    return attributes;
}

QSpiAttributeSet QSpiAdaptor::GetDefaultAttributes()
{
    // Deprecated in favour of default attribute set.
    return GetDefaultAttributeSet();
}

int QSpiAdaptor::GetNSelections()
{
    return interface->textInterface()->selectionCount();
}

int QSpiAdaptor::GetOffsetAtPoint(int x, int y, uint coordType)
{
    return interface->textInterface()->offsetAtPoint (QPoint (x, y), static_cast <QAccessible2::CoordinateType> (coordType));
}

int QSpiAdaptor::GetRangeExtents(int startOffset, int endOffset, uint coordType, int &y, int &width, int &height)
{
    if (endOffset == -1)
        endOffset = interface->textInterface()->characterCount();

    if (endOffset <= startOffset) {
        y=0;
        width = 0;
        height = 0;
        return 0;
    }

    int xOffset = 0, yOffset = 0;
    QAccessibleTextInterface *textInterface = interface->textInterface();

    // QAccessible2 has RelativeToParent as a coordinate type instead of relative
    // to top-level window, which is an AT-SPI coordinate type.
    if (static_cast<QAccessible2::CoordinateType>(coordType) != QAccessible2::RelativeToScreen) {
        const QWidget *widget = qobject_cast<const QWidget*>(interface->object());
        if (!widget) {
            y = 0;
            width = 0;
            height = 0;
            return 0;
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
    return minX + xOffset;
}

int QSpiAdaptor::GetSelection(int selectionNum, int &endOffset)
{
    int start, end;
    interface->textInterface()->selection (selectionNum, &start, &end);
    endOffset = end;
    return start;
}

QString QSpiAdaptor::GetText(int startOffset, int endOffset)
{
    if (endOffset == -1)
        endOffset = interface->textInterface()->characterCount();
    return interface->textInterface()->text(startOffset, endOffset);
}

QString QSpiAdaptor::GetTextAfterOffset(int offset, uint type, int &startOffset, int &endOffset)
{
    // FIXME find out if IA2 types are the same as the ones in at-spi
    return interface->textInterface()->textAfterOffset(offset, (QAccessible2::BoundaryType)type, &startOffset, &endOffset);
}

QString QSpiAdaptor::GetTextAtOffset(int offset, uint type, int &startOffset, int &endOffset)
{
    // FIXME find out if IA2 types are the same as the ones in at-spi
    QAccessibleTextInterface * t = interface->textInterface();
    QString text = t->textAtOffset(offset, (QAccessible2::BoundaryType)type, &startOffset, &endOffset);
    return text;
}

QString QSpiAdaptor::GetTextBeforeOffset(int offset, uint type, int &startOffset, int &endOffset)
{
    // FIXME find out if IA2 types are the same as the ones in at-spi
    return interface->textInterface()->textBeforeOffset(offset, (QAccessible2::BoundaryType)type, &startOffset, &endOffset);
}

bool QSpiAdaptor::RemoveSelection(int selectionNum)
{
    interface->textInterface()->removeSelection(selectionNum);
    return true;
}

bool QSpiAdaptor::SetCaretOffset(int offset)
{
    interface->textInterface()->setCursorPosition(offset);
    return true;
}

bool QSpiAdaptor::SetSelection(int selectionNum, int startOffset, int endOffset)
{
    interface->textInterface()->setSelection(selectionNum, startOffset, endOffset);
    return true;
}

/* AT-SPI Value interface ---------------------------------------------------*/
/*---------------------------------------------------------------------------*/

double QSpiAdaptor::currentValue() const
{
    double val;
    bool success;
    val = interface->valueInterface()->currentValue().toDouble (&success);
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
    interface->valueInterface()->setCurrentValue(QVariant (value));
}

double QSpiAdaptor::maximumValue() const
{
    double val;
    bool success;
    val = interface->valueInterface()->maximumValue().toDouble (&success);
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
    // FIXME: should be in value interface
    return 0.0;
}

double QSpiAdaptor::minimumValue() const
{
    double val;
    bool success;
    val = interface->valueInterface()->minimumValue().toDouble (&success);
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
