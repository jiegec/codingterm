
#include "mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QTextCodec>

const char *fileNames[8] = {"1 do.mp3",  "2 re.mp3", "3 mi.mp3", "4 fa.mp3",
                            "5 sol.mp3", "6 la.mp3", "7 si.mp3"};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setupUi(this);
  for (int i = 0; i < 7; i++) {
    buttons[i] = new QPushButton(this);
    buttons[i]->setText(QString("%1").arg(i + 1));
    verticalLayout->addWidget(buttons[i]);
    mapper.setMapping(buttons[i], i);
    connect(buttons[i], SIGNAL(clicked()), &mapper, SLOT(map()));
  }

  connect(&mapper, SIGNAL(mapped(int)), this, SLOT(onButtonPressed(int)));
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

void MainWindow::onButtonPressed(int index) {
  int note = index + 1;
  text += QString("%1").arg(note);
  textEdit->setPlainText(text);
  textEdit->repaint();

  QMediaPlayer *player = new QMediaPlayer(this);
  connect(player, SIGNAL(stateChanged(QMediaPlayer::State)), this,
          SLOT(onPlayerStateChanged(QMediaPlayer::State)));

  // FIXME: Change it when the location of source code changed
  player->setMedia(QUrl::fromLocalFile(
      QString("/Volumes/Data/oopterm/Exam2018Week1/Player/") +
      fileNames[index]));
  player->setVolume(50);
  player->play();
}

void MainWindow::keyPressEvent(QKeyEvent *e) {
  int key = e->key();
  int number = 0;
  switch (key) {
  case Qt::Key_1:
    number = 1;
    break;
  case Qt::Key_2:
    number = 2;
    break;
  case Qt::Key_3:
    number = 3;
    break;
  case Qt::Key_4:
    number = 4;
    break;
  case Qt::Key_5:
    number = 5;
    break;
  case Qt::Key_6:
    number = 6;
    break;
  case Qt::Key_7:
    number = 7;
    break;
  }
  if (number) {
    int index = number - 1;
    buttons[index]->animateClick();
  }
}

void MainWindow::onPlayerStateChanged(QMediaPlayer::State state) {
  if (state == QMediaPlayer::StoppedState) {
    delete sender();
  }
}