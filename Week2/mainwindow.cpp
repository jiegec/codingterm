#include "mainwindow.h"
#include "newgamedialog.h"
#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), languages({"en", "zh"}) {
  setupUi(this);

  tr("en"), tr("zh");
  if (translator.load(":/translations/")) {
    qWarning() << "Loaded translation for default locale";
  } else {
    translator.load(":/translation/en");
    qWarning() << "Using en for fallback locale";
  }
  QApplication::installTranslator(&translator);

  socket = nullptr;
  server = nullptr;

  onNewGame();
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

void MainWindow::onNewGame() {
  NewGameDialog dialog;
  if (dialog.exec() == QDialog::Accepted) {
    if (socket) {
      socket->deleteLater();
      socket = nullptr;
    }
    if (server) {
      server->deleteLater();
      server = nullptr;
    }

    socket = dialog.socket;
    server = dialog.server;
    if (socket) {
      connect(socket, SIGNAL(readyRead()), this, SLOT(onSocketAvailable()));
    }
    emit board->setInitialBoard();
    emit board->setPlayerSide(dialog.side);
    emit board->setRenderSide(dialog.side);
    emit board->setCurrentTurn(SIDE_RED);
    emit board->setSinglePlayer(false);

    if (dialog.side == SIDE_RED) {
      playerSideLabel->setText(tr("Red"));
    } else {
      playerSideLabel->setText(tr("Black"));
    }
    messageLabel->setText("");

    if (dialog.loadFilePath != "") {
      QFile file(dialog.loadFilePath);

      if (!file.open(QFile::ReadOnly)) {
        return;
      }
      QByteArray data = file.readAll();
      emit board->loadBoard(data);
      playerSideLabel->setText(tr("Both"));
    }
  } else {
    emit board->setInitialBoard();
    emit board->setPlayerSide(SIDE_RED);
    playerSideLabel->setText(tr("Both"));
    emit board->setRenderSide(SIDE_RED);
    emit board->setCurrentTurn(SIDE_RED);
    emit board->setSinglePlayer(true);
  }

  numMoves = 0;
}

void MainWindow::onSaveGame() {
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"));
  if (fileName == "") {
    return;
  }

  QFile file(fileName);

  if (!file.open(QFile::WriteOnly)) {
    return;
  }
  QByteArray data = board->dumpBoard();
  file.write(data);
}

void MainWindow::onUserMove(int fromX, int fromY, int toX, int toY) {
  if (socket) {
    QJsonObject json;
    json["fromX"] = fromX;
    json["toX"] = toX;
    json["fromY"] = fromY;
    json["toY"] = toY;

    QJsonDocument doc(json);

    qWarning() << "Sent json" << doc;
    socket->write(doc.toJson());
  }
}

void MainWindow::onSocketAvailable() {
  auto data = socket->readAll();
  QJsonDocument json = QJsonDocument::fromJson(data);
  qWarning() << "Got json" << json;
  if (json["surrender"].toBool()) {
    messageLabel->setText(messageLabel->text() +
                          tr("\nYou opponent has surrendered."));

    if (socket) {
      socket->deleteLater();
      socket = nullptr;
    }
    if (server) {
      server->deleteLater();
      server = nullptr;
    }
    emit board->setSinglePlayer(true);

    QMessageBox msgBox;
    msgBox.setText("Your opponent has surrendered.");
    msgBox.exec();
  } else {
    int fromX = json["fromX"].toInt();
    int fromY = json["fromY"].toInt();
    int toX = json["toX"].toInt();
    int toY = json["toY"].toInt();
    emit board->doMove(fromX, fromY, toX, toY);
  }
}

void MainWindow::onCurrentTurnChanged(int side) {
  if (side == SIDE_RED) {
    currentTurnLabel->setText(tr("Red"));
  } else {
    currentTurnLabel->setText(tr("Black"));
  }
}

void MainWindow::onCheck(int side) {
  if (side == SIDE_RED) {
    messageLabel->setText(messageLabel->text() +
                          tr("\nRed general is checked"));
  } else {
    messageLabel->setText(messageLabel->text() +
                          tr("\nBlack general is checked"));
  }
}

void MainWindow::onCheckmate(int side) {
  if (side == SIDE_RED) {
    messageLabel->setText(messageLabel->text() +
                          tr("\nRed general is checkmated"));
  } else {
    messageLabel->setText(messageLabel->text() +
                          tr("\nBlack general is checkmated"));
  }
  QMessageBox msgBox;
  msgBox.setText(tr("Checkmate!"));
  msgBox.exec();
}

void MainWindow::onSurrender() {
  if (socket) {
    QJsonObject json;
    json["surrender"] = true;

    QJsonDocument doc(json);

    qWarning() << "Sent json" << doc;
    socket->write(doc.toJson());
  }

  messageLabel->setText(messageLabel->text() + tr("\nYou have surrendered."));
  if (socket) {
    socket->deleteLater();
    socket = nullptr;
  }
  if (server) {
    server->deleteLater();
    server = nullptr;
  }
  emit board->setSinglePlayer(true);
  QMessageBox msgBox;
  msgBox.setText("You have surrendered.");
  msgBox.exec();
}

void MainWindow::onTimerChanged(int time) {
  lcdNumber->display(time);
}

void MainWindow::onTimeout() {
  onSurrender();
}