/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2009-2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "accessible.h"
#include "adaptor.h"
#include "bridge.h"
#include "constant_mappings.h"


QSpiAdaptor::QSpiAdaptor(QAccessibleInterface *interface_, int child)
    : interface(interface_), child(child)
{
}

QObject* QSpiAdaptor::getObject() const
{
    return interface->object();
}

QSpiObjectReference QSpiAdaptor::getReference() const
{
    return reference;
}

QStringList QSpiAdaptor::getSupportedInterfaces() const
{
    return supportedInterfaces;
}

bool QSpiAdaptor::checkInterface() const
{
    // The interface can be deleted behind our back with no notification.
    if (!interface->isValid()) {
        spiBridge->removeAdaptor(const_cast<QSpiAdaptor*>(this));
        return false;
    }
    return true;
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
    item.children = GetChildren();
    item.supportedInterfaces = getSupportedInterfaces();
    item.name = interface->text(QAccessible::Name, child);
    item.role = qSpiRoleMapping.value(interface->role(child)).spiRole();
    item.description = interface->text(QAccessible::Description, child);
    item.state = GetState();
    return item;
}

void QSpiAdaptor::signalChildrenChanged(const QString &type, int detail1, int detail2, const QDBusVariant &data)
{
    emit ChildrenChanged(type, detail1, detail2, data, spiBridge->getRootReference());
}

int QSpiAdaptor::childCount() const
{
    if (!checkInterface()) return 0;

    if (child)
        return 0;
    return interface->childCount();
}

QString QSpiAdaptor::description() const
{
    if (!checkInterface()) return QString();
    return interface->text(QAccessible::Description, child);
}

QString QSpiAdaptor::name() const
{
    if (!checkInterface()) return QString();
    QString name = interface->text(QAccessible::Name, child);
    if (!name.isEmpty()) {
        return name;
    }
    return interface->text(QAccessible::Value, child);
}

QSpiObjectReference QSpiAdaptor::parent() const
{
    if (!checkInterface()) return QSpiObjectReference();
    return getParentReference();
}

QSpiObjectReference QSpiAdaptor::GetApplication() const
{
    if (!checkInterface()) return QSpiObjectReference();
    return spiBridge->getRootReference();
}

QSpiAttributeSet QSpiAdaptor::GetAttributes() const
{
    if (!checkInterface()) return QSpiAttributeSet();
    // No attributes interface in QAccessible so a blank list seems the sensible option.
    QSpiAttributeSet out0;
    return out0;
}

QSpiObjectReference QSpiAdaptor::GetChildAtIndex(int index) const
{
    if (!checkInterface()) return QSpiObjectReference();
    // if we are the child of a complex widget, we cannot have any children
    Q_ASSERT(child == 0);
    Q_ASSERT(index < interface->childCount());
    QSpiAdaptor* child = getChild(index+1);
    if (!child) {
        qWarning() << "WARNING:QSpiAdaptor::GetChildAtIndex no child: " << index << getReference().path.path() << interface->object();
        return QSpiObjectReference();
    }
    return child->getReference();
}

