/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -i marshallers.h -a cache_adaptor ../../xml/Cache.xml
 *
 * qdbusxml2cpp is Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef CACHE_ADAPTOR_H_1297240343
#define CACHE_ADAPTOR_H_1297240343

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
 * Adaptor class for interface org.a11y.atspi.Cache
 */
class CacheAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.a11y.atspi.Cache")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.a11y.atspi.Cache\">\n"
"    <method name=\"GetItems\">\n"
"      <arg direction=\"out\" type=\"a((so)(so)a(so)assusau)\" name=\"nodes\"/>\n"
"      <annotation value=\"QSpiAccessibleCacheArray\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\"/>\n"
"    </method>\n"
"    <signal name=\"AddAccessible\">\n"
"      <arg type=\"((so)(so)a(so)assusau)\" name=\"nodeAdded\"/>\n"
"      <annotation value=\"QSpiAccessibleCacheItem\" name=\"com.trolltech.QtDBus.QtTypeName.In0\"/>\n"
"    </signal>\n"
"    <signal name=\"RemoveAccessible\">\n"
"      <arg type=\"(so)\" name=\"nodeRemoved\"/>\n"
"      <annotation value=\"QSpiObjectReference\" name=\"com.trolltech.QtDBus.QtTypeName.In0\"/>\n"
"    </signal>\n"
"  </interface>\n"
        "")
public:
    CacheAdaptor(QObject *parent);
    virtual ~CacheAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    QSpiAccessibleCacheArray GetItems();
Q_SIGNALS: // SIGNALS
    void AddAccessible(const QSpiAccessibleCacheItem &nodeAdded);
    void RemoveAccessible(const QSpiObjectReference &nodeRemoved);
};

#endif
