/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -i adaptor.h -a dec_proxy -c DeviceEventControllerProxy ../../xml/DeviceEventController.xml
 *
 * qdbusxml2cpp is Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef DEC_PROXY_H_1298562874
#define DEC_PROXY_H_1298562874

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include "adaptor.h"
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;

/*
 * Adaptor class for interface org.a11y.atspi.DeviceEventController
 */
class DeviceEventControllerProxy: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.a11y.atspi.DeviceEventController")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.a11y.atspi.DeviceEventController\">\n"
"    <!--\n"
"  <method name=\"RegisterKeystrokeListener\">\n"
"    <arg direction=\"in\" name=\"listener\" type=\"o\"/>\n"
"    <arg direction=\"in\" name=\"keys\" type=\"a(iisi)\">\n"
"      <annotation name=\"com.trolltech.QtDBus.QtTypeName.In1\" value=\"QSpiKeyTypeArray\"/>\n"
"    </arg>\n"
"    <arg direction=\"in\" name=\"mask\" type=\"u\"/>\n"
"    <arg direction=\"in\" name=\"type\" type=\"au\">\n"
"      <annotation name=\"com.trolltech.QtDBus.QtTypeName.In3\" value=\"QSpiEventTypeArray\"/>\n"
"    </arg>\n"
"    <arg direction=\"in\" name=\"mode\" type=\"(bbb)\">\n"
"      <annotation name=\"com.trolltech.QtDBus.QtTypeName.In4\" value=\"QSpiEventMode\"/>\n"
"    </arg>\n"
"    <arg direction=\"out\" type=\"b\"/>\n"
"  </method>\n"
"\n"
"  <method name=\"DeregisterKeystrokeListener\">\n"
"    <arg direction=\"in\" name=\"listener\" type=\"o\"/>\n"
"    <arg direction=\"in\" name=\"keys\" type=\"a(iisi)\">\n"
"      <annotation name=\"com.trolltech.QtDBus.QtTypeName.In1\" value=\"QSpiKeyTypeArray\"/>\n"
"    </arg>\n"
"    <arg direction=\"in\" name=\"mask\" type=\"u\"/>\n"
"    <arg direction=\"in\" name=\"type\" type=\"u\"/>\n"
"  </method>\n"
"\n"
"  <method name=\"RegisterDeviceEventListener\">\n"
"    <arg direction=\"in\" name=\"listener\" type=\"o\"/>\n"
"    <arg direction=\"in\" name=\"types\" type=\"u\"/>\n"
"    <arg direction=\"out\" type=\"b\"/>\n"
"  </method>\n"
"\n"
"  <method name=\"DeregisterDeviceEventListener\">\n"
"    <arg direction=\"in\" name=\"listener\" type=\"o\"/>\n"
"    <arg direction=\"in\" name=\"types\" type=\"u\"/>\n"
"  </method>\n"
"\n"
"  <method name=\"GenerateKeyboardEvent\">\n"
"    <arg direction=\"in\" name=\"keycode\" type=\"i\"/>\n"
"    <arg direction=\"in\" name=\"keystring\" type=\"s\"/>\n"
"    <arg direction=\"in\" name=\"type\" type=\"u\"/>\n"
"  </method>\n"
"\n"
"  <method name=\"GenerateMouseEvent\">\n"
"    <arg direction=\"in\" name=\"x\" type=\"i\"/>\n"
"    <arg direction=\"in\" name=\"y\" type=\"i\"/>\n"
"    <arg direction=\"in\" name=\"eventName\" type=\"s\"/>\n"
"  </method>\n"
"-->\n"
"    <method name=\"NotifyListenersSync\">\n"
"      <arg direction=\"in\" type=\"(uinnisb)\" name=\"event\"/>\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"      <annotation value=\"QSpiDeviceEvent\" name=\"com.trolltech.QtDBus.QtTypeName.In0\"/>\n"
"    </method>\n"
"    <method name=\"NotifyListenersAsync\">\n"
"      <arg direction=\"in\" type=\"(uinnisb)\" name=\"event\"/>\n"
"      <annotation value=\"QSpiDeviceEvent\" name=\"com.trolltech.QtDBus.QtTypeName.In0\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    DeviceEventControllerProxy(QObject *parent);
    virtual ~DeviceEventControllerProxy();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    void NotifyListenersAsync(const QSpiDeviceEvent &event);
    bool NotifyListenersSync(const QSpiDeviceEvent &event);
Q_SIGNALS: // SIGNALS
};

#endif
