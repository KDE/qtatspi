/*
 * This file was generated by dbusxml2cpp version 0.6
 * Command line was: dbusxml2cpp -i adaptor_marshallers.h -c QSpiActionAdaptor -a action_adaptor org.freedesktop.atspi.Action.xml
 *
 * dbusxml2cpp is Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef ACTION_ADAPTOR_H_1239786631
#define ACTION_ADAPTOR_H_1239786631

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
 * Adaptor class for interface org.freedesktop.atspi.Action
 */
class QSpiActionAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.atspi.Action")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.freedesktop.atspi.Action\" >\n"
"    <property access=\"read\" type=\"i\" name=\"nActions\" />\n"
"    <method name=\"getDescription\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"index\" />\n"
"      <arg direction=\"out\" type=\"s\" />\n"
"    </method>\n"
"    <method name=\"getName\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"index\" />\n"
"      <arg direction=\"out\" type=\"s\" />\n"
"    </method>\n"
"    <method name=\"getKeyBinding\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"index\" />\n"
"      <arg direction=\"out\" type=\"s\" />\n"
"    </method>\n"
"    <method name=\"getActions\" >\n"
"      <arg direction=\"out\" type=\"a(sss)\" name=\"index\" />\n"
"      <annotation value=\"QSpiActionArray\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\" />\n"
"    </method>\n"
"    <method name=\"doAction\" >\n"
"      <arg direction=\"in\" type=\"i\" name=\"index\" />\n"
"      <arg direction=\"out\" type=\"b\" />\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    QSpiActionAdaptor(QObject *parent);
    virtual ~QSpiActionAdaptor();

public: // PROPERTIES
    Q_PROPERTY(int nActions READ nActions)
    int nActions() const;

public Q_SLOTS: // METHODS
    bool doAction(int index);
    QSpiActionArray getActions();
    QString getDescription(int index);
    QString getKeyBinding(int index);
    QString getName(int index);
Q_SIGNALS: // SIGNALS
};

#endif