QSpiObjectReferenceArray QSpiAdaptor::GetChildren() const
{
    QList<QSpiObjectReference> children;
    if (!checkInterface()) return children;

    // TODO: become independent of caching the interfaces...
    //    QPair<QAccessibleInterface*, int> pair = QSpiAccessible::interfaceFromPath(getReference().path.path());
//    qDebug() << "CHILDREN: " << getReference().path.path();

    // when we are a child that means that we cannot have children of our own
    if (child) {
        QAccessibleInterface *iface;
        interface->navigate(QAccessible::Ancestor, 1, &iface);
        qWarning() << "WARNING:QSpiAdaptor::GetChildren Request child of virtual child: "
                   << getReference().path.path() << interface->object() << "child: " << childIndex() << " parent: " << iface->object();
        delete iface;
        return children;
    }

    for (int i = 1; i <= interface->childCount(); ++i) {
        QSpiAdaptor* child = getChild(i);
        if (child)
            children << child->getReference();

//        // use navigate and return refernces
//        QAccessibleInterface* childInterface;
//        int childIndex = interface->navigate(QAccessible::Child, i, &childInterface);
//        QString path;
//        if (childIndex) {
//            path = QSpiAccessible::pathForInterface(interface, childIndex);
//        } else {
//            Q_ASSERT(childInterface);
//            path = QSpiAccessible::pathForInterface(childInterface, childIndex);
//        }
//        qDebug() << "   CHILD: " << path;
//        children.append(QSpiObjectReference(spiBridge->dBusConnection(), QDBusObjectPath(path)));
    }

    Q_ASSERT(interface->childCount() == children.count());

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

int QSpiAdaptor::GetIndexInParent() const
{
    if (!checkInterface()) return -1;

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

QString QSpiAdaptor::GetLocalizedRoleName() const
{
    if (!checkInterface()) return QString();

    QString out0;
    out0 = qSpiRoleMapping.value(interface->role(child)).localizedName();
    return out0;
}

QSpiRelationArray QSpiAdaptor::GetRelationSet() const
{
    if (!checkInterface()) return QSpiRelationArray();

    const QAccessible::RelationFlag relationsToCheck[] = {QAccessible::Label, QAccessible::Labelled, QAccessible::Controller, QAccessible::Controlled, static_cast<QAccessible::RelationFlag>(-1)};
    const AtspiRelationType relationTypes[] = {ATSPI_RELATION_LABELLED_BY, ATSPI_RELATION_LABEL_FOR, ATSPI_RELATION_CONTROLLED_BY, ATSPI_RELATION_CONTROLLER_FOR};

    QSpiRelationArray relations;
    
    QAccessibleInterface *target;

    for (int i = 0; relationsToCheck[i] >= 0; i++) {
        QList<QSpiObjectReference> related;
        int navigateResult = 1;

        for (int j = 1; navigateResult >= 0; j++) {
            navigateResult = interface->navigate(relationsToCheck[i], j, &target);

            if (navigateResult == 0) {
                QSpiAdaptor *targetAdaptor = spiBridge->interfaceToAccessible(target, 0, false);
                related.append(targetAdaptor->getReference());
                delete target;
            } else if (navigateResult > 0) {
                //Then it's a child of the object
                related.append(this->GetChildAtIndex(navigateResult));
            }
        }
        if (!related.isEmpty())
            relations.append(QSpiRelationArrayEntry(relationTypes[i], related));
    }

    return relations;
}

uint QSpiAdaptor::GetRole() const
{
    if (!checkInterface()) return QAccessible::NoRole;

    QAccessible::Role role = interface->role(child);
    return qSpiRoleMapping[role].spiRole();
}

QString QSpiAdaptor::GetRoleName() const
{
    if (!checkInterface()) return QString();

    return qSpiRoleMapping[interface->role(child)].name();
}

QSpiUIntList QSpiAdaptor::GetState() const
{
    if (!checkInterface()) return QSpiUIntList();

    quint64 spiState = spiStatesFromQState(interface->state(child));
    if (interface->table2Interface()) {
        setSpiStateBit(&spiState, ATSPI_STATE_MANAGES_DESCENDANTS);
    }
    return spiStateSetFromSpiStates(spiState);
}

int QSpiAdaptor::nActions() const
{
    if (!checkInterface()) return 0;

    return interface->actionInterface()->actionCount();
}

bool QSpiAdaptor::DoAction(int index)
{
    if (!checkInterface()) return false;

    interface->actionInterface()->doAction(index);
    return TRUE;
}

/* AT-SPI Action interface --------------------------------------------------*/
/*---------------------------------------------------------------------------*/

QSpiActionArray QSpiAdaptor::GetActions()
{
    QSpiActionArray index;
    if (!checkInterface()) return index;

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
    if (!checkInterface()) return QString();

    return interface->actionInterface()->description(index);
}

QString QSpiAdaptor::GetKeyBinding(int index)
{
    if (!checkInterface()) return QString();
    QStringList keyBindings;

    keyBindings = interface->actionInterface()->keyBindings(index);
    /* Might as well return the first key binding, what are the other options? */
    if (keyBindings.length() > 0)
        return keyBindings[0];
    else
        return QString();
}

QString QSpiAdaptor::GetName(int index)
{
    if (!checkInterface()) return QString();
    return interface->actionInterface()->name(index);
}

/* AT-SPI Application interface ---------------------------------------------*/
/*---------------------------------------------------------------------------*/

int QSpiAdaptor::id() const
{
    if (!checkInterface()) return -1;
    return property("Id").toInt();
}

QString QSpiAdaptor::toolkitName() const
{
    if (!checkInterface()) return QString();
//    qWarning() << "QSpiAdaptor::toolkitName FIXME: We pretend to be GAIL as toolkit. This is evil and needs fixing.";
    return QLatin1String("Qt");
//    return QLatin1String("GAIL");
}

QString QSpiAdaptor::version() const
{
    if (!checkInterface()) return QString();
    return QLatin1String(QT_VERSION_STR);
}

/// The bus address for direct (p2p) connections.
/// Not supported atm.
QString QSpiAdaptor::GetApplicationBusAddress() const
{
    qDebug() << "QSpiAdaptor::GetApplicationBusAddress implement me!";
    return QString();
}

QString QSpiAdaptor::GetLocale(uint lctype)
{
    if (!checkInterface()) return QString();
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
    if (!checkInterface()) return false;
    if (coord_type == 0)
        return interface->rect(child).contains(x, y);
    else
        return getRelativeRect(interface, child).contains(x, y);
}

QSpiObjectReference QSpiAdaptor::GetAccessibleAtPoint(int x, int y, uint coord_type)
{
    if (!checkInterface()) return QSpiObjectReference();
    Q_UNUSED (coord_type)

    // Grab the top level widget. For complex widgets we want to return a child
    // at the right position instead.
    QWidget* w = qApp->widgetAt(x,y);
    if (w) {
        QSpiAdaptor* adaptor = spiBridge->objectToAccessible(w);

        int i = adaptor->associatedInterface()->childAt(x, y);
        if (i > 0) {
            QSpiAdaptor* child = adaptor->getChild(i);
            return child->getReference();
        }
        return adaptor->getReference();
    } else {
        return QSpiObjectReference(spiBridge->dBusConnection(), QDBusObjectPath(QSPI_OBJECT_PATH_NULL));
    }
}

double QSpiAdaptor::GetAlpha()
{
    if (!checkInterface()) return 0.0;
    // TODO Find out if the QAccessible iterface needs extending to provide an alpha value.
    return 1.0;
}

QSpiRect QSpiAdaptor::GetExtents(uint coord_type)
{
    QSpiRect val;
    if (!checkInterface()) return val;

    QRect rect;
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
    if (!checkInterface()) return 0;
    // TODO Find out if QT has any concept of 'Layers'
    return 1; // Corresponds to LAYER_WINDOW.
}

short QSpiAdaptor::GetMDIZOrder()
{
    if (!checkInterface()) return 0;
    // TODO Does Qt have any concept of Layers?
    return 0;
}

int QSpiAdaptor::GetPosition(uint coord_type, int &y)
{
    if (!checkInterface()) return 0;
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
    if (!checkInterface()) return 0;
    QRect rect = interface->rect(child);
    height = rect.height();
    return rect.width();
}

bool QSpiAdaptor::GrabFocus()
{
    if (!checkInterface()) return false;
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
    if (!checkInterface()) return;
    interface->editableTextInterface()->copyText(startPos, endPos);
}

bool QSpiAdaptor::CutText(int startPos, int endPos)
{
    if (!checkInterface()) return false;
    interface->editableTextInterface()->cutText(startPos, endPos);
    return TRUE;
}

bool QSpiAdaptor::DeleteText(int startPos, int endPos)
{
    if (!checkInterface()) return false;
    interface->editableTextInterface()->deleteText(startPos, endPos);
    return TRUE;
}

bool QSpiAdaptor::InsertText(int position, const QString &text, int length)
{
    if (!checkInterface()) return false;
    ;
    return TRUE;
}

bool QSpiAdaptor::PasteText(int position)
{
    if (!checkInterface()) return false;
    interface->editableTextInterface()->pasteText(position);
    return TRUE;
}

bool QSpiAdaptor::SetTextContents(const QString &newContents)
{
    if (!checkInterface()) return false;
    interface->editableTextInterface()->replaceText(0, interface->textInterface()->characterCount(), newContents);
    return TRUE;
}

/* AT-SPI Table interface ---------------------------------------------------*/
/*---------------------------------------------------------------------------*/

QSpiObjectReference QSpiAdaptor::caption() const
{
    if (!checkInterface()) return QSpiObjectReference();

    QAccessibleInterface* captionObject = interface->table2Interface()->caption();
    if (captionObject)
        return spiBridge->interfaceToAccessible(captionObject, 0, true)->getReference();

    return QSpiObjectReference();
}

int QSpiAdaptor::nColumns() const
{
    if (!checkInterface()) return 0;
    return interface->table2Interface()->columnCount();
}

int QSpiAdaptor::nRows() const
{
    if (!checkInterface()) return 0;
    return interface->table2Interface()->rowCount();
}

int QSpiAdaptor::nSelectedColumns() const
{
    if (!checkInterface()) return 0;
    return interface->table2Interface()->selectedColumnCount();
}

int QSpiAdaptor::nSelectedRows() const
{
    if (!checkInterface()) return 0;
    return interface->table2Interface()->selectedRowCount();
}

QSpiObjectReference QSpiAdaptor::summary() const
{
    if (!checkInterface()) return QSpiObjectReference();
    QAccessibleInterface* summaryObject = interface->table2Interface()->summary();
    if (summaryObject)
        return spiBridge->interfaceToAccessible(summaryObject, 0, true)->getReference();
    return QSpiObjectReference();
}

bool QSpiAdaptor::AddColumnSelection(int column)
{
    if (!checkInterface()) return false;
    return interface->table2Interface()->selectColumn(column);
}

bool QSpiAdaptor::AddRowSelection(int row)
{
    if (!checkInterface()) return false;
    return interface->table2Interface()->selectRow(row);
}

QSpiObjectReference QSpiAdaptor::GetAccessibleAt(int row, int column)
{
    if (!checkInterface()) return QSpiObjectReference();
    Q_ASSERT(interface->table2Interface());
    Q_ASSERT(row >= 0);
    Q_ASSERT(column >= 0);
    qDebug() << "GetAccessibleAt" << row << column;

    QAccessibleInterface* cell = interface->table2Interface()->cellAt(row, column);

    if (!cell) {
        qWarning() << "WARNING: no row interface returned for " << interface->object();
        return QSpiObjectReference();
    }
    return spiBridge->interfaceToAccessible(cell, 0, true)->getReference();
}

int QSpiAdaptor::GetColumnAtIndex(int index)
{
    if (!checkInterface()) return 0;

    Q_ASSERT(0);
    int cols = interface->table2Interface()->columnCount();
    return index%cols;
}

QString QSpiAdaptor::GetColumnDescription(int column)
{
    if (!checkInterface()) return QString();
    return interface->table2Interface()->columnDescription(column);
}

int QSpiAdaptor::GetColumnExtentAt(int row, int column)
{
    if (!checkInterface()) return 0;
    return interface->table2Interface()->cellAt(row, column)->columnExtent();
}

QSpiObjectReference QSpiAdaptor::GetColumnHeader(int column)
{
    if (!checkInterface()) return QSpiObjectReference();
    Q_UNUSED (column);
    // TODO There should be a column param in this function right?

    return QSpiObjectReference();
//    return spiBridge->objectToAccessible(interface->tableInterface()->columnHeader()->object())->getReference();
}

int QSpiAdaptor::GetIndexAt(int row, int column)
{
    if (!checkInterface()) return 0;
    return row*interface->table2Interface()->columnCount()+column;
}

int QSpiAdaptor::GetRowAtIndex(int index)
{
    if (!checkInterface()) return 0;
    Q_ASSERT(0);
    int cols = interface->table2Interface()->columnCount();
    return index/cols;
}

bool QSpiAdaptor::GetRowColumnExtentsAtIndex(int index,
						  int &row,
						  int &col,
						  int &row_extents,
						  int &col_extents,
						  bool &is_selected)
{
    if (!checkInterface()) return false;

    int cols = interface->table2Interface()->columnCount();
    row = index/cols;
    col = index%cols;
    QAccessibleTable2CellInterface *cell = interface->table2Interface()->cellAt(row, col);
    if (cell) {
        cell->rowColumnExtents(&row, &col, &row_extents, &col_extents, &is_selected);
        return true;
    }

    return false;
}

QString QSpiAdaptor::GetRowDescription(int row)
{
    if (!checkInterface()) return QString();
    return interface->table2Interface()->rowDescription(row);
}

int QSpiAdaptor::GetRowExtentAt(int row, int column)
{
    if (!checkInterface()) return 0;
    return interface->table2Interface()->cellAt(row, column)->rowExtent();
}

QSpiObjectReference QSpiAdaptor::GetRowHeader(int row)
{
    if (!checkInterface()) return QSpiObjectReference();
    Q_UNUSED (row);
    qWarning() << "Implement: QSpiAdaptor::GetRowHeader";
    return QSpiObjectReference();

    // TODO There should be a row param here right?
//    return spiBridge->objectToAccessible(interface->tableInterface()->rowHeader()->object())->getReference();
}

QSpiIntList QSpiAdaptor::GetSelectedColumns()
{
    QSpiIntList columns;
    if (!checkInterface()) return columns;
    columns = interface->table2Interface()->selectedColumns();
    return columns;
}

QSpiIntList QSpiAdaptor::GetSelectedRows()
{
    QSpiIntList rows;
    if (!checkInterface()) return rows;
    rows = interface->table2Interface()->selectedRows();
    return rows;
}

bool QSpiAdaptor::IsColumnSelected(int column)
{
    if (!checkInterface()) return false;
    return interface->table2Interface()->isColumnSelected(column);
}

bool QSpiAdaptor::IsRowSelected(int row)
{
    if (!checkInterface()) return false;
    return interface->table2Interface()->isRowSelected(row);
}

bool QSpiAdaptor::IsSelected(int row, int column)
{
    if (!checkInterface()) return false;
    QAccessibleTable2CellInterface* cell = interface->table2Interface()->cellAt(row, column);
    bool selected = cell->isSelected();
    delete cell;
    return selected;
}

bool QSpiAdaptor::RemoveColumnSelection(int column)
{
    if (!checkInterface()) return false;
    return interface->table2Interface()->unselectColumn(column);
}

bool QSpiAdaptor::RemoveRowSelection(int row)
{
    if (!checkInterface()) return false;
    return interface->table2Interface()->unselectRow(row);
}

/* AT-SPI Text interface ----------------------------------------------------*/
/*---------------------------------------------------------------------------*/

int QSpiAdaptor::caretOffset() const
{
    if (!checkInterface()) return 0;
    return interface->textInterface()->cursorPosition();
}

int QSpiAdaptor::characterCount() const
{
    if (!checkInterface()) return 0;
    return interface->textInterface()->characterCount();
}

bool QSpiAdaptor::AddSelection(int startOffset, int endOffset)
{
    if (!checkInterface()) return false;
    int lastSelection = interface->textInterface()->selectionCount ();
    interface->textInterface()->setSelection (lastSelection, startOffset, endOffset);
    return interface->textInterface()->selectionCount() > lastSelection;
}

QSpiAttributeSet QSpiAdaptor::GetAttributeRun(int offset,
				              bool includeDefaults,
					      int &startOffset,
					      int &endOffset)
{
    if (!checkInterface()) return QSpiAttributeSet();
    Q_UNUSED (includeDefaults);
    return GetAttributes (offset, startOffset, endOffset);
}

QString QSpiAdaptor::GetAttributeValue(int offset,
		  		      const QString &attributeName,
				      int &startOffset,
				      int &endOffset,
				      bool &defined)
{
    if (!checkInterface()) return QString();
    QString     mapped;
    QString     joined;
    QStringList attributes;
    QSpiAttributeSet map;

    joined = interface->textInterface()->attributes(offset, &startOffset, &endOffset);
    attributes = joined.split (';', QString::SkipEmptyParts, Qt::CaseSensitive);
    foreach (QString attr, attributes)
    {
        QStringList items;
        items = attr.split(':', QString::SkipEmptyParts, Qt::CaseSensitive);
        map[items[0]] = items[1];
    }
    mapped = map[attributeName];
    if (mapped == "")
       defined = TRUE;
    else
       defined = FALSE;
    return mapped;
}

QSpiAttributeSet QSpiAdaptor::GetAttributes(int offset, int &startOffset, int &endOffset)
{
    QSpiAttributeSet set;
    if (!checkInterface()) return set;

    QString     joined;
    QStringList attributes;

    joined = interface->textInterface()->attributes(offset, &startOffset, &endOffset);
    attributes = joined.split (';', QString::SkipEmptyParts, Qt::CaseSensitive);
    foreach (const QString &attr, attributes)
    {
        QStringList items;
        items = attr.split(':', QString::SkipEmptyParts, Qt::CaseSensitive);
        set[items[0]] = items[1];
    }
    return set;
}

QSpiTextRangeList QSpiAdaptor::GetBoundedRanges(int x,
					        int y,
					        int width,
					        int height,
					        uint coordType,
					        uint xClipType,
					        uint yClipType)
{
    if (!checkInterface()) return QSpiTextRangeList();
    qWarning("Not implemented: QSpiAdaptor::GetBoundedRanges");
    Q_UNUSED(x) Q_UNUSED (y) Q_UNUSED(width)
    Q_UNUSED(height) Q_UNUSED(coordType)
    Q_UNUSED(xClipType) Q_UNUSED(yClipType)
    QSpiTextRangeList out0;
    return out0;
}

int QSpiAdaptor::GetCharacterAtOffset(int offset)
{
    if (!checkInterface()) return 0;
    int start=offset, end=offset+1;
    QString result;
    result = interface->textInterface()->textAtOffset(offset, QAccessible2::CharBoundary, &start, &end);
    return *(qPrintable (result));
}

int QSpiAdaptor::GetCharacterExtents(int offset, uint coordType, int &y, int &width, int &height)
{
    if (!checkInterface()) return 0;
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
    if (!checkInterface()) return attributes;
    return attributes;
}

QSpiAttributeSet QSpiAdaptor::GetDefaultAttributes()
{
    // Deprecated in favour of default attribute set.
    return GetDefaultAttributeSet();
}

int QSpiAdaptor::GetNSelections()
{
    if (!checkInterface()) return 0;
    return interface->textInterface()->selectionCount();
}

int QSpiAdaptor::GetOffsetAtPoint(int x, int y, uint coordType)
{
    if (!checkInterface()) return -1;
    return interface->textInterface()->offsetAtPoint (QPoint (x, y), static_cast <QAccessible2::CoordinateType> (coordType));
}

int QSpiAdaptor::GetRangeExtents(int startOffset, int endOffset, uint coordType, int &y, int &width, int &height)
{
    if (!checkInterface()) return -1;

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
    if (!checkInterface()) return -1;
    int start, end;
    interface->textInterface()->selection(selectionNum, &start, &end);

    if (start<0) {
        endOffset = interface->textInterface()->cursorPosition();
        return endOffset;
    }

    endOffset = end;
    return start;
}

QString QSpiAdaptor::GetText(int startOffset, int endOffset)
{
    if (!checkInterface()) return QString();
    if (endOffset == -1)
        endOffset = interface->textInterface()->characterCount();
    return interface->textInterface()->text(startOffset, endOffset);
}

QString QSpiAdaptor::GetTextAfterOffset(int offset, uint type, int &startOffset, int &endOffset)
{
    if (!checkInterface()) return QString();
    // FIXME find out if IA2 types are the same as the ones in at-spi
    return interface->textInterface()->textAfterOffset(offset, (QAccessible2::BoundaryType)type, &startOffset, &endOffset);
}

QString QSpiAdaptor::GetTextAtOffset(int offset, uint type, int &startOffset, int &endOffset)
{
    if (!checkInterface()) return QString();
    QAccessibleTextInterface * t = interface->textInterface();
    QAccessible2::BoundaryType rType;
    switch (type) {
    case ATSPI_TEXT_BOUNDARY_CHAR:
        rType = QAccessible2::CharBoundary;
        break;
    case ATSPI_TEXT_BOUNDARY_WORD_START: {
        QString text = t->textAtOffset(offset, QAccessible2::WordBoundary, &startOffset, &endOffset);

        if ((startOffset < 0) || (endOffset < 0))
            return text;

        int limit = t->characterCount();
        for (int i=endOffset + 1; i < limit; i++) {
            int j;
            int k;
            t->textAtOffset(i, QAccessible2::WordBoundary, &j, &k);
            if (j <= i) {
                endOffset = j;
                break;
            }
        }
        return t->text(startOffset, endOffset); }
    case ATSPI_TEXT_BOUNDARY_WORD_END: {
        QString text = t->textAtOffset(offset, QAccessible2::WordBoundary, &startOffset, &endOffset);

        if ((startOffset < 0) || (endOffset < 0))
            return text;

        for (int i=startOffset - 1; i >= 0; i--) {
            int j;
            int k;
            t->textAtOffset(i, QAccessible2::WordBoundary, &j, &k);
            if (k >= i) {
                startOffset = k;
                break;
            }
        }
        return t->text(startOffset, endOffset); }
    case ATSPI_TEXT_BOUNDARY_SENTENCE_END:
    case ATSPI_TEXT_BOUNDARY_SENTENCE_START:
        rType = QAccessible2::SentenceBoundary;
        break;
    case ATSPI_TEXT_BOUNDARY_LINE_START: {
        QString text = t->textAtOffset(offset, QAccessible2::LineBoundary, &startOffset, &endOffset);

        if ((startOffset < 0) || (endOffset < 0))
            return text;

        int limit = t->characterCount();
        for (int i=endOffset + 1; i < limit; i++) {
            int j;
            int k;
            t->textAtOffset(i, QAccessible2::LineBoundary, &j, &k);
            if (j <= i) {
                endOffset = j;
                break;
            }
        }
        return t->text(startOffset, endOffset); }
    case ATSPI_TEXT_BOUNDARY_LINE_END: {
        QString text = t->textAtOffset(offset, QAccessible2::LineBoundary, &startOffset, &endOffset);

        if ((startOffset < 0) || (endOffset < 0))
            return text;

        if (startOffset <= offset)
            text = t->textAtOffset(offset + 1, QAccessible2::LineBoundary, &startOffset, &endOffset);

        for (int i=startOffset - 1; i >= 0; i--) {
            int j;
            int k;
            t->textAtOffset(i, QAccessible2::LineBoundary, &j, &k);
            if (k >= i) {
                startOffset = k;
                break;
            }
        }
        return t->text(startOffset, endOffset); }
    default:
        startOffset = -1;
        endOffset = -1;
        return QString();
    }

    return t->textAtOffset(offset, rType, &startOffset, &endOffset);
}

QString QSpiAdaptor::GetTextBeforeOffset(int offset, uint type, int &startOffset, int &endOffset)
{
    if (!checkInterface()) return QString();
    // FIXME find out if IA2 types are the same as the ones in at-spi
    return interface->textInterface()->textBeforeOffset(offset, (QAccessible2::BoundaryType)type, &startOffset, &endOffset);
}

bool QSpiAdaptor::RemoveSelection(int selectionNum)
{
    if (!checkInterface()) return false;
    interface->textInterface()->removeSelection(selectionNum);
    return true;
}

bool QSpiAdaptor::SetCaretOffset(int offset)
{
    if (!checkInterface()) return false;
    interface->textInterface()->setCursorPosition(offset);
    return true;
}

bool QSpiAdaptor::SetSelection(int selectionNum, int startOffset, int endOffset)
{
    if (!checkInterface()) return false;
    interface->textInterface()->setSelection(selectionNum, startOffset, endOffset);
    return true;
}

/* AT-SPI Value interface ---------------------------------------------------*/
/*---------------------------------------------------------------------------*/

double QSpiAdaptor::currentValue() const
{
    if (!checkInterface()) return 0.0;
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

void QSpiAdaptor::SetCurrentValue(double value)
{
    if (!checkInterface()) return;
    interface->valueInterface()->setCurrentValue(QVariant (value));
}

double QSpiAdaptor::maximumValue() const
{
    if (!checkInterface()) return 0.0;
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
    if (!checkInterface()) return 0.0;
    // FIXME: should be in value interface
    return 0.0;
}

double QSpiAdaptor::minimumValue() const
{
    if (!checkInterface()) return 0.0;
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
