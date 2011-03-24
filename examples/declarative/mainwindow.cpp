#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtGui/QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->declarativeView->setSource(QUrl::fromLocalFile("accessible.qml"));

    if (qgetenv("QT_ACCESSIBILITY") != "1")
        QMessageBox::warning(this, tr("Qt Accessibility not enabled"),
            tr("In order to test accessibility, please export \"QT_ACCESSIBILITY=1\" before starting this applicaton."));
}

MainWindow::~MainWindow()
{
    delete ui;
}
