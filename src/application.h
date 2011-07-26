/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2009-2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef Q_SPI_APPLICATION_H
#define Q_SPI_APPLICATION_H

#include <QtDBus/QDBusConnection>

#include <QAccessible>
#include <QAccessibleInterface>
#include <QQueue>

/*
 * Used for the root object.
 *
 * Uses the root object reference and reports its parent as the desktop object.
 */
class QSpiApplicationAdaptor :public QObject
{
    Q_OBJECT

public:
    QSpiApplicationAdaptor();
    virtual ~QSpiApplicationAdaptor() {}

    // the Id property gets written and read by the accessibility framework
    // we do nothing with it internally, it is only for the at-spi2 to identify us
    Q_PROPERTY(int Id READ id WRITE setId)
    int id() const;
    void setId(int value);

Q_SIGNALS:
    void windowActivated(QObject* window);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private Q_SLOTS:
    void notifyKeyboardListenerCallback(const QDBusMessage& message);
    void notifyKeyboardListenerError(const QDBusError& error, const QDBusMessage& message);

private:
    static QKeyEvent* copyKeyEvent(QKeyEvent*);

    int applicationId;
    QQueue<QPair<QObject*, QKeyEvent*> > keyEvents;
};

#endif
