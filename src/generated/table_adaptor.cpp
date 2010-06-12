/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -i marshallers.h -a table_adaptor ../../xml/Table.xml
 *
 * qdbusxml2cpp is Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "table_adaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class TableAdaptor
 */

TableAdaptor::TableAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

TableAdaptor::~TableAdaptor()
{
    // destructor
}

QSpiObjectReference TableAdaptor::caption() const
{
    // get the value of property caption
    return qvariant_cast< QSpiObjectReference >(parent()->property("caption"));
}

int TableAdaptor::nColumns() const
{
    // get the value of property nColumns
    return qvariant_cast< int >(parent()->property("nColumns"));
}

int TableAdaptor::nRows() const
{
    // get the value of property nRows
    return qvariant_cast< int >(parent()->property("nRows"));
}

int TableAdaptor::nSelectedColumns() const
{
    // get the value of property nSelectedColumns
    return qvariant_cast< int >(parent()->property("nSelectedColumns"));
}

int TableAdaptor::nSelectedRows() const
{
    // get the value of property nSelectedRows
    return qvariant_cast< int >(parent()->property("nSelectedRows"));
}

QSpiObjectReference TableAdaptor::summary() const
{
    // get the value of property summary
    return qvariant_cast< QSpiObjectReference >(parent()->property("summary"));
}

bool TableAdaptor::AddColumnSelection(int column)
{
    // handle method call org.a11y.atspi.Table.AddColumnSelection
    bool out0;
    QMetaObject::invokeMethod(parent(), "AddColumnSelection", Q_RETURN_ARG(bool, out0), Q_ARG(int, column));
    return out0;
}

bool TableAdaptor::AddRowSelection(int row)
{
    // handle method call org.a11y.atspi.Table.AddRowSelection
    bool out0;
    QMetaObject::invokeMethod(parent(), "AddRowSelection", Q_RETURN_ARG(bool, out0), Q_ARG(int, row));
    return out0;
}

QSpiObjectReference TableAdaptor::GetAccessibleAt(int row, int column)
{
    // handle method call org.a11y.atspi.Table.GetAccessibleAt
    QSpiObjectReference out0;
    QMetaObject::invokeMethod(parent(), "GetAccessibleAt", Q_RETURN_ARG(QSpiObjectReference, out0), Q_ARG(int, row), Q_ARG(int, column));
    return out0;
}

int TableAdaptor::GetColumnAtIndex(int index)
{
    // handle method call org.a11y.atspi.Table.GetColumnAtIndex
    int out0;
    QMetaObject::invokeMethod(parent(), "GetColumnAtIndex", Q_RETURN_ARG(int, out0), Q_ARG(int, index));
    return out0;
}

QString TableAdaptor::GetColumnDescription(int column)
{
    // handle method call org.a11y.atspi.Table.GetColumnDescription
    QString out0;
    QMetaObject::invokeMethod(parent(), "GetColumnDescription", Q_RETURN_ARG(QString, out0), Q_ARG(int, column));
    return out0;
}

int TableAdaptor::GetColumnExtentAt(int row, int column)
{
    // handle method call org.a11y.atspi.Table.GetColumnExtentAt
    int out0;
    QMetaObject::invokeMethod(parent(), "GetColumnExtentAt", Q_RETURN_ARG(int, out0), Q_ARG(int, row), Q_ARG(int, column));
    return out0;
}

QSpiObjectReference TableAdaptor::GetColumnHeader(int column)
{
    // handle method call org.a11y.atspi.Table.GetColumnHeader
    QSpiObjectReference out0;
    QMetaObject::invokeMethod(parent(), "GetColumnHeader", Q_RETURN_ARG(QSpiObjectReference, out0), Q_ARG(int, column));
    return out0;
}

int TableAdaptor::GetIndexAt(int row, int column)
{
    // handle method call org.a11y.atspi.Table.GetIndexAt
    int out0;
    QMetaObject::invokeMethod(parent(), "GetIndexAt", Q_RETURN_ARG(int, out0), Q_ARG(int, row), Q_ARG(int, column));
    return out0;
}

