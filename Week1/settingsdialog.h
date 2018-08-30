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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "ui_settingsdialog.h"
#include <QMainWindow>
#include <QTranslator>

class SettingsDialog : public QDialog, private Ui::SettingsDialog {
  Q_OBJECT

public:
  explicit SettingsDialog(QWidget *parent = nullptr);

signals:
  void sideChanged(int);

private slots:
  void onSideChanged(int);
  void onInputCol1Changed(int);
  void onInputCol2Changed(int);
  void onOutputCol1Changed(int);
  void onOutputCol2Changed(int);
  void onOutputCol3Changed(int);
  void onDone();

private:
  int side;
  int inputCol[2];
  int outputCol[3];

  friend class MainWindow;
};

#endif // MAINWINDOW_H