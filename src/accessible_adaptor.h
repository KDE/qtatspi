/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -i adaptor_marshallers.h -c QSpiAccessibleAdaptor -a accessible_adaptor org.freedesktop.atspi.Accessible.xml
 *
 * qdbusxml2cpp is Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef ACCESSIBLE_ADAPTOR_H_1259750591
#define ACCESSIBLE_ADAPTOR_H_1259750591

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include "adaptor_marshallers.h"
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;

/*
 * Adaptor class for interface org.freedesktop.atspi.Accessible
 */
class QSpiAccessibleAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.atspi.Accessible")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.freedesktop.atspi.Accessible\" >\n"
"    <property access=\"read\" type=\"s\" name=\"name\" />\n"
"    <property access=\"read\" type=\"s\" name=\"description\" />\n"
"    <property access=\"read\" type=\"o\" name=\"parent\" />\n"
"    <property access=\"read\" type=\"i\" name=\"childCount\" />\n"
"    <method name=\"GetChildAtIndex\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"index\" />\n"
"      <arg direction=\"out\" type=\"(so)\" />\n"
"      <annotation value=\"QSpiObjectAddress\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\" />\n"
"    </method>\n"
"    <method name=\"GetIndexInParent\" >\n"
"      <arg direction=\"out\" type=\"i\" />\n"
"    </method>\n"
"    <method name=\"GetRelationSet\" >\n"
"      <arg direction=\"out\" type=\"a(ua(so))\" />\n"
"      <annotation value=\"QSpiRelationArray\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\" />\n"
"    </method>\n"
"    <method name=\"GetRole\" >\n"
"      <arg direction=\"out\" type=\"u\" />\n"
"    </method>\n"
"    <method name=\"GetRoleName\" >\n"
"      <arg direction=\"out\" type=\"s\" />\n"
"    </method>\n"
"    <method name=\"GetLocalizedRoleName\" >\n"
"      <arg direction=\"out\" type=\"s\" />\n"
"    </method>\n"
"    <method name=\"GetState\" >\n"
"      <arg direction=\"out\" type=\"au\" />\n"
"      <annotation value=\"QSpiIntList\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\" />\n"
"    </method>\n"
"    <method name=\"GetAttributes\" >\n"
"      <arg direction=\"out\" type=\"a{ss}\" />\n"
"      <annotation value=\"QSpiAttributeSet\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\" />\n"
"    </method>\n"
"    <method name=\"GetApplication\" >\n"
"      <arg direction=\"out\" type=\"(so)\" />\n"
"      <annotation value=\"QSpiObjectAddress\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\" />\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    QSpiAccessibleAdaptor(QObject *parent);
    virtual ~QSpiAccessibleAdaptor();

public: // PROPERTIES
    Q_PROPERTY(int childCount READ childCount)
    int childCount() const;

    Q_PROPERTY(QString description READ description)
    QString description() const;

    Q_PROPERTY(QString name READ name)
    QString name() const;

    Q_PROPERTY(QDBusObjectPath parent READ parent)
    QDBusObjectPath parent() const;

public Q_SLOTS: // METHODS
    QSpiObjectAddress GetApplication();
    QSpiAttributeSet GetAttributes();
    QSpiObjectAddress GetChildAtIndex(int index);
    int GetIndexInParent();
    QString GetLocalizedRoleName();
    QSpiRelationArray GetRelationSet();
    uint GetRole();
    QString GetRoleName();
    QSpiIntList GetState();
Q_SIGNALS: // SIGNALS
};

#endif
