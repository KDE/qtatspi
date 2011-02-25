/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2009      Nokia.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <QDBusPendingReply>
#include <QDebug>

#include "accessible.h"
#include "application.h"
#include "cache.h"

#include "generated/accessible_adaptor.h"
#include "generated/application_adaptor.h"
#include "generated/socket_proxy.h"

#define QSPI_REGISTRY_NAME "org.a11y.atspi.Registry"


QSpiApplication::QSpiApplication(QSpiAccessibleCache  *cache,
                                 QAccessibleInterface *interface,
                                 QDBusConnection c)
    :QSpiAdaptor(cache, interface), dbusConnection(c)
{
    SocketProxy *registry;
    QDBusPendingReply <QSpiObjectReference> reply;

    reference = new QSpiObjectReference (dbusConnection.baseService(),
                                               QDBusObjectPath (QSPI_OBJECT_PATH_ROOT));

    new AccessibleAdaptor(this);
    supportedInterfaces << QSPI_INTERFACE_ACCESSIBLE;

    new ApplicationAdaptor(this);
    supportedInterfaces << QSPI_INTERFACE_APPLICATION;

    dbusConnection.registerObject(reference->path.path(),
                                                 this,
                                                 QDBusConnection::ExportAdaptors);

    /* Plug in to the desktop socket */
    registry = new SocketProxy (QSPI_REGISTRY_NAME,
                             QSPI_OBJECT_PATH_ROOT,
                             dbusConnection);
    reply = registry->Embed(getReference());
    reply.waitForFinished();
    if (reply.isValid ()) {
        const QSpiObjectReference &_socket = reply.value();
        accessibilityRegistry = new QSpiObjectReference(_socket);
    } else {
        accessibilityRegistry = new QSpiObjectReference();
        qDebug() << "Error in contacting registry";
        qDebug() << reply.error().name();
        qDebug() << reply.error().message();
    }
    delete registry;

    qApp->installEventFilter(this);
}

QSpiObjectReference &QSpiApplication::getParentReference() const
{
    return *accessibilityRegistry;
}

void QSpiApplication::accessibleEvent(QAccessible::Event event)
{
    qDebug() << "Event in QSpiApplication: " << QString::number(event, 16);
}

enum QSpiKeyEventType
{
      QSPI_KEY_EVENT_PRESS,
      QSPI_KEY_EVENT_RELEASE,
      QSPI_KEY_EVENT_LAST_DEFINED
};

bool QSpiApplication::eventFilter(QObject *obj, QEvent *event)
{
    if (!event->spontaneous())
        return false;

    switch (event->type()) {
        case QEvent::WindowActivate: {
        qDebug() << " Window activate: " << event->spontaneous() << obj;
        QSpiObject* a = cache->objectToAccessible(obj);
        QSpiAccessible* acc = static_cast<QSpiAccessible*>(a);
        acc->windowActivated();
        break;
    }

        case QEvent::KeyPress:
        case QEvent::KeyRelease: {
            QKeyEvent *keyEvent = static_cast <QKeyEvent *>(event);
            QSpiDeviceEvent de;

            if (event->type() == QEvent::KeyPress)
                de.type = QSPI_KEY_EVENT_PRESS;
            else
                de.type = QSPI_KEY_EVENT_RELEASE;

            de.id = keyEvent->nativeVirtualKey();
            de.hardwareCode = keyEvent->nativeScanCode();

            de.modifiers = keyEvent->nativeModifiers();
            de.timestamp = QDateTime::currentMSecsSinceEpoch();

            // FIXME: how to generate key strings?
            if (keyEvent->key() == Qt::Key_Tab) {
                de.text = "Tab";
            } else {
                de.text = keyEvent->text();
            }

            de.isText = !keyEvent->text().isEmpty();

            qDebug() << "Key event text: " << de.isText << " " << de.text
                     << " modifiers: " << keyEvent->modifiers()
                     << " hardware code: " << de.hardwareCode
                     << " native sc: " << keyEvent->nativeScanCode()
                     << " native mod: " << keyEvent->nativeModifiers()
                     << "native virt: " << keyEvent->nativeVirtualKey();

            QDBusMessage m = QDBusMessage::createMethodCall("org.a11y.atspi.Registry",
                                                            "/org/a11y/atspi/registry/deviceeventcontroller",
                                                            "org.a11y.atspi.DeviceEventController", "NotifyListenersSync");
            m.setArguments(QVariantList() <<QVariant::fromValue(de));
            QDBusMessage reply = dbusConnection.call(m);

            qDebug() << "QSpiApplication::eventFilter Key reply: " << reply.errorMessage() << reply.errorName() << reply.arguments();
            break;
    }
        default:
            break;
    }
    return false;
}
