/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -i adaptor.h -a text_adaptor -l QSpiAdaptor ../../xml/Text.xml
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

TextAdaptor::TextAdaptor(QSpiAdaptor *parent)
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
    return parent()->AddSelection(startOffset, endOffset);
}

QSpiAttributeSet TextAdaptor::GetAttributeRun(int offset, bool includeDefaults, int &startOffset, int &endOffset)
{
    // handle method call org.a11y.atspi.Text.GetAttributeRun
    return parent()->GetAttributeRun(offset, includeDefaults, startOffset, endOffset);
}

QString TextAdaptor::GetAttributeValue(int offset, const QString &attributeName, int &startOffset, int &endOffset, bool &defined)
{
    // handle method call org.a11y.atspi.Text.GetAttributeValue
    return parent()->GetAttributeValue(offset, attributeName, startOffset, endOffset, defined);
}

QSpiAttributeSet TextAdaptor::GetAttributes(int offset, int &startOffset, int &endOffset)
{
    // handle method call org.a11y.atspi.Text.GetAttributes
    return parent()->GetAttributes(offset, startOffset, endOffset);
}

QSpiRangeList TextAdaptor::GetBoundedRanges(int x, int y, int width, int height, uint coordType, uint xClipType, uint yClipType)
{
    // handle method call org.a11y.atspi.Text.GetBoundedRanges
    return parent()->GetBoundedRanges(x, y, width, height, coordType, xClipType, yClipType);
}

int TextAdaptor::GetCharacterAtOffset(int offset)
{
    // handle method call org.a11y.atspi.Text.GetCharacterAtOffset
    return parent()->GetCharacterAtOffset(offset);
}

int TextAdaptor::GetCharacterExtents(int offset, uint coordType, int &y, int &width, int &height)
{
    // handle method call org.a11y.atspi.Text.GetCharacterExtents
    return parent()->GetCharacterExtents(offset, coordType, y, width, height);
}

QSpiAttributeSet TextAdaptor::GetDefaultAttributeSet()
{
    // handle method call org.a11y.atspi.Text.GetDefaultAttributeSet
    return parent()->GetDefaultAttributeSet();
}

QSpiAttributeSet TextAdaptor::GetDefaultAttributes()
{
    // handle method call org.a11y.atspi.Text.GetDefaultAttributes
    return parent()->GetDefaultAttributes();
}

int TextAdaptor::GetNSelections()
{
    // handle method call org.a11y.atspi.Text.GetNSelections
    return parent()->GetNSelections();
}

int TextAdaptor::GetOffsetAtPoint(int x, int y, uint coordType)
{
    // handle method call org.a11y.atspi.Text.GetOffsetAtPoint
    return parent()->GetOffsetAtPoint(x, y, coordType);
}

int TextAdaptor::GetRangeExtents(int startOffset, int endOffset, uint coordType, int &y, int &width, int &height)
{
    // handle method call org.a11y.atspi.Text.GetRangeExtents
    return parent()->GetRangeExtents(startOffset, endOffset, coordType, y, width, height);
}

int TextAdaptor::GetSelection(int selectionNum, int &endOffset)
{
    // handle method call org.a11y.atspi.Text.GetSelection
    return parent()->GetSelection(selectionNum, endOffset);
}

QString TextAdaptor::GetText(int startOffset, int endOffset)
{
    // handle method call org.a11y.atspi.Text.GetText
    return parent()->GetText(startOffset, endOffset);
}

QString TextAdaptor::GetTextAfterOffset(int offset, uint type, int &startOffset, int &endOffset)
{
    // handle method call org.a11y.atspi.Text.GetTextAfterOffset
    return parent()->GetTextAfterOffset(offset, type, startOffset, endOffset);
}

QString TextAdaptor::GetTextAtOffset(int offset, uint type, int &startOffset, int &endOffset)
{
    // handle method call org.a11y.atspi.Text.GetTextAtOffset
    return parent()->GetTextAtOffset(offset, type, startOffset, endOffset);
}

QString TextAdaptor::GetTextBeforeOffset(int offset, uint type, int &startOffset, int &endOffset)
{
    // handle method call org.a11y.atspi.Text.GetTextBeforeOffset
    return parent()->GetTextBeforeOffset(offset, type, startOffset, endOffset);
}

bool TextAdaptor::RemoveSelection(int selectionNum)
{
    // handle method call org.a11y.atspi.Text.RemoveSelection
    return parent()->RemoveSelection(selectionNum);
}

bool TextAdaptor::SetCaretOffset(int offset)
{
    // handle method call org.a11y.atspi.Text.SetCaretOffset
    return parent()->SetCaretOffset(offset);
}

bool TextAdaptor::SetSelection(int selectionNum, int startOffset, int endOffset)
{
    // handle method call org.a11y.atspi.Text.SetSelection
    return parent()->SetSelection(selectionNum, startOffset, endOffset);
}

