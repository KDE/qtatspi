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

#include "accessible.h"

#include <QDBusPendingReply>
#include <QDebug>
#include <QtGui/QWidget>

#include <QAccessibleValueInterface>

#include "adaptor.h"
#include "bridge.h"
#include "cache.h"

#include "generated/accessible_adaptor.h"
#include "generated/action_adaptor.h"
#include "generated/component_adaptor.h"
#include "generated/editable_text_adaptor.h"
#include "generated/event_adaptor.h"
#include "generated/socket_proxy.h"
#include "generated/table_adaptor.h"
#include "generated/text_adaptor.h"
#include "generated/value_adaptor.h"

#define QSPI_REGISTRY_NAME "org.a11y.atspi.Registry"


QDBusObjectPath QSpiAccessible::getUnique()
{
    static int id = 1;
    QString prefix(QSPI_OBJECT_PATH_PREFIX);
    QString num;

    if (id == 0)
       id++;
    return QDBusObjectPath(prefix + num.setNum(id++));
}

QSpiAccessible::QSpiAccessible(QAccessibleInterface *interface, int index)
    : QSpiAdaptor(interface, index)
{
    reference = QSpiObjectReference(spiBridge->dBusConnection(),
                                               getUnique());

    qDebug() << "ACCESSIBLE: " << interface->object() << reference.path.path();

    new AccessibleAdaptor(this);
    supportedInterfaces << QSPI_INTERFACE_ACCESSIBLE;

    if ((!interface->rect(index).isEmpty()) || (interface->object() && interface->object()->isWidgetType())) {
        new ComponentAdaptor(this);
        supportedInterfaces << QSPI_INTERFACE_COMPONENT;

        if (interface->object() && interface->object()->isWidgetType()) {
            QWidget *w = qobject_cast<QWidget*>(interface->object());
            if (w->isWindow()) {
                new WindowAdaptor(this);
                qDebug() << " + window";
            }
        }
    } else {
        qDebug() << " - component";
    }

    new ObjectAdaptor(this);
    new FocusAdaptor(this);

    if (interface->actionInterface())
    {
        new ActionAdaptor(this);
        supportedInterfaces << QSPI_INTERFACE_ACTION;
    }
    if (interface->textInterface())
    {
        new TextAdaptor(this);
        supportedInterfaces << QSPI_INTERFACE_TEXT;
        oldText = interface->textInterface()->text(0, interface->textInterface()->characterCount());
        qDebug() << "Old Text: " << interface->object() << oldText;
    }
    if (interface->editableTextInterface())
    {
        new EditableTextAdaptor(this);
        supportedInterfaces << QSPI_INTERFACE_EDITABLE_TEXT;
    }
    if (interface->valueInterface())
    {
        new ValueAdaptor(this);
        supportedInterfaces << QSPI_INTERFACE_VALUE;
    }
    if (interface->tableInterface())
    {
        new TableAdaptor(this);
        supportedInterfaces << QSPI_INTERFACE_TABLE;
    }

    spiBridge->dBusConnection().registerObject(reference.path.path(),
                                  this, QDBusConnection::ExportAdaptors);
    state = interface->state(childIndex());
}

QSpiObjectReference QSpiAccessible::getParentReference() const
{
    Q_ASSERT(interface);

    if (interface->isValid()) {
        QAccessibleInterface *parentInterface = 0;
        interface->navigate(QAccessible::Ancestor, 1, &parentInterface);
        if (parentInterface)
        {
            QSpiAdaptor *parent = spiBridge->objectToAccessible(parentInterface->object());
            delete parentInterface;
            if (parent)
                return parent->getReference();
        }
    }
    return QSpiObjectReference();
}

