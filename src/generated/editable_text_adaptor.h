/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -i marshallers.h -a editable_text_adaptor ../../xml/EditableText.xml
 *
 * qdbusxml2cpp is Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef EDITABLE_TEXT_ADAPTOR_H_1297240343
#define EDITABLE_TEXT_ADAPTOR_H_1297240343

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include "marshallers.h"
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;

/*
 * Adaptor class for interface org.a11y.atspi.EditableText
 */
class EditableTextAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.a11y.atspi.EditableText")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.a11y.atspi.EditableText\">\n"
"    <method name=\"SetTextContents\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"newContents\"/>\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"    </method>\n"
"    <method name=\"InsertText\">\n"
"      <arg direction=\"in\" type=\"i\" name=\"position\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"text\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"length\"/>\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"    </method>\n"
"    <method name=\"CopyText\">\n"
"      <arg direction=\"in\" type=\"i\" name=\"startPos\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"endPos\"/>\n"
"    </method>\n"
"    <method name=\"CutText\">\n"
"      <arg direction=\"in\" type=\"i\" name=\"startPos\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"endPos\"/>\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"    </method>\n"
"    <method name=\"DeleteText\">\n"
"      <arg direction=\"in\" type=\"i\" name=\"startPos\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"endPos\"/>\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"    </method>\n"
"    <method name=\"PasteText\">\n"
"      <arg direction=\"in\" type=\"i\" name=\"position\"/>\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    EditableTextAdaptor(QObject *parent);
    virtual ~EditableTextAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    void CopyText(int startPos, int endPos);
    bool CutText(int startPos, int endPos);
    bool DeleteText(int startPos, int endPos);
    bool InsertText(int position, const QString &text, int length);
    bool PasteText(int position);
    bool SetTextContents(const QString &newContents);
Q_SIGNALS: // SIGNALS
};

#endif
