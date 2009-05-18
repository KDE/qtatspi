/*
 * This file was generated by dbusxml2cpp version 0.6
 * Command line was: dbusxml2cpp -i adaptor_marshallers.h -c QSpiActionAdaptor -a action_adaptor org.freedesktop.atspi.Action.xml
 *
 * dbusxml2cpp is Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "action_adaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

#define ACCESSIBLE_INTERFACE static_cast <QSpiAccessibleObject *>(parent())->getInterface()

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
    // get the value of property nActions
    return ACCESSIBLE_INTERFACE.userActionCount (0);
}

bool QSpiActionAdaptor::doAction(int index)
{
    // handle method call org.freedesktop.atspi.Action.doAction
    return ACCESSIBLE_INTERFACE.doAction (index, 0, QVariantList ());
}

QSpiActionArray QSpiActionAdaptor::getActions()
{
    // handle method call org.freedesktop.atspi.Action.getActions
    QSpiActionArray index;
    for (int i = 0; i < ACCESSIBLE_INTERFACE.userActionCount(0); i++)
    {
        QSpiAction action;
        action.name = ACCESSIBLE_INTERFACE.actionText (i, QAccessible::Name, 0);
        action.description = ACCESSIBLE_INTERFACE.actionText (i, QAccessible::Description, 0);
        action.keyBinding = ACCESSIBLE_INTERFACE.actionText (i, QAccessible::Accelerator, 0);
        index << action;
    }
    return index;
}

QString QSpiActionAdaptor::getDescription(int index)
{
    // handle method call org.freedesktop.atspi.Action.getDescription
    return ACCESSIBLE_INTERFACE.actionText (index, QAccessible::Description, 0);
}

QString QSpiActionAdaptor::getKeyBinding(int index)
{
    // handle method call org.freedesktop.atspi.Action.getKeyBinding
    return ACCESSIBLE_INTERFACE.actionText (index, QAccessible::Accelerator, 0);
}

QString QSpiActionAdaptor::getName(int index)
{
    // handle method call org.freedesktop.atspi.Action.getName
    return ACCESSIBLE_INTERFACE.actionText (index, QAccessible::Name, 0);
}
