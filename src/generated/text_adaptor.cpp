/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -i marshallers.h -a text_adaptor ../../xml/Text.xml
 *
 * qdbusxml2cpp is Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "text_adaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class TextAdaptor
 */

TextAdaptor::TextAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

TextAdaptor::~TextAdaptor()
{
    // destructor
}

int TextAdaptor::caretOffset() const
{
    // get the value of property caretOffset
    return qvariant_cast< int >(parent()->property("caretOffset"));
}

int TextAdaptor::characterCount() const
{
    // get the value of property characterCount
    return qvariant_cast< int >(parent()->property("characterCount"));
}

bool TextAdaptor::AddSelection(int startOffset, int endOffset)
{
    // handle method call org.a11y.atspi.Text.AddSelection
    bool out0;
    QMetaObject::invokeMethod(parent(), "AddSelection", Q_RETURN_ARG(bool, out0), Q_ARG(int, startOffset), Q_ARG(int, endOffset));
    return out0;
}

QSpiAttributeSet TextAdaptor::GetAttributeRun(int offset, bool includeDefaults, int &startOffset, int &endOffset)
{
    // handle method call org.a11y.atspi.Text.GetAttributeRun
    //return static_cast<YourObjectType *>(parent())->GetAttributeRun(offset, includeDefaults, startOffset, endOffset);
}

QString TextAdaptor::GetAttributeValue(int offset, const QString &attributeName, int &startOffset, int &endOffset, bool &defined)
{
    // handle method call org.a11y.atspi.Text.GetAttributeValue
    //return static_cast<YourObjectType *>(parent())->GetAttributeValue(offset, attributeName, startOffset, endOffset, defined);
}

QSpiAttributeSet TextAdaptor::GetAttributes(int offset, int &startOffset, int &endOffset)
{
    // handle method call org.a11y.atspi.Text.GetAttributes
    //return static_cast<YourObjectType *>(parent())->GetAttributes(offset, startOffset, endOffset);
}

QSpiRangeList TextAdaptor::GetBoundedRanges(int x, int y, int width, int height, uint coordType, uint xClipType, uint yClipType)
{
    // handle method call org.a11y.atspi.Text.GetBoundedRanges
    QSpiRangeList out0;
    QMetaObject::invokeMethod(parent(), "GetBoundedRanges", Q_RETURN_ARG(QSpiRangeList, out0), Q_ARG(int, x), Q_ARG(int, y), Q_ARG(int, width), Q_ARG(int, height), Q_ARG(uint, coordType), Q_ARG(uint, xClipType), Q_ARG(uint, yClipType));
    return out0;
}

int TextAdaptor::GetCharacterAtOffset(int offset)
{
    // handle method call org.a11y.atspi.Text.GetCharacterAtOffset
    int out0;
    QMetaObject::invokeMethod(parent(), "GetCharacterAtOffset", Q_RETURN_ARG(int, out0), Q_ARG(int, offset));
    return out0;
}

int TextAdaptor::GetCharacterExtents(int offset, uint coordType, int &y, int &width, int &height)
{
    // handle method call org.a11y.atspi.Text.GetCharacterExtents
    //return static_cast<YourObjectType *>(parent())->GetCharacterExtents(offset, coordType, y, width, height);
}

QStringList TextAdaptor::GetDefaultAttributeSet()
{
    // handle method call org.a11y.atspi.Text.GetDefaultAttributeSet
    QStringList out0;
    QMetaObject::invokeMethod(parent(), "GetDefaultAttributeSet", Q_RETURN_ARG(QStringList, out0));
    return out0;
}

QSpiAttributeSet TextAdaptor::GetDefaultAttributes()
{
    // handle method call org.a11y.atspi.Text.GetDefaultAttributes
    QSpiAttributeSet out0;
    QMetaObject::invokeMethod(parent(), "GetDefaultAttributes", Q_RETURN_ARG(QSpiAttributeSet, out0));
    return out0;
}

int TextAdaptor::GetNSelections()
{
    // handle method call org.a11y.atspi.Text.GetNSelections
    int out0;
    QMetaObject::invokeMethod(parent(), "GetNSelections", Q_RETURN_ARG(int, out0));
    return out0;
}

int TextAdaptor::GetOffsetAtPoint(int x, int y, uint coordType)
{
    // handle method call org.a11y.atspi.Text.GetOffsetAtPoint
    int out0;
    QMetaObject::invokeMethod(parent(), "GetOffsetAtPoint", Q_RETURN_ARG(int, out0), Q_ARG(int, x), Q_ARG(int, y), Q_ARG(uint, coordType));
    return out0;
}

int TextAdaptor::GetRangeExtents(int startOffset, int endOffset, uint coordType, int &y, int &width, int &height)
{
    // handle method call org.a11y.atspi.Text.GetRangeExtents
    //return static_cast<YourObjectType *>(parent())->GetRangeExtents(startOffset, endOffset, coordType, y, width, height);
}

int TextAdaptor::GetSelection(int selectionNum, int &endOffset)
{
    // handle method call org.a11y.atspi.Text.GetSelection
    //return static_cast<YourObjectType *>(parent())->GetSelection(selectionNum, endOffset);
}

QString TextAdaptor::GetText(int startOffset, int endOffset)
{
    // handle method call org.a11y.atspi.Text.GetText
    QString out0;
    QMetaObject::invokeMethod(parent(), "GetText", Q_RETURN_ARG(QString, out0), Q_ARG(int, startOffset), Q_ARG(int, endOffset));
    return out0;
}

QString TextAdaptor::GetTextAfterOffset(int offset, uint type, int &startOffset, int &endOffset)
{
    // handle method call org.a11y.atspi.Text.GetTextAfterOffset
    //return static_cast<YourObjectType *>(parent())->GetTextAfterOffset(offset, type, startOffset, endOffset);
}

QString TextAdaptor::GetTextAtOffset(int offset, uint type, int &startOffset, int &endOffset)
{
    // handle method call org.a11y.atspi.Text.GetTextAtOffset
    //return static_cast<YourObjectType *>(parent())->GetTextAtOffset(offset, type, startOffset, endOffset);
}

QString TextAdaptor::GetTextBeforeOffset(int offset, uint type, int &startOffset, int &endOffset)
{
    // handle method call org.a11y.atspi.Text.GetTextBeforeOffset
    //return static_cast<YourObjectType *>(parent())->GetTextBeforeOffset(offset, type, startOffset, endOffset);
}

bool TextAdaptor::RemoveSelection(int selectionNum)
{
    // handle method call org.a11y.atspi.Text.RemoveSelection
    bool out0;
    QMetaObject::invokeMethod(parent(), "RemoveSelection", Q_RETURN_ARG(bool, out0), Q_ARG(int, selectionNum));
    return out0;
}

bool TextAdaptor::SetCaretOffset(int offset)
{
    // handle method call org.a11y.atspi.Text.SetCaretOffset
    bool out0;
    QMetaObject::invokeMethod(parent(), "SetCaretOffset", Q_RETURN_ARG(bool, out0), Q_ARG(int, offset));
    return out0;
}

bool TextAdaptor::SetSelection(int selectionNum, int startOffset, int endOffset)
{
    // handle method call org.a11y.atspi.Text.SetSelection
    bool out0;
    QMetaObject::invokeMethod(parent(), "SetSelection", Q_RETURN_ARG(bool, out0), Q_ARG(int, selectionNum), Q_ARG(int, startOffset), Q_ARG(int, endOffset));
    return out0;
}

