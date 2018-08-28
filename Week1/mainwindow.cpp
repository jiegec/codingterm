#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setupUi(this);
  onSideChanged(5);
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
  emit sideChanged(side);
  sideNumberLabel->setText(QString("%1").arg(side));
}

