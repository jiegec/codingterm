#include "serverworker.h"
#include "mainwindow.h"
#include <QDataStream>
#include <QThread>
#include <QTimer>

ServerWorker::ServerWorker(MainWindow *mw) : QObject(nullptr) { this->mw = mw; }

void ServerWorker::setFd(qintptr fd) {
  current = new QTcpSocket();
  qWarning() << "socket in thread" << thread();
  Q_ASSERT(current->setSocketDescriptor(fd));
  connect(current, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));
}

void ServerWorker::setAnother(ServerWorker *another) {
  this->another = another;
}

void ServerWorker::sendDataTo(const QString &fileName,
                              const QByteArray &array) {
  qWarning() << "Server: sendDataTo" << fileName << array;
  QDataStream stream(current);
  stream << QString("saveFile");
  stream << fileName;
  stream << array;
}

void ServerWorker::allowSendFile() {
  qWarning() << "Server: allowSendFile";
  QDataStream stream(current);
  stream << QString("allowSendFile");
}

void ServerWorker::onDataAvailable() {
  qWarning() << "Server: ready read" << thread();
  while (current->bytesAvailable()) {
    QDataStream stream(current);
    QString action;
    stream >> action;
    qWarning() << "Server: Got action" << action;
    if (action == "sendFile") {
      QString fileName;
      QByteArray data;
      stream >> fileName;
      stream >> data;
      QTimer::singleShot(0, another,
                         [=] { another->sendDataTo(fileName, data); });
      emit mw->showMessage("Got file " + fileName +
                           "from user, sending to another one");
    } else if (action == "sendFileDone") {
      QTimer::singleShot(0, another, &ServerWorker::allowSendFile);
    }
  }
}