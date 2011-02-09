/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -i marshallers.h -a accessible_adaptor ../../xml/Accessible.xml
 *
 * qdbusxml2cpp is Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef ACCESSIBLE_ADAPTOR_H_1297240343
#define ACCESSIBLE_ADAPTOR_H_1297240343

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
 * Adaptor class for interface org.a11y.atspi.Accessible
 */
class AccessibleAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.a11y.atspi.Accessible")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.a11y.atspi.Accessible\">\n"
"    <property access=\"read\" type=\"s\" name=\"Name\"/>\n"
"    <property access=\"read\" type=\"s\" name=\"Description\"/>\n"
"    <property access=\"read\" type=\"(so)\" name=\"Parent\">\n"
"      <annotation value=\"QSpiObjectReference\" name=\"com.trolltech.QtDBus.QtTypeName\"/>\n"
"    </property>\n"
"    <property access=\"read\" type=\"i\" name=\"ChildCount\"/>\n"
"    <method name=\"GetChildAtIndex\">\n"
"      <arg direction=\"in\" type=\"i\" name=\"index\"/>\n"
"      <arg direction=\"out\" type=\"(so)\"/>\n"
"      <annotation value=\"QSpiObjectReference\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
"    </method>\n"
"    <method name=\"GetChildren\">\n"
"      <arg direction=\"out\" type=\"a(so)\"/>\n"
"      <annotation value=\"QSpiObjectReferenceArray\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
"    </method>\n"
"    <method name=\"GetIndexInParent\">\n"
"      <arg direction=\"out\" type=\"i\"/>\n"
"    </method>\n"
"    <method name=\"GetRelationSet\">\n"
"      <arg direction=\"out\" type=\"a(ua(so))\"/>\n"
"      <annotation value=\"QSpiRelationArray\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
"    </method>\n"
"    <method name=\"GetRole\">\n"
"      <arg direction=\"out\" type=\"u\"/>\n"
"    </method>\n"
"    <method name=\"GetRoleName\">\n"
"      <arg direction=\"out\" type=\"s\"/>\n"
"    </method>\n"
"    <method name=\"GetLocalizedRoleName\">\n"
"      <arg direction=\"out\" type=\"s\"/>\n"
"    </method>\n"
"    <method name=\"GetState\">\n"
"      <arg direction=\"out\" type=\"au\"/>\n"
"      <annotation value=\"QSpiIntList\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
"    </method>\n"
"    <method name=\"GetAttributes\">\n"
"      <arg direction=\"out\" type=\"a{ss}\"/>\n"
"      <annotation value=\"QSpiAttributeSet\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
"    </method>\n"
"    <method name=\"GetApplication\">\n"
"      <arg direction=\"out\" type=\"(so)\"/>\n"
"      <annotation value=\"QSpiObjectReference\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    AccessibleAdaptor(QObject *parent);
    virtual ~AccessibleAdaptor();

public: // PROPERTIES
    Q_PROPERTY(int ChildCount READ childCount)
    int childCount() const;

    Q_PROPERTY(QString Description READ description)
    QString description() const;

    Q_PROPERTY(QString Name READ name)
    QString name() const;

    Q_PROPERTY(QSpiObjectReference Parent READ getParent)
    QSpiObjectReference getParent() const;

public Q_SLOTS: // METHODS
    QSpiObjectReference GetApplication();
    QSpiAttributeSet GetAttributes();
    QSpiObjectReference GetChildAtIndex(int index);
    QSpiObjectReferenceArray GetChildren();
    int GetIndexInParent();
    QString GetLocalizedRoleName();
    QSpiRelationArray GetRelationSet();
    uint GetRole();
    QString GetRoleName();
    QSpiIntList GetState();
Q_SIGNALS: // SIGNALS
};

#endif
