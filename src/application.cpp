/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2009-2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "application.h"

#include "qapplication.h"
#include <QDBusPendingReply>
#include <QDebug>

#include "generated/dec_proxy.h"

//#define KEYBOARD_DEBUG


/*!
    \class QSpiApplicationAdaptor

    \brief QSpiApplicationAdaptor

    QSpiApplicationAdaptor
*/

QSpiApplicationAdaptor::QSpiApplicationAdaptor(const QDBusConnection &connection, QObject *parent)
    : QObject(parent), dbusConnection(connection)
{
    qApp->installEventFilter(this);
}

enum QSpiKeyEventType {
      QSPI_KEY_EVENT_PRESS,
      QSPI_KEY_EVENT_RELEASE,
      QSPI_KEY_EVENT_LAST_DEFINED
};

bool QSpiApplicationAdaptor::eventFilter(QObject *target, QEvent *event)
{
    if (!event->spontaneous())
        return false;

    switch (event->type()) {
    case QEvent::WindowActivate: {
        emit windowActivated(target, true);
        break;
    case QEvent::WindowDeactivate:
            emit windowActivated(target, false);
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

#ifdef KEYBOARD_DEBUG
            qDebug() << "Key event text: " << event->type() << de.isText << " " << de.text
                     << " hardware code: " << de.hardwareCode
                     << " native sc: " << keyEvent->nativeScanCode()
                     << " native mod: " << keyEvent->nativeModifiers()
                     << "native virt: " << keyEvent->nativeVirtualKey();
#endif

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

QKeyEvent* QSpiApplicationAdaptor::copyKeyEvent(QKeyEvent* old)
{
    return new QKeyEvent(old->type(), old->key(), old->modifiers(), old->text(), old->isAutoRepeat(), old->count());
}

void QSpiApplicationAdaptor::notifyKeyboardListenerCallback(const QDBusMessage& message)
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

void QSpiApplicationAdaptor::notifyKeyboardListenerError(const QDBusError& error, const QDBusMessage& /*message*/)
{
    qWarning() << "QSpiApplication::keyEventError " << error.name() << error.message();
    while (!keyEvents.isEmpty()) {
        QPair<QObject*, QKeyEvent*> event = keyEvents.dequeue();
        QApplication::postEvent(event.first, event.second);
    }
}
