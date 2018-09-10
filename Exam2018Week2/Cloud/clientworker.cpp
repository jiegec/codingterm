#include "clientworker.h"
#include "mainwindow.h"
#include <QThread>

ClientWorker::ClientWorker(MainWindow *mw) : QObject(nullptr) { this->mw = mw; }

void ClientWorker::connect() {
  socket = new QTcpSocket(this);

  socket->connectToHost(QHostAddress::LocalHost, 2018);
  QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));
}

void ClientWorker::sendAction(QString action, QByteArray data) {
  QByteArray buffer;
  QDataStream stream(&buffer, QIODevice::WriteOnly);
  stream << action;
  stream << data;
  QDataStream stream2(socket);
  stream2 << buffer.length();
  socket->write(buffer);
  emit mw->showMessage(
      QString("Sending action to server from thread %1").arg((ptrdiff_t)QThread::currentThread()));
}

void ClientWorker::onDataAvailable() {
  qWarning() << "Client: ready read" << thread();
  while (socket->bytesAvailable()) {
    QDataStream stream(socket);
    QString action;
    QString result;
    stream >> action;
    stream >> result;
    emit mw->finished(action);
    emit mw->showMessage("Got result " + result +
                         QString(" for action %1 from thread %2").arg(action).arg((ptrdiff_t)QThread::currentThread()));
    socket->close();
    QThread::currentThread()->quit();
  }
}