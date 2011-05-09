#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtGui/QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusBar->showMessage("Status Bar Text");

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(createButton()));
    connect(ui->action_Quit, SIGNAL(triggered()), qApp, SLOT(quit()));

    if (qgetenv("QT_ACCESSIBILITY") != "1")
        QMessageBox::warning(this, tr("Qt Accessibility not enabled"),
            tr("In order to test accessibility, please export \"QT_ACCESSIBILITY=1\" before starting this applicaton."));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createButton()
{
    QPushButton* button = new QPushButton(ui->centralWidget);
    button->setText("I am a new button");
    button->setAccessibleDescription("This is an accessible description for a button.");
    ui->centralWidget->layout()->addWidget(button);
    ui->statusBar->showMessage("Button created.");
}
