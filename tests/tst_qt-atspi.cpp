/*
 * D-Bus AT-SPI, Qt Adaptor
 *
 * Copyright 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#define QT_GUI_LIB
#include <QtTest/QtTest>
#include <QtGui>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusInterface>
#include <QDBusReply>

#include <atspi/atspi-constants.h>

#include "../src/dbusconnection.h"
#include "../src/struct_marshallers.h"

#define COMPARE3(v1, v2, v3) QCOMPARE(v1, v3); QCOMPARE(v2, v3);

class AccessibleTestWindow: public QWidget
{
    Q_OBJECT
public:
    AccessibleTestWindow() {
        DBusConnection c;
        m_address = c.connection().baseService().toLatin1().data();
        new QHBoxLayout(this);
    }
    QString dbusAddress() const {return m_address;}

    void addWidget(QWidget* widget) {
        layout()->addWidget(widget);
        widget->show();
        QTest::qWaitForWindowShown(widget);
    }

    void clearChildren()
    {
        qDeleteAll(children());
        new QHBoxLayout(this);
    }

private:
    QString m_address;
    QString m_bus;
};




class tst_QtAtSpi :public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void testLabel();
    void testLineEdit();
    void testListWidget();
    void testTreeWidget();
    void testTextEdit();
    void testSlider();

    void cleanupTestCase();
//     void rootObject();
//     void navigateChildren();
//     void menu();
// 
//     void cache();

private:
    void registerDbus();
    static QString getParent(QDBusInterface* interface);
    static QStringList getChildren(QDBusInterface* interface);
    QDBusInterface* getInterface(const QString& path, const QString& interfaceName);

    AccessibleTestWindow* m_window;

    QString bus;
    QString address;
    QDBusInterface* root; // the root object on dbus (for the app)
    QDBusInterface* rootApplication;
    QDBusInterface* mainWindow;

    DBusConnection dbus;
};

// helper to find children of a dbus object
QStringList tst_QtAtSpi::getChildren(QDBusInterface* interface)
{
    QSpiObjectReferenceArray list;
    interface->call(QDBus::Block, "GetChildren").arguments().first().value<QDBusArgument>() >> list;

    Q_ASSERT(interface->property("ChildCount").toInt() == list.count());

    QStringList children;
    foreach(const QSpiObjectReference& ref, list) {
        children << ref.path.path();
    }
    return children;
}

QString tst_QtAtSpi::getParent(QDBusInterface* interface)
{
    if (!interface->isValid())
        return QString();

    QVariant var = interface->property("Parent");
    if (!var.canConvert<QSpiObjectReference>()) {
        qWarning() << "Invalid parent";
        return QString();
    }
    qDebug() << "VAR: " << var;

    QSpiObjectReference parent = var.value<QSpiObjectReference>();
//    const QDBusArgument a = var.value<QDBusArgument>();

//    a >> parent;
    return parent.path.path();
}

// helper to get dbus object
QDBusInterface* tst_QtAtSpi::getInterface(const QString& path, const QString& interfaceName)
{
    return new QDBusInterface(address, path, interfaceName, dbus.connection(), this);
}


void tst_QtAtSpi::initTestCase()
{
    // Oxygen style creates many extra items, it's simply unusable here
    qDebug() << "Using plastique style...";
    qApp->setStyle("plastique");
    qApp->setApplicationName("tst_QtAtSpi app");
    QCOMPARE(qgetenv("QT_ACCESSIBILITY"), QByteArray("1"));

    dbus = DBusConnection();

    m_window = new AccessibleTestWindow();
    m_window->show();
    QTest::qWaitForWindowShown(m_window);

    address = m_window->dbusAddress();
    registerDbus();

    QStringList appChildren = getChildren(root);
    QString window = appChildren.at(0);
    mainWindow = getInterface(window, "org.a11y.atspi.Accessible");
}

void tst_QtAtSpi::cleanupTestCase()
{
    delete mainWindow;
    delete rootApplication;
    delete root;
    delete m_window;
}

void tst_QtAtSpi::registerDbus()
{
    QVERIFY(dbus.connection().isConnected());

    root = getInterface("/org/a11y/atspi/accessible/root",
                        "org.a11y.atspi.Accessible");

    rootApplication = getInterface("/org/a11y/atspi/accessible/root",
                        "org.a11y.atspi.Application");
    QVERIFY(root->isValid());
    QVERIFY(rootApplication->isValid());

    QStringList appChildren = getChildren(root);
    QString window = appChildren.at(0);
    mainWindow = getInterface(window, "org.a11y.atspi.Accessible");
}

#define ROOTPATH "/org/a11y/atspi/accessible"

void tst_QtAtSpi::testLabel()
{
    QLabel* l = new QLabel(m_window);
    l->setText("Hello A11y");
    m_window->addWidget(l);

    // Application
    QCOMPARE(getParent(mainWindow), QLatin1String(ATSPI_DBUS_PATH_ROOT));
    QStringList children = getChildren(mainWindow);

    QDBusInterface* labelInterface = getInterface(children.at(0), "org.a11y.atspi.Accessible");
    QVERIFY(labelInterface->isValid());
    QCOMPARE(labelInterface->property("Name").toString(), QLatin1String("Hello A11y"));
    QCOMPARE(getChildren(labelInterface).count(), 0);
    QCOMPARE(labelInterface->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("label"));
    QCOMPARE(labelInterface->call(QDBus::Block, "GetRole").arguments().first().toUInt(), 29u);
    QCOMPARE(getParent(labelInterface), mainWindow->path());

    l->setText("New text");
    QCOMPARE(labelInterface->property("Name").toString(), l->text());

    m_window->clearChildren();
    delete labelInterface;
}

void tst_QtAtSpi::testLineEdit()
{
    QLineEdit *lineEdit = new QLineEdit(m_window);
    lineEdit->setText("a11y test QLineEdit");
    m_window->addWidget(lineEdit);

    QStringList children = getChildren(mainWindow);

    QDBusInterface* accessibleInterface = getInterface(children.at(0), "org.a11y.atspi.Accessible");
    QDBusInterface* editableTextInterface = getInterface(children.at(0), "org.a11y.atspi.EditableText");
    QDBusInterface* textInterface = getInterface(children.at(0), "org.a11y.atspi.Text");
    QVERIFY(accessibleInterface->isValid());
    QVERIFY(editableTextInterface->isValid());
    QVERIFY(textInterface->isValid());

    QCOMPARE(accessibleInterface->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("text"));
    QCOMPARE(textInterface->call(QDBus::Block,"GetText", 5, -1).arguments().first().toString(), QLatin1String("test QLineEdit"));
    QString newText = "Text has changed!";
    editableTextInterface->call(QDBus::Block, "SetTextContents", newText);
    COMPARE3(lineEdit->text(), textInterface->call(QDBus::Block, "GetText", 0, -1).arguments().first().toString(), newText);
    QCOMPARE(textInterface->call(QDBus::Block, "GetText", 0, 4).arguments().first().toString(), QLatin1String("Text"));
    editableTextInterface->call(QDBus::Block, "DeleteText", 4, 8);
    COMPARE3(lineEdit->text(), QString("Te" + textInterface->call(QDBus::Block, "GetText", 2, 10).arguments().first().toString() + "ed!"), QLatin1String("Text changed!"));
    editableTextInterface->call(QDBus::Block, "InsertText", 12, " again ", 6);
    QCOMPARE(lineEdit->text(), QLatin1String("Text changed again!"));
    COMPARE3(lineEdit->text().length(), textInterface->property("CharacterCount").toInt(), 19);

    textInterface->call(QDBus::Block, "SetCaretOffset", 4);
    COMPARE3(lineEdit->cursorPosition(), textInterface->property("CaretOffset").toInt(), 4);

    textInterface->call(QDBus::Block, "AddSelection", 1, 4);
    QList<QVariant> data = textInterface->call(QDBus::Block, "GetSelection", 0).arguments();
    COMPARE3(data.at(0).toInt(), lineEdit->selectionStart(), 1);
    QCOMPARE(data.at(1).toInt(), 4);
    QCOMPARE(lineEdit->selectedText().length(), 3);
    QCOMPARE(textInterface->call(QDBus::Block, "GetNSelections").arguments().first().toInt(), 1); 
    textInterface->call(QDBus::Block, "SetSelection", 0, 0, 5);
    data = textInterface->call(QDBus::Block, "GetSelection", 0).arguments();
    COMPARE3(data.at(0).toInt(), lineEdit->selectionStart(), 0);
    COMPARE3(data.at(1).toInt(), lineEdit->selectedText().length(), 5);
    textInterface->call(QDBus::Block, "RemoveSelection", 0);
    QCOMPARE(lineEdit->selectionStart(), -1);
    QCOMPARE(textInterface->call(QDBus::Block, "GetNSelections").arguments().first().toInt(), 0);

    m_window->clearChildren();
    delete accessibleInterface;
    delete textInterface;
    delete editableTextInterface;
}

void tst_QtAtSpi::testListWidget()
{
    QListWidget *lw = new QListWidget;
    lw->addItem("Hello");
    lw->addItem("Good morning");
    lw->addItem("Good bye");
    m_window->addWidget(lw);

    QStringList children = getChildren(mainWindow);
    QDBusInterface* listIface = getInterface(children.at(0), "org.a11y.atspi.Accessible");
    QCOMPARE(listIface->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("list"));
    QStringList tableChildren = getChildren(listIface);
    QCOMPARE(tableChildren.size(), 3);

    QDBusInterface* cell1 = getInterface(tableChildren.at(0), "org.a11y.atspi.Accessible");
    QCOMPARE(cell1->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("list item"));
    QCOMPARE(cell1->property("Name").toString(), QLatin1String("Hello"));

    QDBusInterface* cell2 = getInterface(tableChildren.at(1), "org.a11y.atspi.Accessible");
    QCOMPARE(cell2->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("list item"));
    QCOMPARE(cell2->property("Name").toString(), QLatin1String("Good morning"));

    QDBusInterface* cell3 = getInterface(tableChildren.at(2), "org.a11y.atspi.Accessible");
    QCOMPARE(cell3->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("list item"));
    QCOMPARE(cell3->property("Name").toString(), QLatin1String("Good bye"));

    delete cell1; delete cell2; delete cell3;
    m_window->clearChildren();
    delete listIface;
}

void tst_QtAtSpi::testTreeWidget()
{
    QTreeWidget *tree = new QTreeWidget;
    tree->setColumnCount(2);
    tree->setHeaderLabels(QStringList() << "Header 1" << "Header 2");

    QTreeWidgetItem *top1 = new QTreeWidgetItem(QStringList() << "0.0" << "0.1");
    tree->addTopLevelItem(top1);

    QTreeWidgetItem *top2 = new QTreeWidgetItem(QStringList() << "1.0" << "1.1");
    tree->addTopLevelItem(top2);

    QTreeWidgetItem *child1 = new QTreeWidgetItem(QStringList() << "1.0 0.0" << "1.0 0.1");
    top2->addChild(child1);

    m_window->addWidget(tree);

    QStringList children = getChildren(mainWindow);
    QDBusInterface* treeIface = getInterface(children.at(0), "org.a11y.atspi.Accessible");
    QCOMPARE(treeIface->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("tree"));
    QStringList tableChildren = getChildren(treeIface);

    QCOMPARE(tableChildren.size(), 6);

    QDBusInterface* cell1 = getInterface(tableChildren.at(0), "org.a11y.atspi.Accessible");
    QCOMPARE(cell1->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("column header"));
    QCOMPARE(cell1->property("Name").toString(), QLatin1String("Header 1"));

    QDBusInterface* cell2 = getInterface(tableChildren.at(1), "org.a11y.atspi.Accessible");
    QCOMPARE(cell2->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("column header"));
    QCOMPARE(cell2->property("Name").toString(), QLatin1String("Header 2"));

    QDBusInterface* cell3 = getInterface(tableChildren.at(2), "org.a11y.atspi.Accessible");
    QCOMPARE(cell3->property("Name").toString(), QLatin1String("0.0"));

    QDBusInterface* cell4 = getInterface(tableChildren.at(3), "org.a11y.atspi.Accessible");
    QCOMPARE(cell4->property("Name").toString(), QLatin1String("0.1"));

    tree->expandItem(top2);
    tableChildren = getChildren(treeIface);
    QCOMPARE(tableChildren.size(), 8);

    QDBusInterface* cell5 = getInterface(tableChildren.at(6), "org.a11y.atspi.Accessible");
    QCOMPARE(cell5->property("Name").toString(), QLatin1String("1.0 0.0"));

    QDBusInterface* cell6 = getInterface(tableChildren.at(7), "org.a11y.atspi.Accessible");
    QCOMPARE(cell6->property("Name").toString(), QLatin1String("1.0 0.1"));


    QDBusInterface* treeTableIface = getInterface(children.at(0), "org.a11y.atspi.Table");

    QCOMPARE(treeTableIface->call(QDBus::Block, "GetRowAtIndex", 0).arguments().first().toInt(), -1);
    QCOMPARE(treeTableIface->call(QDBus::Block, "GetRowAtIndex", 1).arguments().first().toInt(), -1);
    QCOMPARE(treeTableIface->call(QDBus::Block, "GetRowAtIndex", 2).arguments().first().toInt(), 0);
    QCOMPARE(treeTableIface->call(QDBus::Block, "GetRowAtIndex", 3).arguments().first().toInt(), 0);
    QCOMPARE(treeTableIface->call(QDBus::Block, "GetRowAtIndex", 4).arguments().first().toInt(), 1);
    QCOMPARE(treeTableIface->call(QDBus::Block, "GetRowAtIndex", 5).arguments().first().toInt(), 1);
    QCOMPARE(treeTableIface->call(QDBus::Block, "GetRowAtIndex", 6).arguments().first().toInt(), 2);
    QCOMPARE(treeTableIface->call(QDBus::Block, "GetRowAtIndex", 7).arguments().first().toInt(), 2);

    QCOMPARE(treeTableIface->call(QDBus::Block, "GetColumnAtIndex", 0).arguments().first().toInt(), 0);
    QCOMPARE(treeTableIface->call(QDBus::Block, "GetColumnAtIndex", 1).arguments().first().toInt(), 1);
    QCOMPARE(treeTableIface->call(QDBus::Block, "GetColumnAtIndex", 2).arguments().first().toInt(), 0);
    QCOMPARE(treeTableIface->call(QDBus::Block, "GetColumnAtIndex", 3).arguments().first().toInt(), 1);
    QCOMPARE(treeTableIface->call(QDBus::Block, "GetColumnAtIndex", 4).arguments().first().toInt(), 0);
    QCOMPARE(treeTableIface->call(QDBus::Block, "GetColumnAtIndex", 5).arguments().first().toInt(), 1);
    QCOMPARE(treeTableIface->call(QDBus::Block, "GetColumnAtIndex", 6).arguments().first().toInt(), 0);
    QCOMPARE(treeTableIface->call(QDBus::Block, "GetColumnAtIndex", 7).arguments().first().toInt(), 1);

    delete treeTableIface;
    delete cell1; delete cell2; delete cell3; delete cell4;
    m_window->clearChildren();
    delete treeIface;
}

void tst_QtAtSpi::testTextEdit()
{
    QTextEdit *textEdit = new QTextEdit(m_window);
    textEdit->setText("<html><head></head><body>This is a <b>sample</b> text.<br />"
                      "How are you today</body></html>");
    textEdit->show();
    m_window->addWidget(textEdit);

    QStringList children = getChildren(mainWindow);
    QDBusInterface* accessibleInterface = getInterface(children.at(0), "org.a11y.atspi.Accessible");
    QDBusInterface* editableTextInterface = getInterface(children.at(0), "org.a11y.atspi.EditableText");
    QDBusInterface* textInterface = getInterface(children.at(0), "org.a11y.atspi.Text");
    QVERIFY(accessibleInterface->isValid());
    QVERIFY(editableTextInterface->isValid());
    QVERIFY(textInterface->isValid());

    QList<QVariant> callResult;

    QDBusMessage msg = textInterface->call(QDBus::Block, "GetText", 0, 5);
    callResult = msg.arguments();
    QCOMPARE(callResult.at(0).toString(), QLatin1String("This "));

    msg = textInterface->call(QDBus::Block, "GetTextAtOffset", 12, (uint) ATSPI_TEXT_BOUNDARY_WORD_START);
    callResult = msg.arguments();

    QEXPECT_FAIL("", "Word should contain space at end according to atspi.", Continue);
    QCOMPARE(callResult.at(0).toString(), QLatin1String("sample "));
    QCOMPARE(callResult.at(1).toInt(), 10);
    QEXPECT_FAIL("", "Due to missing space the count is off by one.", Continue);
    QCOMPARE(callResult.at(2).toInt(), 17);

    // Check if at least CharacterExtents and RangeExtents give a consistent result
    QDBusReply<QRect> replyRect20 = textInterface->call(QDBus::Block, "GetCharacterExtents", 20, ATSPI_COORD_TYPE_SCREEN);
    QVERIFY(replyRect20.isValid());
    QRect r1 = replyRect20.value();
    QDBusReply<QRect> replyRect21  = textInterface->call(QDBus::Block, "GetCharacterExtents", 21, ATSPI_COORD_TYPE_SCREEN);
    QRect r2 = replyRect21.value();
    QDBusReply<QRect> reply = textInterface->call(QDBus::Block, "GetRangeExtents", 20, 21, ATSPI_COORD_TYPE_SCREEN);
    QRect rect = reply.value();
    QCOMPARE(rect, r1|r2);

    m_window->clearChildren();
    delete textInterface;
}

void tst_QtAtSpi::testSlider()
{
    QSlider *slider = new QSlider(m_window);
    slider->setMinimum(2);
    slider->setMaximum(5);
    slider->setValue(3);
    m_window->addWidget(slider);

    QStringList children = getChildren(mainWindow);

    QDBusInterface* accessibleInterface = getInterface(children.at(0), "org.a11y.atspi.Accessible");
    QDBusInterface* valueInterface = getInterface(children.at(0), "org.a11y.atspi.Value");
    QVERIFY(accessibleInterface->isValid());
    QVERIFY(valueInterface->isValid());

    QCOMPARE(valueInterface->property("CurrentValue").toInt(), 3);
    QCOMPARE(valueInterface->property("MinimumValue").toInt(), 2);
    QCOMPARE(valueInterface->property("MaximumValue").toInt(), 5);

    valueInterface->setProperty("CurrentValue", 4);
    QCOMPARE(valueInterface->property("CurrentValue").toInt(), 4);
}


// void tst_QtAtSpi::rootObject()
// {
//     QCOMPARE(root->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("application"));
//     QCOMPARE(root->call(QDBus::Block, "GetRole").arguments().first().toUInt(), 75u);
//     QCOMPARE(root->property("ChildCount").toInt(), 1);
//     QCOMPARE(root->property("Name").toString(), QLatin1String("Simple A11y Test App"));
//     QCOMPARE(root->call(QDBus::Block, "GetChildren").arguments().first().toString(), QString());
// 
//     QStringList children = getChildren(root);
//     QCOMPARE(children.length(), 1);
// //    QCOMPARE(children.at(0), QLatin1String(ROOTPATH "/1"));
// 
//     qDebug() << "Toolkit Name: " << rootApplication->property("ToolkitName");
//     qDebug() << "Version: " << rootApplication->property("Version");
// 

//     QCOMPARE(mainWindow->property("Name").toString(), QLatin1String("MainWindow"));
//     QCOMPARE(mainWindow->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("window"));
// }
// 
// 
// void tst_QtAtSpi::navigateChildren()
// {
// 
//     // FIXME
// //    QCOMPARE(getParent(mainWindow), QLatin1String("/org/a11y/atspi/accessible/0"));
// //    mainWindow->property("Parent");
// 
//     QStringList mainWinChildren = getChildren(mainWindow);
//     QCOMPARE(mainWinChildren.count(), 3);
// //    QCOMPARE(mainWinChildren, QStringList() << ROOTPATH "/2" << ROOTPATH "/3" << ROOTPATH "/4");
// 
//     // first child: central widget. the others are menu+statusbar
//     QDBusInterface* centralWidget = getInterface(mainWinChildren.first(), "org.a11y.atspi.Accessible");
//     QCOMPARE(centralWidget->property("Name").toString(), QString());
//     QStringList centralWidgetChildren = getChildren(centralWidget);
//     QCOMPARE(centralWidgetChildren.count(), 7);
// 
//     QDBusInterface* button = getInterface(centralWidgetChildren.at(1), "org.a11y.atspi.Accessible");
//     QVERIFY(button->isValid());
//     QCOMPARE(button->property("Name").toString(), QLatin1String("Create More Buttons"));
//     QCOMPARE(getChildren(button).count(), 0);
// 
//     QDBusInterface* label2 = getInterface(centralWidgetChildren.at(2), "org.a11y.atspi.Accessible");
//     QCOMPARE(label2->property("Name").toString(), QLatin1String("Enter your name:"));
//     QCOMPARE(getChildren(label2).count(), 0);
// 
//     QDBusInterface* lineEdit = getInterface(centralWidgetChildren.at(3), "org.a11y.atspi.Accessible");
//     QCOMPARE(lineEdit->property("Name").toString(), QLatin1String("Name line edit"));
//     QCOMPARE(getChildren(lineEdit).count(), 0);
// 
//     QDBusInterface* text = getInterface(centralWidgetChildren.at(4), "org.a11y.atspi.Accessible");
//     // QCOMPARE(text->property("Name").toString().left(6), QString());
//     QCOMPARE(getChildren(text).count(), 7);
//     // FIXME: why does this guy have so many kids?
// 
//     // tabbar and stackwidget
//     QDBusInterface* tabWidget = getInterface(centralWidgetChildren.at(5), "org.a11y.atspi.Accessible");
//     QCOMPARE(tabWidget->property("Name").toString(), QString());
//     QStringList tabWidgetChildren = getChildren(tabWidget);
//     QCOMPARE(tabWidgetChildren.count(), 2);
// 
//     QDBusInterface* stackWidget = getInterface(tabWidgetChildren.at(0), "org.a11y.atspi.Accessible");
//     QCOMPARE(stackWidget->property("Name").toString(), QString());
//     QCOMPARE(stackWidget->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("layered pane"));
//     QStringList stackChildren = getChildren(stackWidget);
//     QCOMPARE(stackChildren.count(), 2);
// 
//     QDBusInterface* stackWidget1 = getInterface(stackChildren.at(0), "org.a11y.atspi.Accessible");
//     QCOMPARE(stackWidget1->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("filler"));
//     QStringList stackWidget1Children = getChildren(stackWidget1);
// 
//     // radiobuttons
//     QDBusInterface* radio1 = getInterface(stackWidget1Children.at(0), "org.a11y.atspi.Accessible");
//     QCOMPARE(radio1->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("radio button"));
//     QCOMPARE(radio1->property("ChildCount").toInt(), 0);
//     QCOMPARE(getChildren(radio1), QStringList());
// //    QCOMPARE(radio1->property("Name").toString(), QLatin1String("Apple"));
// 
//     QDBusInterface* stackWidget2 = getInterface(stackChildren.at(1), "org.a11y.atspi.Accessible");
//     QCOMPARE(stackWidget2->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("filler"));
// 
//     // second tab?
// 
//     delete radio1;
//     delete stackWidget1;
//     delete stackWidget2;
// 
//     QDBusInterface* tabBar = getInterface(tabWidgetChildren.at(1), "org.a11y.atspi.Accessible");
//     // FIXME: is that good? we get the child's name here...
//     QCOMPARE(tabBar->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("page tab list"));
//     QCOMPARE(tabBar->property("ChildCount").toInt(), 4);
//     QStringList tabBarChildren = getChildren(tabWidget);
//     QCOMPARE(tabBarChildren.count(), 2);
// 
//     QDBusInterface* tabButton1 = getInterface(tabBarChildren.at(0), "org.a11y.atspi.Accessible");
//     QCOMPARE(tabButton1->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("page tab"));
//     QCOMPARE(tabButton1->property("Name").toString(), QLatin1String("Fruits"));
//     QStringList tabButton1Children = getChildren(tabButton1);
//     QVERIFY(tabBarChildren.first() != tabButton1Children.first());
//     QCOMPARE(getChildren(tabButton1).count(), 4);
//     QCOMPARE(tabButton1->property("ChildCount").toInt(), 4);
// 
//     QDBusInterface* tabButton2 = getInterface(tabBarChildren.at(1), "org.a11y.atspi.Accessible");
//     QCOMPARE(tabButton2->property("Name").toString(), QLatin1String("Other"));
//     QCOMPARE(tabButton2->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("page tab"));
//     QStringList tabButton2Children = getChildren(tabButton1);
//     QVERIFY(tabBarChildren.first() != tabButton2Children.first());
//     QCOMPARE(getChildren(tabButton2).count(), 4);
//     QCOMPARE(tabButton2->property("ChildCount").toInt(), 4);
// 
// 
// //    QDBusInterface* tabButton2 = getInterface(tabBarChildren.at(1), "org.a11y.atspi.Accessible");
// //    QCOMPARE(tabButton2->property("Name").toString(), QLatin1String("Fruits"));
// //    QCOMPARE(tabButton2->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("page tab"));
// //    QCOMPARE(getChildren(tabButton1).count(), 0);
// 
//     delete tabButton1;
//     delete tabButton2;
// 
//     delete stackWidget;
//     delete tabBar;
// 
// 
//     // combobox
//     QDBusInterface* combobox = getInterface(centralWidgetChildren.at(6), "org.a11y.atspi.Accessible");
//     QCOMPARE(combobox->property("Name").toString(), QLatin1String("Comboboxes are fun!"));
//     QCOMPARE(getChildren(combobox).count(), 3);
// 
// 
// 
//     delete combobox;
//     delete tabWidget;
//     delete text;
//     delete lineEdit;
//     delete label2;
//     delete button;
//     delete label;
// 
//     delete centralWidget;
//     delete mainWindow;
// }
// 
// void tst_QtAtSpi::menu()
// {
//     QStringList c = getChildren(mainWindow);
//     QDBusInterface* menuRoot = getInterface(c.at(1), "org.a11y.atspi.Accessible");
//     QCOMPARE(menuRoot->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("menu bar"));
//     QCOMPARE(menuRoot->property("Name").toString(), QLatin1String("aN:menubar"));
//     QStringList menus = getChildren(menuRoot);
//     QCOMPARE(menus.count(), 2); // File, Edit
// 
//     QDBusInterface* fileMenu = getInterface(menus.at(0), "org.a11y.atspi.Accessible");
//     QCOMPARE(fileMenu->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("menu item"));
//     QCOMPARE(fileMenu->property("Name").toString(), QLatin1String("File"));
// 
//     QDBusInterface* editMenu = getInterface(menus.at(1), "org.a11y.atspi.Accessible");
//     QCOMPARE(editMenu->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("menu item"));
//     QCOMPARE(editMenu->property("Name").toString(), QLatin1String("Edit"));
// }
// 
// void tst_QtAtSpi::cache()
// {
//     QDBusInterface* cache = getInterface("/org/a11y/atspi/cache", "org.a11y.atspi.Cache");
//     //qDebug() << cache->call(QDBus::Block, "GetItems");
//     delete cache;
// }

QTEST_MAIN(tst_QtAtSpi)

#include "tst_qt-atspi.moc"
