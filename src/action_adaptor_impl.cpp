/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2008, 2009      Nokia.
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

#include "action_adaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

#include <QAccessibleTextInterface>

#define ACTION_INTERFACE static_cast <QSpiAccessibleObject *>(parent())->getInterface().actionInterface()

/*
 * Implementation of adaptor class QSpiActionAdaptor
 */

QSpiActionAdaptor::QSpiActionAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

QSpiActionAdaptor::~QSpiActionAdaptor()
{
    // destructor
}

int QSpiActionAdaptor::nActions() const
{
    return ACTION_INTERFACE->actionCount ();
}

bool QSpiActionAdaptor::DoAction(int index)
{
    ACTION_INTERFACE->doAction (index);
    return TRUE;
}

QSpiActionArray QSpiActionAdaptor::GetActions()
{
    // handle method call org.freedesktop.atspi.Action.GetActions
    QSpiActionArray index;
    for (int i = 0; i < ACTION_INTERFACE->actionCount(); i++)
    {
        QSpiAction action;
        QStringList keyBindings;

        action.name = ACTION_INTERFACE->name (i);
        action.description = ACTION_INTERFACE->description (i);

        keyBindings = ACTION_INTERFACE->keyBindings (i);

        if (keyBindings.length() > 0)
                action.keyBinding = keyBindings[0];
        else
                action.keyBinding = "";

        index << action;
    }
    return index;
}

QString QSpiActionAdaptor::GetDescription(int index)
{
    return ACTION_INTERFACE->description (index);
}

QString QSpiActionAdaptor::GetKeyBinding(int index)
{
    QStringList keyBindings;

    keyBindings = ACTION_INTERFACE->keyBindings (index);
    /* Might as well return the first key binding, what are the other options? */
    if (keyBindings.length() > 0)
        return keyBindings[0];
    else
        return "";
}

QString QSpiActionAdaptor::GetName(int index)
{
    // handle method call org.freedesktop.atspi.Action.GetName
    return ACTION_INTERFACE->name (index);
}

