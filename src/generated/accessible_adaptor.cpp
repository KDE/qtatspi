/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -i marshallers.h -a accessible_adaptor ../../xml/Accessible.xml
 *
 * qdbusxml2cpp is Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "accessible_adaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class AccessibleAdaptor
 */

AccessibleAdaptor::AccessibleAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

AccessibleAdaptor::~AccessibleAdaptor()
{
    // destructor
}

int AccessibleAdaptor::childCount() const
{
    // get the value of property childCount
    return qvariant_cast< int >(parent()->property("childCount"));
}

QString AccessibleAdaptor::description() const
{
    // get the value of property description
    return qvariant_cast< QString >(parent()->property("description"));
}

QString AccessibleAdaptor::name() const
{
    // get the value of property name
    return qvariant_cast< QString >(parent()->property("name"));
}

//HAND-EDIT!!
QSpiObjectReference AccessibleAdaptor::getParent() const
{
    // get the value of property parent
    return qvariant_cast< QSpiObjectReference >(parent()->property("parent"));
}

QSpiObjectReference AccessibleAdaptor::GetApplication()
{
    // handle method call org.a11y.atspi.Accessible.GetApplication
    QSpiObjectReference out0;
    QMetaObject::invokeMethod(parent(), "GetApplication", Q_RETURN_ARG(QSpiObjectReference, out0));
    return out0;
}

QSpiAttributeSet AccessibleAdaptor::GetAttributes()
{
    // handle method call org.a11y.atspi.Accessible.GetAttributes
    QSpiAttributeSet out0;
    QMetaObject::invokeMethod(parent(), "GetAttributes", Q_RETURN_ARG(QSpiAttributeSet, out0));
    return out0;
}

QSpiObjectReference AccessibleAdaptor::GetChildAtIndex(int index)
{
    // handle method call org.a11y.atspi.Accessible.GetChildAtIndex
    QSpiObjectReference out0;
    QMetaObject::invokeMethod(parent(), "GetChildAtIndex", Q_RETURN_ARG(QSpiObjectReference, out0), Q_ARG(int, index));
    return out0;
}

int AccessibleAdaptor::GetIndexInParent()
{
    // handle method call org.a11y.atspi.Accessible.GetIndexInParent
    int out0;
    QMetaObject::invokeMethod(parent(), "GetIndexInParent", Q_RETURN_ARG(int, out0));
    return out0;
}

QString AccessibleAdaptor::GetLocalizedRoleName()
{
    // handle method call org.a11y.atspi.Accessible.GetLocalizedRoleName
    QString out0;
    QMetaObject::invokeMethod(parent(), "GetLocalizedRoleName", Q_RETURN_ARG(QString, out0));
    return out0;
}

QSpiRelationArray AccessibleAdaptor::GetRelationSet()
{
    // handle method call org.a11y.atspi.Accessible.GetRelationSet
    QSpiRelationArray out0;
    QMetaObject::invokeMethod(parent(), "GetRelationSet", Q_RETURN_ARG(QSpiRelationArray, out0));
    return out0;
}

uint AccessibleAdaptor::GetRole()
{
    // handle method call org.a11y.atspi.Accessible.GetRole
    uint out0;
    QMetaObject::invokeMethod(parent(), "GetRole", Q_RETURN_ARG(uint, out0));
    return out0;
}

QString AccessibleAdaptor::GetRoleName()
{
    // handle method call org.a11y.atspi.Accessible.GetRoleName
    QString out0;
    QMetaObject::invokeMethod(parent(), "GetRoleName", Q_RETURN_ARG(QString, out0));
    return out0;
}

QSpiIntList AccessibleAdaptor::GetState()
{
    // handle method call org.a11y.atspi.Accessible.GetState
    QSpiIntList out0;
    QMetaObject::invokeMethod(parent(), "GetState", Q_RETURN_ARG(QSpiIntList, out0));
    return out0;
}
