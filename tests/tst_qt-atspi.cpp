
#include <QtTest/QtTest>
#include <QtGui>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusInterface>

class tst_QtAtSpi :public QObject
{
    Q_OBJECT

private slots:
     void initTestCase();
     void cleanupTestCase();

     void registerDbus();

private:
     QProcess* calc;

     QString bus;
     QString address;
     QDBusInterface* root; // the root object on dbus (for the app)
};

void tst_QtAtSpi::initTestCase()
{
    QCOMPARE(qgetenv("QT_ACCESSIBILITY"), QByteArray("1"));

    QString app = "../examples/simple/simple";
    calc = new QProcess(this);
    calc->start(app, QStringList() << "--style" << "plastique");
    QVERIFY(calc->waitForStarted());

    bool initialized = false;
    while (!initialized) {
        calc->waitForReadyRead();
        QByteArray output = calc->readLine();
        if (output.startsWith("DBUS bus: ")) {
            bus = output.mid(10, -1);
        }
        if (output.startsWith("DBUS address: ")) {
            address = output.mid(14, -1);
            initialized = true;
        }
    }
}

void tst_QtAtSpi::cleanupTestCase()
{}

void tst_QtAtSpi::registerDbus()
{
    QDBusConnection c = bus.isEmpty() ? QDBusConnection::sessionBus() : QDBusConnection(bus);
    QVERIFY(c.isConnected());

    root = new QDBusInterface(address,
                                "/org/a11y/atspi/accessible/root",
                                "org.a11y.atspi.Accessible",
                                c, this);

    QVERIFY(root->isValid());

    QCOMPARE(root->call(QDBus::Block, "GetRoleName").arguments().first().toString(), QLatin1String("application"));
    QCOMPARE(root->call(QDBus::Block, "GetRole").arguments().first().toUInt(), 75u);
    QCOMPARE(root->property("ChildCount").toInt(), 1);
    QCOMPARE(root->property("Name").toString(), QLatin1String("Simple A11y Test App"));

    QCOMPARE(root->call(QDBus::Block, "GetChildren").arguments().first().toString(), QString());




    delete root;
}


QTEST_MAIN(tst_QtAtSpi)

#include "tst_qt-atspi.moc"
