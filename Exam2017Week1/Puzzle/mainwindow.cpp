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
#include <QKeyEvent>
#include <QMessageBox>
#include <cstdlib>
#include <ctime>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setupUi(this);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      int number = i * 3 + j;
      buttons[number] = new QPushButton(this);
      textfields[number] = new QLineEdit(this);
      textfields[number]->setVisible(false);
      layouts[number] = new QStackedLayout(this);
      layouts[number]->addWidget(buttons[number]);
      layouts[number]->addWidget(textfields[number]);
      gridLayout->addLayout(layouts[number], i, j);
    }
  }

  srand(time(nullptr));
  for (int i = 0; i < 9; i++) {
    state[i] = i + 1;
  }
  state[8] = 0;

  for (int i = 2; i < 9; i++) {
    int index = rand() % i;
    int temp = state[i];
    state[i] = state[index];
    state[index] = temp;
  }

  for (int i = 0; i < 9; i++) {
    origState[i] = state[i];
  }

  for (int i = 0; i < 9; i++) {
    buttons[i]->setMinimumSize(QSize(50, 50));
    mapper.setMapping(buttons[i], i);
    connect(buttons[i], SIGNAL(clicked()), &mapper, SLOT(map()));
  }
  connect(&mapper, SIGNAL(mapped(int)), this, SLOT(onButtonPressed(int)));
  currentHighlighted = -1;

  isCustomizing = false;
  updateBoard();
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

void MainWindow::updateBoard() {
  for (int i = 0; i < 9; i++) {
    textfields[i]->setText(QString("%1").arg(state[i]));
    if (state[i]) {
      buttons[i]->setText(QString("%1").arg(state[i]));
      if (i == currentHighlighted) {
        buttons[i]->setStyleSheet("border: 1px solid #FFEB3B;");
      } else {
        buttons[i]->setStyleSheet("");
      }
      buttons[i]->setEnabled(true);
    } else {
      buttons[i]->setText("");
      buttons[i]->setStyleSheet("");
      buttons[i]->setEnabled(false);
    }
    buttons[i]->repaint();
  }
}

void MainWindow::onButtonPressed(int index) {
  qWarning() << index;
  if (index != currentHighlighted) {
    int dir[4] = {-3, -1, 1, 3};
    bool flag = false;
    for (int i = 0; i < 4; i++) {
      int neigh = index + dir[i];
      if (0 <= neigh && neigh < 9 && state[neigh] == 0) {
        // not the same row, no
        if (abs(dir[i]) == 1 && (neigh / 3) != (index / 3)) {
          continue;
        }
        // empty slot here
        flag = true;
        currentHighlighted = index;
      }
    }

    if (!flag) {
      currentHighlighted = -1;
    }
    updateBoard();
  }
}

void MainWindow::keyReleaseEvent(QKeyEvent *e) {
  int key = e->key();
  if (currentHighlighted == -1) {
    return;
  }
  int row = currentHighlighted / 3;
  int col = currentHighlighted % 3;
  switch (key) {
  case Qt::Key_Left:
    if (col != 0 && state[currentHighlighted - 1] == 0) {
      state[currentHighlighted - 1] = state[currentHighlighted];
      state[currentHighlighted] = 0;
      currentHighlighted -= 1;
    }
    break;
  case Qt::Key_Right:
    if (col != 2 && state[currentHighlighted + 1] == 0) {
      state[currentHighlighted + 1] = state[currentHighlighted];
      state[currentHighlighted] = 0;
      currentHighlighted += 1;
    }
    break;
  case Qt::Key_Up:
    if (row != 0 && state[currentHighlighted - 3] == 0) {
      state[currentHighlighted - 3] = state[currentHighlighted];
      state[currentHighlighted] = 0;
      currentHighlighted -= 3;
    }
    break;
  case Qt::Key_Down:
    if (row != 2 && state[currentHighlighted + 3] == 0) {
      state[currentHighlighted + 3] = state[currentHighlighted];
      state[currentHighlighted] = 0;
      currentHighlighted += 3;
    }
    break;
  default:
    return;
    break;
  }

  bool success = true;
  for (int i = 0; i < 8; i++) {
    if (state[i] != i + 1) {
      success = false;
      break;
    }
  }

  if (success) {
    QMessageBox msgBox;
    msgBox.setText("Success!");
    msgBox.exec();
  }

  updateBoard();
}

void MainWindow::onReplay() {
  for (int i = 0; i < 9; i++) {
    state[i] = origState[i];
  }
  currentHighlighted = -1;
  updateBoard();
}

void MainWindow::onCustomize() {
  if (!isCustomizing) {
    isCustomizing = true;
    customizeButton->setText("Finish");
    for (int i = 0; i < 9; i++) {
      layouts[i]->setCurrentIndex(1);
      textfields[i]->setVisible(true);
      buttons[i]->setVisible(false);
    }
    repaint();
  } else {
    int numbers[9] = {0};
    for (int i = 0; i < 9; i++) {
      numbers[i] = textfields[i]->text().toInt();
      if (numbers[i] < 0 || numbers[i] > 8) {
        QMessageBox msgBox;
        msgBox.setText("Invalid number! Should be 0-9.");
        msgBox.exec();
        return;
      }

      bool flag = false;
      for (int j = 0; j < i; j++) {
        if (numbers[i] == numbers[j]) {
          QMessageBox msgBox;
          msgBox.setText("Numbers can not be the same!");
          msgBox.exec();
          return;
        }
      }
    }
    for (int i = 0; i < 9; i++) {
      state[i] = numbers[i];
    }
    currentHighlighted = -1;

    isCustomizing = false;
    customizeButton->setText("Customize");
    for (int i = 0; i < 9; i++) {
      layouts[i]->setCurrentIndex(0);
      textfields[i]->setVisible(false);
      buttons[i]->setVisible(true);
    }
    updateBoard();
  }
}