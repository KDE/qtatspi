/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -i adaptor_marshallers.h -c QSpiApplicationAdaptor -a application_adaptor org.freedesktop.atspi.Application.xml
 *
 * qdbusxml2cpp is Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "application_adaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class QSpiApplicationAdaptor
 */

QSpiApplicationAdaptor::QSpiApplicationAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

QSpiApplicationAdaptor::~QSpiApplicationAdaptor()
{
    // destructor
}

int QSpiApplicationAdaptor::id() const
{
    return 0;
}

QString QSpiApplicationAdaptor::toolkitName() const
{
    return QLatin1String("Qt");
}

QString QSpiApplicationAdaptor::version() const
{
    return QLatin1String(QT_VERSION_STR);
}

QString QSpiApplicationAdaptor::GetLocale(uint lctype)
{
    QLocale currentLocale;
    return currentLocale.languageToString (currentLocale.language());
}

