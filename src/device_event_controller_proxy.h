/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp org.freedesktop.atspi.DeviceEventController.xml -i proxy_marshallers.h -c QSpiDECProxy -p device_event_controller_proxy
 *
 * qdbusxml2cpp is Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef DEVICE_EVENT_CONTROLLER_PROXY_H_1259751713
#define DEVICE_EVENT_CONTROLLER_PROXY_H_1259751713

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include "proxy_marshallers.h"

/*
 * Proxy class for interface org.freedesktop.atspi.DeviceEventController
 */
class QSpiDECProxy: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.freedesktop.atspi.DeviceEventController"; }

public:
    QSpiDECProxy(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~QSpiDECProxy();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> NotifyListenersAsync(const QSpiDeviceEvent &event)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(event);
        return asyncCallWithArgumentList(QLatin1String("NotifyListenersAsync"), argumentList);
    }

    inline QDBusPendingReply<bool> NotifyListenersSync(const QSpiDeviceEvent &event)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(event);
        return asyncCallWithArgumentList(QLatin1String("NotifyListenersSync"), argumentList);
    }

Q_SIGNALS: // SIGNALS
};

namespace org {
  namespace freedesktop {
    namespace atspi {
      typedef ::QSpiDECProxy DeviceEventController;
    }
  }
}
#endif
