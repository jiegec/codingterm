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

#include "settingsdialog.h"
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {

  setupUi(this);
  onSideChanged(5);

  onInputCol1Changed(1);
  onInputCol2Changed(1);
  onOutputCol1Changed(1);
  onOutputCol2Changed(1);
  onOutputCol3Changed(1);
}

void SettingsDialog::onSideChanged(int value) {
  side = value;
  sideLabel->setText(QString("%1").arg(side));
  emit sideChanged(side);
}

void SettingsDialog::onInputCol1Changed(int value) {
  inputCol[0] = value - 1;
  input1Label->setText(QString("%1").arg(value));
}
void SettingsDialog::onInputCol2Changed(int value) {
  inputCol[1] = value - 1;
  input2Label->setText(QString("%1").arg(value));
}
void SettingsDialog::onOutputCol1Changed(int value) {
  outputCol[0] = value - 1;
  output1Label->setText(QString("%1").arg(value));
}
void SettingsDialog::onOutputCol2Changed(int value) {
  outputCol[1] = value - 1;
  output2Label->setText(QString("%1").arg(value));
}
void SettingsDialog::onOutputCol3Changed(int value) {
  outputCol[2] = value - 1;
  output3Label->setText(QString("%1").arg(value));
}

void SettingsDialog::onDone() {
  QMessageBox msgbox;
  if (side < 5 || side > 8) {
    msgbox.setText("Invalid side");
    msgbox.exec();
    return;
  }
  if (inputCol[0] == inputCol[1]) {
    msgbox.setText("Invalid input column");
    msgbox.exec();
    return;
  }
  if (inputCol[0] >= side || inputCol[1] >= side) {
    msgbox.setText("Invalid input column");
    msgbox.exec();
    return;
  }
  if (outputCol[0] == outputCol[1] || outputCol[1] == outputCol[2] ||
      outputCol[0] == outputCol[2]) {
    msgbox.setText("Invalid output column");
    msgbox.exec();
    return;
  }
  if (outputCol[0] >= side || outputCol[1] >= side) {
    msgbox.setText("Invalid output column");
    msgbox.exec();
    return;
  }
  accept();
}