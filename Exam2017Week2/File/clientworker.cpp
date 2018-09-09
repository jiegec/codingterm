#include "clientworker.h"
#include "mainwindow.h"

ClientWorker::ClientWorker(MainWindow *mw) : QObject(nullptr) { this->mw = mw; }

void ClientWorker::connect() {
  socket = new QTcpSocket(this);

  emit mw->showMessage("Connecting to server");
  socket->connectToHost(QHostAddress::LocalHost, 1911);
  QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));
}

void ClientWorker::sendFile(QString fileName, QByteArray data) {
  QDataStream stream(socket);
  stream << QString("sendFile");
  stream << fileName;
  stream << data;
  emit mw->showMessage("Sending file to server");
}

void ClientWorker::onDataAvailable() {
  while (socket->bytesAvailable()) {
    QDataStream stream(socket);
    QString action;
    stream >> action;
    qWarning() << "Client: Got action" << action;
    if (action == "saveFile") {
      QString fileName;
      QByteArray data;
      stream >> fileName;
      stream >> data;
      mw->showMessage("Saving file to " + fileName);
      qWarning() << "Client: received data" << data;
      emit saveFile(fileName, data);
      stream << QString("sendFileDone");
    } else if (action == "allowSendFile") {
      emit allowSendFile();
    }
  }
}