#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Simple A11y Test App");
    MainWindow w;
    w.show();

    return a.exec();
}
