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

bool QSpiActionAdaptor::doAction(int index)
{
    ACTION_INTERFACE->doAction (index);
    return TRUE;
}

QSpiActionArray QSpiActionAdaptor::getActions()
{
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

QString QSpiActionAdaptor::getDescription(int index)
{
    return ACTION_INTERFACE->description (index);
}

QString QSpiActionAdaptor::getKeyBinding(int index)
{
    QStringList keyBindings;

    keyBindings = ACTION_INTERFACE->keyBindings (index);
    /* Might as well return the first key binding, what are the other options? */
    if (keyBindings.length() > 0)
        return keyBindings[0];
    else
        return "";
}

QString QSpiActionAdaptor::getName(int index)
{
    return ACTION_INTERFACE->name (index);
}
