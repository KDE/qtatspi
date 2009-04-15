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

#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

#include "object.h"
#include "accessible_adaptor.h"
#include "constant_mappings.h"

/* This is silly. Should talk about moving this mapping into client bindings */
static const char roles[] =
  "invalid\0"
  "accelerator label\0"
  "alert\0"
  "animation\0"
  "arrow\0"
  "calendar\0"
  "canvas\0"
  "check box\0"
  "check menu item\0"
  "color chooser\0"
  "column header\0"
  "combo box\0"
  "dateeditor\0"
  "desktop icon\0"
  "desktop frame\0"
  "dial\0"
  "dialog\0"
  "directory pane\0"
  "drawing area\0"
  "file chooser\0"
  "filler\0"
  "fontchooser\0"
  "frame\0"
  "glass pane\0"
  "html container\0"
  "icon\0"
  "image\0"
  "internal frame\0"
  "label\0"
  "layered pane\0"
  "list\0"
  "list item\0"
  "menu\0"
  "menu bar\0"
  "menu item\0"
  "option pane\0"
  "page tab\0"
  "page tab list\0"
  "panel\0"
  "password text\0"
  "popup menu\0"
  "progress bar\0"
  "push button\0"
  "radio button\0"
  "radio menu item\0"
  "root pane\0"
  "row header\0"
  "scroll bar\0"
  "scroll pane\0"
  "separator\0"
  "slider\0"
  "split pane\0"
  "spin button\0"
  "statusbar\0"
  "table\0"
  "table cell\0"
  "table column header\0"
  "table row header\0"
  "tear off menu item\0"
  "terminal\0"
  "text\0"
  "toggle button\0"
  "tool bar\0"
  "tool tip\0"
  "tree\0"
  "tree table\0"
  "unknown\0"
  "viewport\0"
  "window\0"
  "header\0"
  "footer\0"
  "paragraph\0"
  "ruler\0"
  "application\0"
  "autocomplete\0"
  "edit bar\0"
  "embedded component\0"
  "entry\0"
  "chart\0"
  "caption\0"
  "document frame\0"
  "heading\0"
  "page\0"
  "section\0"
  "redundant object\0"
  "form\0"
  "link\0"
  "input method window";

static const uint roles_offsets[] = {
  0, 8, 26, 32, 42, 48, 57, 64, 
  74, 90, 104, 118, 128, 139, 152, 166, 
  171, 178, 193, 206, 219, 226, 238, 244, 
  255, 270, 275, 281, 296, 302, 315, 320, 
  330, 335, 344, 354, 366, 375, 389, 395, 
  409, 420, 433, 445, 458, 474, 484, 495, 
  506, 518, 528, 535, 546, 558, 568, 574, 
  585, 605, 622, 641, 650, 655, 669, 678, 
  687, 692, 703, 711, 720, 727, 734, 741, 
  751, 757, 769, 782, 791, 810, 816, 822, 
  830, 845, 853, 858, 866, 883, 888, 893
};

/*
 * Implementation of adaptor class QSpiAccessibleAdaptor
 */

QSpiAccessibleAdaptor::QSpiAccessibleAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

QSpiAccessibleAdaptor::~QSpiAccessibleAdaptor()
{
    // destructor
}

int QSpiAccessibleAdaptor::childCount() const
{
    // get the value of property childCount
    return 0;
}

QString QSpiAccessibleAdaptor::description() const
{
    // get the value of property description
    return QString ("Nothing");
}

QString QSpiAccessibleAdaptor::name() const
{
    // get the value of property name
    return QString ("Nothing");
}

QDBusObjectPath QSpiAccessibleAdaptor::getParent() const
{
    // get the value of property parent
    return QDBusObjectPath ("/");
}

QDBusObjectPath QSpiAccessibleAdaptor::getApplication()
{
    // handle method call org.freedesktop.atspi.Accessible.getApplication
    return static_cast <QSpiAccessibleObject *> (parent())->getApplication()->getPath();
}

QStringList QSpiAccessibleAdaptor::getAttributes()
{
    // handle method call org.freedesktop.atspi.Accessible.getAttributes
    // No attributes interface in QAccessible so a blank list seems the sensible option.
    QStringList out0;
    return out0;
}

QDBusObjectPath QSpiAccessibleAdaptor::getChildAtIndex(int index)
{
    // handle method call org.freedesktop.atspi.Accessible.getChildAtIndex
    return static_cast <QSpiAccessibleObject *> (parent())->getChildren().value(index)->getPath();
}

QList<QDBusObjectPath> QSpiAccessibleAdaptor::getChildren()
{
    // handle method call org.freedesktop.atspi.Accessible.getChildren
    QList <QSpiAccessibleObject *> children;
    QList <QDBusObjectPath> childPaths;

    children = static_cast <QSpiAccessibleObject *> (parent())->getChildren();
    foreach (QSpiAccessibleObject *obj, children)
    {
        childPaths << obj->getPath();
    }
    return childPaths;
}

int QSpiAccessibleAdaptor::getIndexInParent()
{
    // handle method call org.freedesktop.atspi.Accessible.getIndexInParent
    // Not handling for now. indexInParent can now be calculated just as
    // easily on the client side.
    // TODO
    return 0;
}

QString QSpiAccessibleAdaptor::getLocalizedRoleName()
{
    // handle method call org.freedesktop.atspi.Accessible.getLocalizedRoleName
    // TODO For now the same as 'getRoleName'
    QString roleName (roles + roles_offsets[getRole()]);
    return roleName;
}

QSpiRelationArray QSpiAccessibleAdaptor::getRelationSet()
{
    // handle method call org.freedesktop.atspi.Accessible.getRelationSet
    QSpiRelationArray out0;
    // TODO
    return out0;
}

uint QSpiAccessibleAdaptor::getRole()
{
    // handle method call org.freedesktop.atspi.Accessible.getRole
    QAccessible::Role role = static_cast <QSpiAccessibleObject *>(parent())->getInterface().role(0);
    return qSpiRoleMapping[role];
}

QString QSpiAccessibleAdaptor::getRoleName()
{
    // handle method call org.freedesktop.atspi.Accessible.getRoleName
    QString roleName (roles + roles_offsets[getRole()]);
    return roleName;
}

QSpiStateSet QSpiAccessibleAdaptor::getState()
{
    // handle method call org.freedesktop.atspi.Accessible.getState
    QSpiStateSet set;
    qspi_stateset_from_qstate (static_cast <QSpiAccessibleObject *>(parent())->getInterface().state(0), set);
    return set;
}
