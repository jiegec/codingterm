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
#include <QInputDialog>
#include <QMessageBox>
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) { setupUi(this); }

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

void MainWindow::onOpen() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"));
  if (fileName == "") {
    return;
  }

  QFile file(fileName);

  if (!file.open(QFile::ReadOnly)) {
    return;
  }
  QByteArray byteArray = file.readAll();
  QTextCodec::ConverterState state;
  const char *codecNames[2] = {"GBK", "UTF-8"};
  QString content;
  for (int i = 0; i < 2; i++) {
    QTextCodec *codec = QTextCodec::codecForName(codecNames[i]);
    content = codec->toUnicode(byteArray.constData(), byteArray.size(), &state);
    if (state.invalidChars == 0) {
      break;
    }
  }

  textEdit->setPlainText(content);
}

void MainWindow::onFind() {
  bool ok;
  QString text =
      QInputDialog::getText(this, tr("Find"), tr("String to find:"),
                            QLineEdit::Normal, "", &ok);
  if (ok && text != "") {
    QString content = textEdit->toPlainText();
    int from = 0;
    int count = 0;
    int current = 0;
    while ((current = content.indexOf(text, from)) != -1) {
      from = current + 1;
      count++;
    }

    QMessageBox msgBox;
    msgBox.setText(QString("Found %1 occurrence(s)").arg(count));
    msgBox.exec();
  }
}