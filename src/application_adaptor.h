/*
 * This file was generated by dbusxml2cpp version 0.6
 * Command line was: dbusxml2cpp -i adaptor_marshallers.h -c QSpiApplicationAdaptor -a application_adaptor org.freedesktop.atspi.Application.xml
 *
 * dbusxml2cpp is Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef APPLICATION_ADAPTOR_H_1239786188
#define APPLICATION_ADAPTOR_H_1239786188

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
 * Adaptor class for interface org.freedesktop.atspi.Application
 */
class QSpiApplicationAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.atspi.Application")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.freedesktop.atspi.Application\" >\n"
"    <property access=\"read\" type=\"s\" name=\"toolkitName\" />\n"
"    <property access=\"read\" type=\"s\" name=\"version\" />\n"
"    <property access=\"readwrite\" type=\"i\" name=\"id\" />\n"
"    <method name=\"registerToolkitEventListener\" >\n"
"      <arg direction=\"in\" type=\"o\" name=\"listener\" />\n"
"      <arg direction=\"in\" type=\"s\" name=\"eventName\" />\n"
"    </method>\n"
"    <method name=\"registerObjectEventListener\" >\n"
"      <arg direction=\"in\" type=\"o\" name=\"listener\" />\n"
"      <arg direction=\"in\" type=\"s\" name=\"eventName\" />\n"
"    </method>\n"
"    <method name=\"pause\" >\n"
"      <arg direction=\"out\" type=\"b\" />\n"
"    </method>\n"
"    <method name=\"resume\" >\n"
"      <arg direction=\"out\" type=\"b\" />\n"
"    </method>\n"
"    <method name=\"getLocale\" >\n"
"      <arg direction=\"in\" type=\"u\" name=\"lctype\" />\n"
"      <arg direction=\"out\" type=\"s\" />\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    QSpiApplicationAdaptor(QObject *parent);
    virtual ~QSpiApplicationAdaptor();

public: // PROPERTIES
    Q_PROPERTY(int id READ id)
    int id() const;

    Q_PROPERTY(QString toolkitName READ toolkitName)
    QString toolkitName() const;

    Q_PROPERTY(QString version READ version)
    QString version() const;

public Q_SLOTS: // METHODS
    QString getLocale(uint lctype);
    bool pause();
    bool resume();
Q_SIGNALS: // SIGNALS
};

#endif