void QSpiAccessible::accessibleEvent(QAccessible::Event event)
{
    Q_ASSERT(interface);
    if (!interface->isValid()) {
        spiBridge->removeAdaptor(this);
        return;
    }

    switch (event) {
    case QAccessible::NameChanged: {
        QSpiObjectReference r = getReference();
        QDBusVariant data;
        data.setVariant(QVariant::fromValue(r));
        emit PropertyChange("accessible-name", 0, 0, data, spiBridge->getRootReference());
        break;
    }
    case QAccessible::DescriptionChanged: {
        QSpiObjectReference r = getReference();
        QDBusVariant data;
        data.setVariant(QVariant::fromValue(r));
        emit PropertyChange("accessible-description", 0, 0, data, spiBridge->getRootReference());
        break;
    }
    case QAccessible::Focus: {
        QDBusVariant data;
        data.setVariant(QVariant::fromValue(getReference()));
        emit StateChanged("focused", 1, 0, data, spiBridge->getRootReference());
        emit Focus("", 0, 0, data, spiBridge->getRootReference());
        break;
    }
#if (QT_VERSION >= QT_VERSION_CHECK(4, 8, 0))
    case QAccessible::TextUpdated: {
        Q_ASSERT(interface->textInterface());

        // at-spi doesn't have a proper text updated/changed, so remove all and re-add the new text
        qDebug() << "Text changed: " << interface->object();
        QDBusVariant data;
        data.setVariant(QVariant::fromValue(oldText));
        emit TextChanged("delete", 0, oldText.length(), data, spiBridge->getRootReference());

        QString text = interface->textInterface()->text(0, interface->textInterface()->characterCount());
        data.setVariant(QVariant::fromValue(text));
        emit TextChanged("insert", 0, text.length(), data, spiBridge->getRootReference());
        oldText = text;

        QDBusVariant cursorData;
        int pos = interface->textInterface()->cursorPosition();
        cursorData.setVariant(QVariant::fromValue(pos));
        emit TextCaretMoved(QString(), pos ,0, cursorData, spiBridge->getRootReference());
        break;
    }
    case QAccessible::TextCaretMoved: {
        Q_ASSERT(interface->textInterface());
        qDebug() << "Text caret moved: " << interface->object();
        QDBusVariant data;
        int pos = interface->textInterface()->cursorPosition();
        data.setVariant(QVariant::fromValue(pos));
        emit TextCaretMoved(QString(), pos ,0, data, spiBridge->getRootReference());
        break;
    }
#endif
    case QAccessible::ValueChanged: {
        Q_ASSERT(interface->valueInterface());
        qDebug() << "Value changed: " << interface->object() << interface->valueInterface()->currentValue();
        QSpiObjectReference r = getReference();
        QDBusVariant data;
        data.setVariant(QVariant::fromValue(r));
        // FIXME: values for data1/2?
        emit PropertyChange("value-changed", 0, 0, data, spiBridge->getRootReference());
        break;
    }
    case QAccessible::ObjectShow:
        break;
    case QAccessible::ObjectHide:
        // TODO
        qWarning() << "Object hide";
        break;
    case QAccessible::ObjectDestroyed:
        qWarning() << "Object destroyed";
        Q_ASSERT(0);
        break;
    case QAccessible::StateChanged: {
        QAccessible::State newState = interface->state(childIndex());
        qDebug() << "StateChanged: old: " << state << " new: " << newState << " xor: " << (state^newState);
        if ((state^newState) & QAccessible::Checked) {
            int checked = (newState & QAccessible::Checked) ? 1 : 0;
            QDBusVariant data;
            data.setVariant(QVariant::fromValue(getReference()));
            emit StateChanged("checked", checked, 0, data, spiBridge->getRootReference());
        }
        state = newState;
        break;
    }
    case QAccessible::ParentChanged:
    default:
//        qWarning() << "QSpiAccessible::accessibleEvent not handled: " << QString::number(event, 16)
//                   << " obj: " << interface->object()
//                   << (interface->isValid() ? interface->object()->objectName() : " invalid interface!");
        break;
    }
}

void QSpiAccessible::windowActivated()
{
    QDBusVariant data;
    data.setVariant(QString());
    emit Create("", 0, 0, data, spiBridge->getRootReference());
    emit Restore("", 0, 0, data, spiBridge->getRootReference());
    emit Activate("", 0, 0, data, spiBridge->getRootReference());
}