int TableAdaptor::GetRowAtIndex(int index)
{
    // handle method call org.a11y.atspi.Table.GetRowAtIndex
    int out0;
    QMetaObject::invokeMethod(parent(), "GetRowAtIndex", Q_RETURN_ARG(int, out0), Q_ARG(int, index));
    return out0;
}

bool TableAdaptor::GetRowColumnExtentsAtIndex(int index, int &row, int &col, int &row_extents, int &col_extents, bool &is_selected)
{
    // handle method call org.a11y.atspi.Table.GetRowColumnExtentsAtIndex
    //return static_cast<YourObjectType *>(parent())->GetRowColumnExtentsAtIndex(index, row, col, row_extents, col_extents, is_selected);
}

QString TableAdaptor::GetRowDescription(int row)
{
    // handle method call org.a11y.atspi.Table.GetRowDescription
    QString out0;
    QMetaObject::invokeMethod(parent(), "GetRowDescription", Q_RETURN_ARG(QString, out0), Q_ARG(int, row));
    return out0;
}

int TableAdaptor::GetRowExtentAt(int row, int column)
{
    // handle method call org.a11y.atspi.Table.GetRowExtentAt
    int out0;
    QMetaObject::invokeMethod(parent(), "GetRowExtentAt", Q_RETURN_ARG(int, out0), Q_ARG(int, row), Q_ARG(int, column));
    return out0;
}

QSpiObjectReference TableAdaptor::GetRowHeader(int row)
{
    // handle method call org.a11y.atspi.Table.GetRowHeader
    QSpiObjectReference out0;
    QMetaObject::invokeMethod(parent(), "GetRowHeader", Q_RETURN_ARG(QSpiObjectReference, out0), Q_ARG(int, row));
    return out0;
}

QSpiIntList TableAdaptor::GetSelectedColumns()
{
    // handle method call org.a11y.atspi.Table.GetSelectedColumns
    QSpiIntList out0;
    QMetaObject::invokeMethod(parent(), "GetSelectedColumns", Q_RETURN_ARG(QSpiIntList, out0));
    return out0;
}

QSpiIntList TableAdaptor::GetSelectedRows()
{
    // handle method call org.a11y.atspi.Table.GetSelectedRows
    QSpiIntList out0;
    QMetaObject::invokeMethod(parent(), "GetSelectedRows", Q_RETURN_ARG(QSpiIntList, out0));
    return out0;
}

bool TableAdaptor::IsColumnSelected(int column)
{
    // handle method call org.a11y.atspi.Table.IsColumnSelected
    bool out0;
    QMetaObject::invokeMethod(parent(), "IsColumnSelected", Q_RETURN_ARG(bool, out0), Q_ARG(int, column));
    return out0;
}

bool TableAdaptor::IsRowSelected(int row)
{
    // handle method call org.a11y.atspi.Table.IsRowSelected
    bool out0;
    QMetaObject::invokeMethod(parent(), "IsRowSelected", Q_RETURN_ARG(bool, out0), Q_ARG(int, row));
    return out0;
}

bool TableAdaptor::IsSelected(int row, int column)
{
    // handle method call org.a11y.atspi.Table.IsSelected
    bool out0;
    QMetaObject::invokeMethod(parent(), "IsSelected", Q_RETURN_ARG(bool, out0), Q_ARG(int, row), Q_ARG(int, column));
    return out0;
}

bool TableAdaptor::RemoveColumnSelection(int column)
{
    // handle method call org.a11y.atspi.Table.RemoveColumnSelection
    bool out0;
    QMetaObject::invokeMethod(parent(), "RemoveColumnSelection", Q_RETURN_ARG(bool, out0), Q_ARG(int, column));
    return out0;
}

bool TableAdaptor::RemoveRowSelection(int row)
{
    // handle method call org.a11y.atspi.Table.RemoveRowSelection
    bool out0;
    QMetaObject::invokeMethod(parent(), "RemoveRowSelection", Q_RETURN_ARG(bool, out0), Q_ARG(int, row));
    return out0;
}
