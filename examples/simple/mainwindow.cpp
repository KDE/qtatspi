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

