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

QSpiAccessible::QSpiAccessible(QSpiAccessibleCache  *cache,
                               QAccessibleInterface *interface,
                               QDBusConnection c)
    : QSpiAdaptor(cache, interface), dbusConnection(c)
{
    reference = new QSpiObjectReference(dbusConnection.baseService(),
                                               getUnique());

    new AccessibleAdaptor(this);
    supportedInterfaces << QSPI_INTERFACE_ACCESSIBLE;

    if (interface->object()->isWidgetType()) {
        qDebug() << "ComponentAdaptor for: " << interface->object();
        new ComponentAdaptor(this);
        supportedInterfaces << QSPI_INTERFACE_COMPONENT;

        QWidget *w = qobject_cast<QWidget*>(interface->object());
        if (w->isWindow()) {
            new WindowAdaptor(this);
            qDebug() << "Created Window adaptor for: " << interface->object();
        }

    } else {
        qDebug() << "NO ComponentAdaptor for: " << interface->object();
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

    dbusConnection.registerObject(reference->path.path(),
                                                 this,
                                                 QDBusConnection::ExportAdaptors);
}

QSpiObjectReference &QSpiAccessible::getParentReference() const
{
    QAccessibleInterface *parentInterface = NULL;
    QSpiObject *parent;

    interface->navigate(QAccessible::Ancestor, 1, &parentInterface);
    if (parentInterface)
    {
        parent = cache->objectToAccessible(parentInterface->object());
        delete parentInterface;
        if (parent)
        {
           return parent->getReference();
        }
    }

    static QSpiObjectReference null_reference(dbusConnection.baseService(),
                                              QDBusObjectPath(QSPI_OBJECT_PATH_NULL));

    return null_reference;
}

QSpiObjectReference QSpiAccessible::getRootReference() const
{
    return QSpiObjectReference(dbusConnection.baseService(), QDBusObjectPath(QSPI_OBJECT_PATH_ROOT));
}

void QSpiAccessible::signalChildrenChanged(const QString &type, int detail1, int detail2, const QDBusVariant &data)
{
    emit ChildrenChanged(type, detail1, detail2, data, getRootReference());
}

void QSpiAccessible::accessibleEvent(QAccessible::Event event)
{
    switch (event) {
    case QAccessible::NameChanged: {
        QSpiObjectReference r = getReference();
        QDBusVariant data;
        data.setVariant(QVariant::fromValue(r));
        emit PropertyChange("accessible-name", 0, 0, data, getRootReference());
        break;
    }
    case QAccessible::DescriptionChanged: {
        QSpiObjectReference r = getReference();
        QDBusVariant data;
        data.setVariant(QVariant::fromValue(r));
        emit PropertyChange("accessible-description", 0, 0, data, getRootReference());
        break;
    }
    case QAccessible::Focus: {
        qDebug() << "Focus event";
        QDBusVariant data;
        data.setVariant(QVariant::fromValue(getReference()));
        emit StateChanged("focused", 1, 0, data, getRootReference());
        emit Focus("", 0, 0, data, getRootReference());
        break;
    }
    case QAccessible::ObjectShow:
        // handled in bridge
        break;
    case QAccessible::ObjectHide:
    case QAccessible::ParentChanged:
    case QAccessible::StateChanged:
    default:
        qWarning() << "QSpiAccessible::accessibleEvent not handled: " << QString::number(event, 16)
                   << " obj: " << interface->object()
                   << interface->object()->objectName() ;
        break;
    }
}

void QSpiAccessible::windowActivated()
{
    QDBusVariant data;
    data.setVariant(QString());
    emit Create("", 0, 0, data, getRootReference());
    emit Restore("", 0, 0, data, getRootReference());
    emit Activate("", 0, 0, data, getRootReference());
}
