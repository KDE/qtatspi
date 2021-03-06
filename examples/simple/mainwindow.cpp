/*
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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QDialog>
#include <QMessageBox>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusBar->showMessage("Status Bar Text");

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(createButton()));
    connect(ui->action_Quit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(ui->addListButton, SIGNAL(clicked()), this, SLOT(addListItem()));
    connect(ui->renameButton, SIGNAL(clicked()), this, SLOT(renameListItem()));
    connect(ui->reorderButton, SIGNAL(clicked()), this, SLOT(reorderListItems()));
    connect(ui->removeListButton, SIGNAL(clicked()), this, SLOT(removeListItem()));
    connect(ui->greetButton, SIGNAL(clicked()), this, SLOT(greet()));
    connect(ui->hideButton, SIGNAL(clicked()), ui->showButton, SLOT(show()));
    connect(ui->hideButton, SIGNAL(clicked()), ui->hideButton, SLOT(hide()));
    connect(ui->hideButton, SIGNAL(clicked()), ui->tableWidget, SLOT(hide()));
    connect(ui->showButton, SIGNAL(clicked()), ui->hideButton, SLOT(show()));
    connect(ui->showButton, SIGNAL(clicked()), ui->showButton, SLOT(hide()));
    connect(ui->showButton, SIGNAL(clicked()), ui->tableWidget, SLOT(show()));
    ui->showButton->hide();

    ui->treeWidget->expandAll();

    QMenu *menu = new QMenu(ui->toolButtonWithMenu);
    ui->toolButtonWithMenu->setMenu(menu);
    ui->toolButtonWithMenu2->setMenu(menu);
    QAction *foo = new QAction("foo", menu);
    menu->addAction(foo);

    ui->scrollArea->setWidget(ui->loremLabel);

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

void MainWindow::greet()
{
    QString greetMessage = QString(QLatin1String("Hi %1!")).arg(ui->lineEdit->text());;
    QMessageBox::information(this, tr("Hi!"), greetMessage);
}

void MainWindow::addListItem()
{
    ui->listWidget->addItem("Foo");
}

void MainWindow::renameListItem()
{
    bool doRename;
    QString currentName = ui->listWidget->currentItem()->text();
    QString newName = QInputDialog::getText(this, QLatin1String("New name"), QLatin1String("Insert the new name"), QLineEdit::Normal, currentName, &doRename);

    if (doRename) {
        ui->listWidget->currentItem()->setText(newName);
    }
}

void MainWindow::reorderListItems()
{
    ui->listWidget->model()->sort(0);
}


void MainWindow::removeListItem()
{
    int positionToRemove = ui->listWidget->currentRow();
    delete ui->listWidget->takeItem(positionToRemove);
}
