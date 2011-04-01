#include <QtGui>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Simple A11y Test App");

    QTabWidget w;
    w.show();

    if (qgetenv("QT_ACCESSIBILITY") != "1")
        QMessageBox::warning(&w, QWidget::tr("Qt Accessibility not enabled"),
            QWidget::tr("In order to test accessibility, please export \"QT_ACCESSIBILITY=1\" before starting this applicaton."));


    w.addTab(new QLabel("Surprise", &w), "Tab 1");




    return a.exec();
}
