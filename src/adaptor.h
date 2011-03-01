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

#ifndef Q_SPI_ADAPTOR_H
#define Q_SPI_ADAPTOR_H

#include "object.h"

/*
 * Implements all methods neccessary to adapt calls from AT-SPI to the
 * QAccessibleInterface.
 *
 */
class QSpiAdaptor :public QSpiObject
{
    Q_OBJECT
public:
    QSpiAdaptor(QAccessibleInterface *interface)
        : QSpiObject(interface) {}

/* AT-SPI Accessible interface */
public:
    Q_PROPERTY(int ChildCount READ childCount)
    int childCount() const;
    Q_PROPERTY(QString Description READ description)
    QString description() const;
    Q_PROPERTY(QString Name READ name)
    QString name() const;
    Q_PROPERTY(QSpiObjectReference Parent READ parent)
    QSpiObjectReference parent() const;

public Q_SLOTS:
    QSpiObjectReference GetApplication();
    QSpiAttributeSet GetAttributes();
    QSpiObjectReference GetChildAtIndex(int index);
    QSpiObjectReferenceArray GetChildren();
    int GetIndexInParent();
    QString GetLocalizedRoleName();
    QSpiRelationArray GetRelationSet();
    uint GetRole();
    QString GetRoleName();
    QSpiUIntList GetState();

/* AT-SPI Action interface */
public:
    Q_PROPERTY(int NActions READ nActions)
    int nActions() const;

public Q_SLOTS:
    bool DoAction(int index);
    QSpiActionArray GetActions();
    QString GetDescription(int index);
    QString GetKeyBinding(int index);
    QString GetName(int index);

/* AT-SPI Application interface */
public:
    Q_PROPERTY(int Id READ id)
    int id() const;
    Q_PROPERTY(QString ToolkitName READ toolkitName)
    QString toolkitName() const;
    Q_PROPERTY(QString Version READ version)
    QString version() const;

public Q_SLOTS: // METHODS
    QString GetLocale(uint lctype);

/* AT-SPI Component interface */
public Q_SLOTS:
    bool Contains(int x, int y, uint coord_type);
    QSpiObjectReference GetAccessibleAtPoint(int x, int y, uint coord_type);
    double GetAlpha();
    QSpiRect GetExtents(uint coord_type);
    uint GetLayer();
    short GetMDIZOrder();
    int GetPosition(uint coord_type, int &y);
    int GetSize(int &height);
    bool GrabFocus();

/* AT-SPI EditableText interface */
public Q_SLOTS:
    void CopyText(int startPos, int endPos);
    bool CutText(int startPos, int endPos);
    bool DeleteText(int startPos, int endPos);
    bool InsertText(int position, const QString &text, int length);
    bool PasteText(int position);
    bool SetTextContents(const QString &newContents);

/* AT-SPI Table interface */
public:
    Q_PROPERTY(QSpiObjectReference Caption READ caption)
    QSpiObjectReference caption() const;
    Q_PROPERTY(int NColumns READ nColumns)
    int nColumns() const;
    Q_PROPERTY(int NRows READ nRows)
    int nRows() const;
    Q_PROPERTY(int NSelectedColumns READ nSelectedColumns)
    int nSelectedColumns() const;
    Q_PROPERTY(int NSelectedRows READ nSelectedRows)
    int nSelectedRows() const;
    Q_PROPERTY(QSpiObjectReference Summary READ summary)
    QSpiObjectReference summary() const;

public Q_SLOTS:
    bool AddColumnSelection(int column);
    bool AddRowSelection(int row);
    QSpiObjectReference GetAccessibleAt(int row, int column);
    int GetColumnAtIndex(int index);
    QString GetColumnDescription(int column);
    int GetColumnExtentAt(int row, int column);
    QSpiObjectReference GetColumnHeader(int column);
    int GetIndexAt(int row, int column);
    int GetRowAtIndex(int index);
    bool GetRowColumnExtentsAtIndex(int index, int &row, int &col, int &row_extents, int &col_extents, bool &is_selected);
    QString GetRowDescription(int row);
    int GetRowExtentAt(int row, int column);
    QSpiObjectReference GetRowHeader(int row);
    QSpiIntList GetSelectedColumns();
    QSpiIntList GetSelectedRows();
    bool IsColumnSelected(int column);
    bool IsRowSelected(int row);
    bool IsSelected(int row, int column);
    bool RemoveColumnSelection(int column);
    bool RemoveRowSelection(int row);

/* AT-SPI Text interface */
public:
    Q_PROPERTY(int CaretOffset READ caretOffset)
    int caretOffset() const;
    Q_PROPERTY(int CharacterCount READ characterCount)
    int characterCount() const;

public Q_SLOTS:
    bool AddSelection(int startOffset, int endOffset);
    QSpiAttributeSet GetAttributeRun(int offset, bool includeDefaults, int &startOffset, int &endOffset);
    QString GetAttributeValue(int offset, const QString &attributeName, int &startOffset, int &endOffset, bool &defined);
    QSpiAttributeSet GetAttributes(int offset, int &startOffset, int &endOffset);
    QSpiRangeList GetBoundedRanges(int x, int y, int width, int height, uint coordType, uint xClipType, uint yClipType);
    int GetCharacterAtOffset(int offset);
    int GetCharacterExtents(int offset, uint coordType, int &y, int &width, int &height);
    QSpiAttributeSet GetDefaultAttributeSet();
    QSpiAttributeSet GetDefaultAttributes();
    int GetNSelections();
    int GetOffsetAtPoint(int x, int y, uint coordType);
    int GetRangeExtents(int startOffset, int endOffset, uint coordType, int &y, int &width, int &height);
    int GetSelection(int selectionNum, int &endOffset);
    QString GetText(int startOffset, int endOffset);
    QString GetTextAfterOffset(int offset, uint type, int &startOffset, int &endOffset);
    QString GetTextAtOffset(int offset, uint type, int &startOffset, int &endOffset);
    QString GetTextBeforeOffset(int offset, uint type, int &startOffset, int &endOffset);
    bool RemoveSelection(int selectionNum);
    bool SetCaretOffset(int offset);
    bool SetSelection(int selectionNum, int startOffset, int endOffset);

/* AT-SPI Text interface */
public:
    Q_PROPERTY(double CurrentValue READ currentValue WRITE setCurrentValue)
    double currentValue() const;
    void setCurrentValue(double value);
    Q_PROPERTY(double MaximumValue READ maximumValue)
    double maximumValue() const;
    Q_PROPERTY(double MinimumIncrement READ minimumIncrement)
    double minimumIncrement() const;
    Q_PROPERTY(double MinimumValue READ minimumValue)
    double minimumValue() const;
};

#endif /* Q_SPI_ADAPTOR_H */
