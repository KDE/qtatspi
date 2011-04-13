#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtGui/QMessageBox>
#include <qsgview.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    if (qgetenv("QT_ACCESSIBILITY") != "1")
        QMessageBox::warning(this, tr("Qt Accessibility not enabled"),
            tr("In order to test accessibility, please export \"QT_ACCESSIBILITY=1\" before starting this applicaton."));

    QSGView* view = new QSGView(this);
    setCentralWidget(view);
    view->setSource(QUrl::fromLocalFile("accessible.qml"));
}

MainWindow::~MainWindow()
{
}
