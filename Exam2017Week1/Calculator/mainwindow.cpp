// Copyright (C) 2018 Jiajie Chen
//
// This file is part of Reader.
//
// Reader is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Reader is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Reader.  If not, see <http://www.gnu.org/licenses/>.
//

#include "mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>

#define ARITH_NONE 0
#define ARITH_PLUS 1
#define ARITH_MINUS 2
#define ARITH_EQUAL 3

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setupUi(this);

  for (int i = 2; i >= 0; i--) {
    for (int j = 1; j <= 3; j++) {
      int number = i * 3 + j;
      numbers[number] = new QPushButton(this);
      gridLayout->addWidget(numbers[number], 2 - i, j - 1);
    }
  }
  numbers[0] = new QPushButton(this);
  gridLayout->addWidget(numbers[0], 3, 0, 1, 3);

  for (int i = 0; i < 10; i++) {
    QString text = QString("%1").arg(i);
    numbers[i]->setMinimumSize(QSize(50, 50));
    numbers[i]->setText(text);
    mapper.setMapping(numbers[i], i);
    connect(numbers[i], SIGNAL(clicked()), &mapper, SLOT(map()));
  }
  connect(&mapper, SIGNAL(mapped(int)), this, SLOT(onNumberPressed(int)));

  currentNumber = 0;
  currentArith = 0;
  stashNumber = 0;
  newNumber = true;
}

void MainWindow::changeEvent(QEvent *e) {
  QWidget::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    retranslateUi(this);
    break;
  default:
    break;
  }
}

void MainWindow::onNumberPressed(int num) {
  if (newNumber) {
    currentNumber = num;
    newNumber = false;
  } else {
    currentNumber = currentNumber * 10 + num;
  }
  emit lcdNumber->display(currentNumber);
  update();
}

void MainWindow::onPlusPressed() {
  doCalc();
  currentArith = ARITH_PLUS;
  stashNumber = currentNumber;
  emit lcdNumber->display(currentNumber);
  repaint();
}

void MainWindow::onMinusPressed() {
  doCalc();
  currentArith = ARITH_MINUS;
  stashNumber = currentNumber;
  emit lcdNumber->display(currentNumber);
  repaint();
}

void MainWindow::onEqualPressed() {
  doCalc();
  currentArith = ARITH_EQUAL;
  stashNumber = currentNumber;
  emit lcdNumber->display(currentNumber);
  repaint();
}

void MainWindow::doCalc() {
  if (currentArith == ARITH_PLUS && !newNumber) {
    currentNumber += stashNumber;
  } else if (currentArith == ARITH_MINUS && !newNumber) {
    currentNumber = stashNumber - currentNumber;
  } else if (currentArith == ARITH_EQUAL && !newNumber) {
    stashNumber = 0;
  }
  currentArith = ARITH_NONE;
  newNumber = true;
}