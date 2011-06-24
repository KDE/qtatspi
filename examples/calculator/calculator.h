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

#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE
class Button;

//! [0]
class Calculator : public QDialog
{
    Q_OBJECT

public:
    Calculator(QWidget *parent = 0);

private slots:
    void digitClicked();
    void unaryOperatorClicked();
    void additiveOperatorClicked();
    void multiplicativeOperatorClicked();
    void equalClicked();
    void pointClicked();
    void changeSignClicked();
    void backspaceClicked();
    void clear();
    void clearAll();
    void clearMemory();
    void readMemory();
    void setMemory();
    void addToMemory();
//! [0]

//! [1]
private:
//! [1] //! [2]
    Button *createButton(const QString &text, const char *member);
    void abortOperation();
    bool calculate(double rightOperand, const QString &pendingOperator);
//! [2]

//! [3]
    double sumInMemory;
//! [3] //! [4]
    double sumSoFar;
//! [4] //! [5]
    double factorSoFar;
//! [5] //! [6]
    QString pendingAdditiveOperator;
//! [6] //! [7]
    QString pendingMultiplicativeOperator;
//! [7] //! [8]
    bool waitingForOperand;
//! [8]

//! [9]
    QLineEdit *display;
//! [9] //! [10]

    enum { NumDigitButtons = 10 };
    Button *digitButtons[NumDigitButtons];
};
//! [10]

#endif
