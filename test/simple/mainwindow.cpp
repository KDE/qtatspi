#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(createButton()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createButton()
{
    QPushButton* button = new QPushButton(ui->centralWidget);
    button->setText("I am a new button");
    ui->centralWidget->layout()->addWidget(button);
}

