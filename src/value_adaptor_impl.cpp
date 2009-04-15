/*
 * This file was generated by dbusxml2cpp version 0.6
 * Command line was: dbusxml2cpp -i adaptor_marshallers.h -c QSpiValueAdaptor -a value_adaptor org.freedesktop.atspi.Value.xml
 *
 * dbusxml2cpp is Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "value_adaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class QSpiValueAdaptor
 */

QSpiValueAdaptor::QSpiValueAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

QSpiValueAdaptor::~QSpiValueAdaptor()
{
    // destructor
}

double QSpiValueAdaptor::currentValue() const
{
    // get the value of property currentValue
    return qvariant_cast< double >(parent()->property("currentValue"));
}

void QSpiValueAdaptor::setCurrentValue(double value)
{
    // set the value of property currentValue
    parent()->setProperty("currentValue", value);
}

double QSpiValueAdaptor::maximumValue() const
{
    // get the value of property maximumValue
    return qvariant_cast< double >(parent()->property("maximumValue"));
}

double QSpiValueAdaptor::minimumIncrement() const
{
    // get the value of property minimumIncrement
    return qvariant_cast< double >(parent()->property("minimumIncrement"));
}

double QSpiValueAdaptor::minimumValue() const
{
    // get the value of property minimumValue
    return qvariant_cast< double >(parent()->property("minimumValue"));
}

