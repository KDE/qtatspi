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

#ifndef Q_SPI_CACHE_H
#define Q_SPI_CACHE_H


#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include <QtDBus/QtDBus>

#include <QAccessible>

class QSpiAccessibleObject;

class QSpiAccessibleCache : public QObject
{
    Q_OBJECT

public:
    QSpiAccessibleCache (QObject *root);

    bool eventFilter(QObject *obj, QEvent *event);

public slots:
    void objectDestroyed (QObject *object);

    QSpiAccessibleObject *getRoot ();
    QList <QSpiAccessibleObject *> listAccessibles ();

    QSpiAccessibleObject *lookupObject (QObject *);

    void updateAccessible (QSpiAccessibleObject *accessible, QAccessible::Event event);

signals:
    void accessibleUpdated   (QSpiAccessibleObject *accessible);
    void accessibleDestroyed (QSpiAccessibleObject *accessible);

private:

    void registerConnected (QObject *object);
    void registerChildren (QObject *object);

    QObject *root;
    QHash <QObject *, QSpiAccessibleObject *> cache;
};

#endif /* Q_SPI_CACHE_H */
