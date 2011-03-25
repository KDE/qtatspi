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
#include <QtGui/QWidget>

#include "adaptor.h"
#include "accessible.h"
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
