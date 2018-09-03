#include "mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setupUi(this);

  currentTime = 0;
  setTime(currentTime);

  connect(&timer, SIGNAL(timeout()), this, SLOT(onTick()));

  startButton->setEnabled(true);
  pauseButton->setEnabled(false);
  resetButton->setEnabled(true);

  isPausing = false;
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

void MainWindow::onStart() {
  timer.start(1000);

  startButton->setEnabled(false);
  pauseButton->setEnabled(true);
  resetButton->setEnabled(false);

  repaint();
}

void MainWindow::onPause() {
  if (isPausing) {
    isPausing = false;
    timer.start(1000);
    startButton->setEnabled(false);
    pauseButton->setEnabled(true);
    resetButton->setEnabled(false);
    pauseButton->setText("Pause");

  } else {
    isPausing = true;
    timer.stop();

    startButton->setEnabled(false);
    pauseButton->setEnabled(true);
    resetButton->setEnabled(true);
    pauseButton->setText("Resume");
  }

  repaint();
}

void MainWindow::onReset() {
  currentTime = 0;
  setTime(currentTime);
  isPausing = false;
  startButton->setEnabled(true);
  pauseButton->setEnabled(false);
  resetButton->setEnabled(false);
  pauseButton->setText("Pause");

  repaint();
}

void MainWindow::onTick() {
  currentTime += 1;
  setTime(currentTime);
}

void MainWindow::setTime(int time) {
  lcdNumber->display(time);
  watch->setTime(time);
  repaint();
}