/*
 * Copyright (C) 2012 José Millán Soto <fid@gpul.org>
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

#ifndef STANDARDACTIONWRAPPER_H
#define STANDARDACTIONWRAPPER_H

#include <qaccessible2.h>
#include <QList>

class StandardActionWrapper: public QAccessibleActionInterface
{
public:
    StandardActionWrapper(QAccessibleInterface *interface, int child);
    virtual int actionCount();
    virtual QString description(int actionIndex);
    virtual void doAction(int actionIndex);
    virtual QStringList keyBindings(int actionIndex);
    virtual QString localizedName(int actionIndex);
    virtual QString name(int actionIndex);

private:
    int getAccessibleInterfaceIndex(int actionIndex);

    QAccessibleInterface *m_interface;
    QList<int> m_implementedStandardActions;
    int m_child;
};

#endif
