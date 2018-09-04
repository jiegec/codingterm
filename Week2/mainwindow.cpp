#include "mainwindow.h"
#include "newgamedialog.h"
#include <QDebug>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>

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
      delete socket;
    }
    if (server) {
      delete server;
    }

    socket = dialog.socket;
    server = dialog.server;
    connect(socket, SIGNAL(readyRead()), this, SLOT(onSocketAvailable()));
    emit board->setInitialBoard();
    emit board->setPlayerSide(dialog.side);
    emit board->setRenderSide(dialog.side);
    emit board->setCurrentTurn(SIDE_RED);

    if (dialog.side == SIDE_RED) {
      playerSideLabel->setText(tr("Red"));
    } else {
      playerSideLabel->setText(tr("Black"));
    }
  }
  numMoves = 0;
}

void MainWindow::onSaveGame() {}

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
  int fromX = json["fromX"].toInt();
  int fromY = json["fromY"].toInt();
  int toX = json["toX"].toInt();
  int toY = json["toY"].toInt();
  emit board->doMove(fromX, fromY, toX, toY);
}

void MainWindow::onCurrentTurnChanged(int side) {
  if (side == SIDE_RED) {
    currentTurnLabel->setText(tr("Red"));
  } else {
    currentTurnLabel->setText(tr("Black"));
  }
}