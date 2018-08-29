// Copyright (C) 2018 Jiajie Chen
//
// This file is part of Week1.
//
// Week1 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Week1 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Week1.  If not, see <http://www.gnu.org/licenses/>.
//

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setupUi(this);
  onSideChanged(5);
  onTargetOutputFlow1Changed(0);
  onTargetOutputFlow2Changed(0);
  onTargetOutputFlow3Changed(0);
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

void MainWindow::onSideChanged(int value) {
  side = value;
  sideNumberLabel->setText(tr("%1").arg(side));
  emit sideChanged(side);
}

void MainWindow::onTargetOutputFlow1Changed(int value) {
  output1Label->setText(tr("%1").arg(value));
}
void MainWindow::onTargetOutputFlow2Changed(int value) {
  output2Label->setText(tr("%1").arg(value));
}
void MainWindow::onTargetOutputFlow3Changed(int value) {
  output3Label->setText(tr("%1").arg(value));
}