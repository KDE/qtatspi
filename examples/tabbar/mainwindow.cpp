#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtGui/QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
}

MainWindow::~MainWindow()
{
    delete ui;
}
