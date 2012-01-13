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

#include "standardactionwrapper.h"

#include <qaccessible2.h>
#include <QSet>
#include <QString>

StandardActionWrapper::StandardActionWrapper(QAccessibleInterface* interface, int child)
{
    m_interface = interface;
    m_child = child;
    QSet<QString> actionNames;
    QSet<int> toCheck;
    toCheck << QAccessible::DefaultAction;

    int state = interface->state(child);
    if (state & QAccessible::Focusable) 
        toCheck << QAccessible::SetFocus;

    for (QSet<int>::const_iterator it = toCheck.constBegin(); it != toCheck.constEnd(); it++) {
        QString actionName = interface->actionText(*it, QAccessible::Name, child);
        
        if (!actionNames.contains(actionName) && !actionName.isEmpty()) {
            actionNames << actionName;
            m_implementedStandardActions.append(*it);
        }
    }
}

int StandardActionWrapper::getAccessibleInterfaceIndex(int actionIndex)
{
    if (actionIndex < m_implementedStandardActions.size())
        return m_implementedStandardActions[actionIndex];
    else
        return actionIndex - m_implementedStandardActions.size() + 1;
}

int StandardActionWrapper::actionCount()
{
    return m_implementedStandardActions.size() + m_interface->userActionCount(m_child);
}

QString StandardActionWrapper::description(int actionIndex)
{
    return m_interface->actionText(getAccessibleInterfaceIndex(actionIndex), QAccessible::Description, m_child);
}

void StandardActionWrapper::doAction(int actionIndex)
{
    m_interface->doAction(getAccessibleInterfaceIndex(actionIndex), m_child);
}

QStringList StandardActionWrapper::keyBindings(int actionIndex)
{
    Q_UNUSED(actionIndex);
    return QStringList();
}

QString StandardActionWrapper::name(int actionIndex)
{
    return m_interface->actionText(getAccessibleInterfaceIndex(actionIndex), QAccessible::Name, m_child);
}

QString StandardActionWrapper::localizedName(int actionIndex)
{
    return name(actionIndex);
}
