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
#include <QPushButton>
#include <QSignalMapper>
#include <QTranslator>
#include <QLineEdit>
#include <QStackedLayout>

class MainWindow : public QMainWindow, private Ui::MainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

signals:

protected:
  void changeEvent(QEvent *e) override;
  void keyReleaseEvent(QKeyEvent *e) override;

private slots:
  void onButtonPressed(int);
  void onReplay();
  void onCustomize();

private:
  void updateBoard();
  QTranslator translator;
  QSignalMapper mapper;
  QPushButton *buttons[9];
  QLineEdit *textfields[9];
  QStackedLayout *layouts[9];
  int state[9];
  int origState[9];
  int currentHighlighted;
  bool isCustomizing;
};

#endif // MAINWINDOW_H
