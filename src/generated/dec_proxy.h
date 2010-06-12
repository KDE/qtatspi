/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -i marshallers.h -p dec_proxy -c DECProxy ../../xml/DeviceEventController.xml
 *
 * qdbusxml2cpp is Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef DEC_PROXY_H_1266098988
#define DEC_PROXY_H_1266098988

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include "marshallers.h"

/*
 * Proxy class for interface org.a11y.atspi.DeviceEventController
 */
class DECProxy: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.a11y.atspi.DeviceEventController"; }

public:
    DECProxy(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~DECProxy();

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
  namespace a11y {
    namespace atspi {
      typedef ::DECProxy DeviceEventController;
    }
  }
}
#endif