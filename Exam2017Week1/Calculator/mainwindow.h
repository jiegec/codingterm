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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QTranslator>
#include <QPushButton>
#include <QSignalMapper>

class MainWindow : public QMainWindow, private Ui::MainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

signals:

protected:
  void changeEvent(QEvent *e) override;

private slots:
  void onNumberPressed(int);
  void onPlusPressed();
  void onMinusPressed();
  void onEqualPressed();

private:
  void doCalc();
  QTranslator translator;
  QSignalMapper mapper;
  QPushButton *numbers[10];
  int currentNumber;
  int stashNumber;
  int currentArith;
  bool newNumber;
};

#endif // MAINWINDOW_H
