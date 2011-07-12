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

#ifndef Q_SPI_ADAPTOR_H
#define Q_SPI_ADAPTOR_H

#include <QtCore/QObject>
#include <QtGui/QAccessibleInterface>

#include "struct_marshallers.h"

#define QSPI_INTERFACE_PREFIX "org.a11y.atspi"

#define QSPI_INTERFACE_ACCESSIBLE            QSPI_INTERFACE_PREFIX ".Accessible"
#define QSPI_INTERFACE_ACTION                QSPI_INTERFACE_PREFIX ".Action"
#define QSPI_INTERFACE_APPLICATION           QSPI_INTERFACE_PREFIX ".Application"
#define QSPI_INTERFACE_COLLECTION            QSPI_INTERFACE_PREFIX ".Collection"
#define QSPI_INTERFACE_COMPONENT             QSPI_INTERFACE_PREFIX ".Component"
#define QSPI_INTERFACE_DOCUMENT              QSPI_INTERFACE_PREFIX ".Document"
#define QSPI_INTERFACE_EDITABLE_TEXT         QSPI_INTERFACE_PREFIX ".EditableText"
#define QSPI_INTERFACE_HYPERLINK             QSPI_INTERFACE_PREFIX ".Hyperlink"
#define QSPI_INTERFACE_HYPERTEXT             QSPI_INTERFACE_PREFIX ".Hypertext"
#define QSPI_INTERFACE_IMAGE                 QSPI_INTERFACE_PREFIX ".Image"
#define QSPI_INTERFACE_REGISTRY              QSPI_INTERFACE_PREFIX ".Registry"
#define QSPI_INTERFACE_SELECTION             QSPI_INTERFACE_PREFIX ".Selection"
#define QSPI_INTERFACE_TABLE                 QSPI_INTERFACE_PREFIX ".Table"
#define QSPI_INTERFACE_TEXT                  QSPI_INTERFACE_PREFIX ".Text"
#define QSPI_INTERFACE_TREE                  QSPI_INTERFACE_PREFIX ".Tree"
#define QSPI_INTERFACE_VALUE                 QSPI_INTERFACE_PREFIX ".Value"

#define QSPI_OBJECT_PATH_PREFIX  "/org/a11y/atspi/accessible/"
#define QSPI_OBJECT_PATH_NULL    QSPI_OBJECT_PATH_PREFIX "null"
#define QSPI_OBJECT_PATH_ROOT    QSPI_OBJECT_PATH_PREFIX "root"

// FIXME lots of const missing for all the getters...

/*
 * Implements all methods neccessary to adapt calls from AT-SPI to the
 * QAccessibleInterface.
 *
 */
class QSpiAdaptor :public QObject
{
    Q_OBJECT

public:
    QSpiAdaptor(QAccessibleInterface *interface, int index);
    virtual ~QSpiAdaptor() {}

    QStringList getSupportedInterfaces() const;
    QSpiAccessibleCacheItem getCacheItem() const;

    virtual QSpiObjectReference getReference() const;
    virtual QSpiObjectReference getParentReference() const = 0;

    virtual void accessibleEvent(QAccessible::Event event) = 0;
    inline QAccessibleInterface* associatedInterface() const { return interface; }
    inline int childIndex() const { return child; }

    QObject* getObject() const;

public:
    // event stuff
    void signalChildrenChanged(const QString &type, int detail1, int detail2, const QDBusVariant &data);
Q_SIGNALS:
    void ChildrenChanged(const QString &type, int detail1, int detail2, const QDBusVariant &data, const QSpiObjectReference &parent);
    void StateChanged(const QString &type, int detail1, int detail2, const QDBusVariant &data, const QSpiObjectReference &parent);
    void PropertyChange(const QString &type, int detail1, int detail2, const QDBusVariant &data, const QSpiObjectReference &parent);
    void Focus(const QString &type, int detail1, int detail2, const QDBusVariant &data, const QSpiObjectReference &parent);
    void TextChanged(const QString &type, int detail1, int detail2, const QDBusVariant &data, const QSpiObjectReference &parent);
    void TextCaretMoved(const QString &type, int detail1, int detail2, const QDBusVariant &data, const QSpiObjectReference &parent);

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
    QSpiObjectReference GetApplication() const;
    QSpiAttributeSet GetAttributes() const;
    QSpiObjectReference GetChildAtIndex(int index) const;
    QSpiObjectReferenceArray GetChildren() const;
    int GetIndexInParent() const;
    QString GetLocalizedRoleName() const;
    QSpiRelationArray GetRelationSet() const;
    uint GetRole() const;
    QString GetRoleName() const;
    QSpiUIntList GetState() const;

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
    QString GetApplicationBusAddress() const;

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
    QSpiObjectReference GetAccessibleAt(int row, int column);
    int GetIndexAt(int row, int column);
    int GetColumnAtIndex(int index);
    int GetRowAtIndex(int index);

    QSpiObjectReference GetColumnHeader(int column);
    QSpiObjectReference GetRowHeader(int row);
    QString GetColumnDescription(int column);
    QString GetRowDescription(int row);

    int GetColumnExtentAt(int row, int column);
    int GetRowExtentAt(int row, int column);
    bool GetRowColumnExtentsAtIndex(int index, int &row, int &col, int &row_extents, int &col_extents, bool &is_selected);

    bool IsSelected(int row, int column);
    bool IsColumnSelected(int column);
    bool IsRowSelected(int row);
    QSpiIntList GetSelectedColumns();
    QSpiIntList GetSelectedRows();

    bool AddColumnSelection(int column);
    bool AddRowSelection(int row);
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
    QSpiTextRangeList GetBoundedRanges(int x, int y, int width, int height, uint coordType, uint xClipType, uint yClipType);
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
    Q_PROPERTY(double CurrentValue READ currentValue WRITE SetCurrentValue)
    double currentValue() const;
    void SetCurrentValue(double value);
    Q_PROPERTY(double MaximumValue READ maximumValue)
    double maximumValue() const;
    Q_PROPERTY(double MinimumIncrement READ minimumIncrement)
    double minimumIncrement() const;
    Q_PROPERTY(double MinimumValue READ minimumValue)
    double minimumValue() const;



public:
    QSpiAdaptor* getChild(int child) const;
protected:
    QAccessibleInterface *interface;
    QSpiObjectReference reference;
    QStringList supportedInterfaces;

private:
    bool checkInterface() const;
    int child;
};

#endif /* Q_SPI_ADAPTOR_H */
