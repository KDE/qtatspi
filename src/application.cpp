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
#include "bridge.h"
#include "cache.h"

#include "generated/accessible_adaptor.h"
#include "generated/application_adaptor.h"
#include "generated/socket_proxy.h"

#define QSPI_REGISTRY_NAME "org.a11y.atspi.Registry"

QSpiApplication::QSpiApplication(const QDBusConnection& c, QAccessibleInterface *interface)
    : QSpiAdaptor(interface, 0), dbusConnection(c), applicationId(-1)
{
    reference = QSpiObjectReference(dbusConnection,
                   QDBusObjectPath(QSPI_OBJECT_PATH_ROOT));

    new AccessibleAdaptor(this);
    supportedInterfaces << QSPI_INTERFACE_ACCESSIBLE;

    new ApplicationAdaptor(this);
    supportedInterfaces << QSPI_INTERFACE_APPLICATION;

    dbusConnection.registerObject(reference.path.path(),
                                  this, QDBusConnection::ExportAdaptors);

    callAccessibilityRegistry();
    qApp->installEventFilter(this);
}

void QSpiApplication::callAccessibilityRegistry()
{
    SocketProxy *registry;
    registry = new SocketProxy(QSPI_REGISTRY_NAME,
                               QSPI_OBJECT_PATH_ROOT, dbusConnection);

    QDBusPendingReply<QSpiObjectReference> reply;
    reply = registry->Embed(getReference());
    reply.waitForFinished();
    if (reply.isValid ()) {
        const QSpiObjectReference &socket = reply.value();
        accessibilityRegistry = QSpiObjectReference(socket);
    } else {
        qDebug() << "Error in contacting registry";
        qDebug() << reply.error().name();
        qDebug() << reply.error().message();
    }
    delete registry;
}

QSpiObjectReference QSpiApplication::getParentReference() const
{
    return accessibilityRegistry;
}

void QSpiApplication::accessibleEvent(QAccessible::Event event)
{
    qDebug() << "Event in QSpiApplication: " << QString::number(event, 16);
}

enum QSpiKeyEventType {
      QSPI_KEY_EVENT_PRESS,
      QSPI_KEY_EVENT_RELEASE,
      QSPI_KEY_EVENT_LAST_DEFINED
};

bool QSpiApplication::eventFilter(QObject *target, QEvent *event)
{
    if (!event->spontaneous())
        return false;

    switch (event->type()) {
        case QEvent::WindowActivate: {
        emit windowActivated(target);
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
            // FIXME: localize?
            if (keyEvent->key() == Qt::Key_Tab) {
                de.text = "Tab";
            } else if (keyEvent->key() == Qt::Key_Backtab) {
                de.text = "Backtab";
            } else if (keyEvent->key() == Qt::Key_Left) {
                de.text = "Left";
            } else if (keyEvent->key() == Qt::Key_Right) {
                de.text = "Right";
            } else if (keyEvent->key() == Qt::Key_Up) {
                de.text = "Up";
            } else if (keyEvent->key() == Qt::Key_Down) {
                de.text = "Down";
            } else if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
                de.text = "Return";
            } else if (keyEvent->key() == Qt::Key_Backspace) {
                de.text = "BackSpace";
            } else if (keyEvent->key() == Qt::Key_Delete) {
                de.text = "Delete";
            } else if (keyEvent->key() == Qt::Key_PageUp) {
                de.text = "Page_Up";
            } else if (keyEvent->key() == Qt::Key_PageDown) {
                de.text = "Page_Down";
            } else if (keyEvent->key() == Qt::Key_Home) {
                de.text = "Home";
            } else if (keyEvent->key() == Qt::Key_End) {
                de.text = "End";
            } else if (keyEvent->key() == Qt::Key_Escape) {
                de.text = "Escape";
            } else if (keyEvent->key() == Qt::Key_Space) {
                de.text = "space";
            } else if (keyEvent->key() == Qt::Key_CapsLock) {
                de.text = "Caps_Lock";
            } else if (keyEvent->key() == Qt::Key_NumLock) {
                de.text = "Num_Lock";
            } else {
                de.text = keyEvent->text();
            }
//            "F1", "F2", "F3", "F4", "F5", "F6",
//            "F7", "F8", "F9", "F10", "F11", "F12"

            // FIXME
            de.isText = !keyEvent->text().trimmed().isEmpty();

            qDebug() << "Key event text: " << event->type() << de.isText << " " << de.text
                     << " hardware code: " << de.hardwareCode
                     << " native sc: " << keyEvent->nativeScanCode()
                     << " native mod: " << keyEvent->nativeModifiers()
                     << "native virt: " << keyEvent->nativeVirtualKey();

            QDBusMessage m = QDBusMessage::createMethodCall("org.a11y.atspi.Registry",
                                                            "/org/a11y/atspi/registry/deviceeventcontroller",
                                                            "org.a11y.atspi.DeviceEventController", "NotifyListenersSync");
            m.setArguments(QVariantList() <<QVariant::fromValue(de));

            // FIXME: this is critical, the timeout should probably be pretty low to allow normal processing
            int timeout = 100;
            bool sent = dbusConnection.callWithCallback(m, this, SLOT(notifyKeyboardListenerCallback(QDBusMessage)),
                                                        SLOT(notifyKeyboardListenerError(QDBusError, QDBusMessage)), timeout);
            if (!sent)
                return false;

            //queue the event and send it after callback
            keyEvents.enqueue(QPair<QObject*, QKeyEvent*> (target, copyKeyEvent(keyEvent)));
            return true;
    }
        default:
            break;
    }
    return false;
}

QKeyEvent* QSpiApplication::copyKeyEvent(QKeyEvent* old)
{
    return new QKeyEvent(old->type(), old->key(), old->modifiers(), old->text(), old->isAutoRepeat(), old->count());
}

void QSpiApplication::notifyKeyboardListenerCallback(const QDBusMessage& message)
{
    Q_ASSERT(message.arguments().length() == 1);
    if (message.arguments().at(0).toBool() == true) {
        if (!keyEvents.length()) {
            qWarning() << "QSpiApplication::notifyKeyboardListenerCallback with no queued key called";
            return;
        }
        keyEvents.dequeue();
    } else {
        if (!keyEvents.length()) {
            qWarning() << "QSpiApplication::notifyKeyboardListenerCallback with no queued key called";
            return;
        }
        QPair<QObject*, QKeyEvent*> event = keyEvents.dequeue();
        QApplication::postEvent(event.first, event.second);
    }
}

void QSpiApplication::notifyKeyboardListenerError(const QDBusError& error, const QDBusMessage& /*message*/)
{
    qWarning() << "QSpiApplication::keyEventError " << error.name() << error.message();
    while (!keyEvents.isEmpty()) {
        QPair<QObject*, QKeyEvent*> event = keyEvents.dequeue();
        QApplication::postEvent(event.first, event.second);
    }
}

int QSpiApplication::id() const
{
    return applicationId;
}

void QSpiApplication::setId(int value)
{
    applicationId = value;
}
