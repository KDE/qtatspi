
#include <QtTest/QtTest>
#include <QtGui>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusInterface>

#include "../src/dbusconnection.h"
#include "../src/struct_marshallers.h"

class AccessibleTestWindow: public QWidget
{
    Q_OBJECT
public:
    AccessibleTestWindow() {
        DBusConnection c;
        m_address = c.connection().baseService().toLatin1().data();
        m_layout = new QHBoxLayout;
        setLayout(m_layout);
    }
    QString dbusAddress() const {return m_address;}

    void addChild(QWidget* c)
    {
        m_layout->addWidget(c);
    }

    void clearChildren()
    {
        qDeleteAll(children());
    }

private:
    QString m_address;
    QString m_bus;
    QHBoxLayout* m_layout;
};




class tst_QtAtSpi :public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void testLabel();

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
    interface->property("Parent");
    QSpiObjectReference parent;
    const QDBusArgument a = interface->property("Parent").value<QDBusArgument>();
    a >> parent;
    return parent.path.path();
}

// helper to get dbus object
QDBusInterface* tst_QtAtSpi::getInterface(const QString& path, const QString& interfaceName)
{
    return new QDBusInterface(address, path, interfaceName, dbus.connection(), this);
}


void tst_QtAtSpi::initTestCase()
{
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
    m_window->addChild(l);

    QStringList children = getChildren(mainWindow);

    QDBusInterface* labelInterface = getInterface(children.at(0), "org.a11y.atspi.Accessible");
    QVERIFY(labelInterface->isValid());
    QCOMPARE(labelInterface->property("Name").toString(), QLatin1String("Hello A11y"));
    QCOMPARE(getChildren(labelInterface).count(), 0);
    QCOMPARE(labelInterface->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("label"));
    QCOMPARE(labelInterface->call(QDBus::Block, "GetRole").arguments().first().toUInt(), 29u);

    l->setText("New text");
    QCOMPARE(labelInterface->property("Name").toString(), l->text());
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
