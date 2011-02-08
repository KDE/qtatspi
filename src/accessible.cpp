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

#include "adaptor.h"
#include "accessible.h"
#include "cache.h"

#include "constant_mappings.h"
#include "struct_marshallers.h"

#include "generated/accessible_adaptor.h"
#include "generated/application_adaptor.h"
#include "generated/text_adaptor.h"
#include "generated/editable_text_adaptor.h"
#include "generated/value_adaptor.h"
#include "generated/component_adaptor.h"
#include "generated/action_adaptor.h"
#include "generated/table_adaptor.h"
#include "generated/socket_proxy.h"

#define QSPI_REGISTRY_NAME "org.a11y.atspi.Registry"

static QSpiObjectReference null_reference (QDBusConnection::sessionBus().baseService(),
                                           QDBusObjectPath (QSPI_OBJECT_PATH_NULL));

QDBusObjectPath QSpiAccessible::getUnique ()
{
    static int id = 1;
    QString prefix(QSPI_OBJECT_PATH_PREFIX);
    QString num;

    if (id == 0)
       id++;
    return QDBusObjectPath(prefix + num.setNum(id++));
}

QSpiAccessible::QSpiAccessible(QSpiAccessibleCache  *cache,
                               QAccessibleInterface *interface)
    :QSpiAdaptor (cache, interface)
{
    reference = new QSpiObjectReference(QDBusConnection::sessionBus().baseService(),
                                               getUnique());

    new AccessibleAdaptor(this);
    supportedInterfaces << QSPI_INTERFACE_ACCESSIBLE;
    new ComponentAdaptor(this);
    supportedInterfaces << QSPI_INTERFACE_COMPONENT;

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

    QDBusConnection::sessionBus().registerObject(reference->path.path(),
                                                 this,
                                                 QDBusConnection::ExportAdaptors);
}

QSpiObjectReference &QSpiAccessible::getParentReference() const
{
    QAccessibleInterface *parentInterface = NULL;
    QSpiObject *parent;

    interface->navigate (QAccessible::Ancestor, 1, &parentInterface);
    if (parentInterface)
    {
        parent = cache->objectToAccessible(parentInterface->object());
        delete parentInterface;
        if (parent)
        {
           return parent->getReference();
        }
    }

    return null_reference;
}

/* QSpiApplication ------------------------------------------------*/

QSpiApplication::QSpiApplication (QSpiAccessibleCache  *cache,
                                  QAccessibleInterface *interface)
    :QSpiAdaptor (cache, interface)
{
    SocketProxy *proxy;
    ApplicationAdaptor *app;
    QDBusPendingReply <QSpiObjectReference> reply;

    reference = new QSpiObjectReference (QDBusConnection::sessionBus().baseService(),
                                               QDBusObjectPath (QSPI_OBJECT_PATH_ROOT));

    new AccessibleAdaptor(this);
    supportedInterfaces << QSPI_INTERFACE_ACCESSIBLE;
    new ComponentAdaptor(this);
    supportedInterfaces << QSPI_INTERFACE_COMPONENT;
    app = new ApplicationAdaptor(this);
    supportedInterfaces << QSPI_INTERFACE_APPLICATION;

    QDBusConnection::sessionBus().registerObject(reference->path.path(),
                                                 this,
                                                 QDBusConnection::ExportAdaptors);

    /* Plug in to the desktop socket */
    proxy = new SocketProxy (QSPI_REGISTRY_NAME,
                             QSPI_OBJECT_PATH_ROOT,
                             QDBusConnection::sessionBus());
    reply = proxy->Embed(getReference());
    reply.waitForFinished();
    if (reply.isValid ()) {
        const QSpiObjectReference &_socket = reply.value();
        socket = new QSpiObjectReference(_socket);
    } else {
        socket = new QSpiObjectReference();
        qDebug() << "Error in contacting registry";
        qDebug() << reply.error().name();
        qDebug() << reply.error().message();
    }
    delete proxy;
}

QSpiObjectReference &QSpiApplication::getParentReference() const
{
    return *socket;
}
