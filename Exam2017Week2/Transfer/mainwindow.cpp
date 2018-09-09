
#include "mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setupUi(this);
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
