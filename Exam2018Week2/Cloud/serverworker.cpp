#include "serverworker.h"
#include "mainwindow.h"
#include <QDataStream>
#include <QThread>
#include <QTimer>
#include <algorithm>

ServerWorker::ServerWorker(MainWindow *mw) : QObject(nullptr) { this->mw = mw; }

void ServerWorker::setFd(qintptr fd) {
  current = new QTcpSocket();
  qWarning() << "socket in thread" << thread();
  Q_ASSERT(current->setSocketDescriptor(fd));
  connect(current, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));
  next_block_size = 0;
}

void ServerWorker::onDataAvailable() {
  qWarning() << "Server: ready read" << thread();
  while (true) {
    if (!next_block_size) {
      if (current->bytesAvailable() < sizeof(quint16)) {
        break;
      }
      QDataStream stream(current);
      uint len = 0;
      stream >> len;
      next_block_size = len;
    }
    if (current->bytesAvailable() < next_block_size) {
      break;
    }

    QByteArray data;
    data = current->read(next_block_size);
    QDataStream stream(data);
    QString action;
    stream >> action;
    QByteArray content;
    stream >> content;
    next_block_size = 0;
    emit mw->showMessage("Got action " + action + " and content size " +
                         QString("%1 from thread %2")
                             .arg(content.size())
                             .arg((ptrdiff_t)QThread::currentThread()));
    if (content.size() < 0) {
      continue;
    }
    QString text(content);
    QStringList lines = text.split("\n");
    int result_min = 0x7fffffff, result_max = -0x7fffffff;
    double result_avg = 0.0;
    QString result;
    int count = 0;
    for (auto line : lines) {
      if (line == "")
        continue;
      count++;
      int number = line.toInt();
      if (action == "min") {
        result_min = result_min < number ? result_min : number;
      } else if (action == "max") {
        result_max = result_max > number ? result_max : number;
      } else if (action == "average") {
        result_avg += number;
      }
    }
    if (action == "average") {
      result_avg /= count;
      result = QString("%1").arg(result_avg);
    } else if (action == "medium") {
      QVector<int> vec;
      for (auto line : lines) {
        if (line == "")
          continue;
        vec.push_back(line.toInt());
      }
      std::sort(vec.begin(), vec.end());
      if (vec.size() % 2) {
        result = QString("%1").arg(vec[vec.size() / 2]);
      } else {
        result = QString("%1").arg(
            (float(vec[vec.size() / 2]) + vec[vec.size() / 2 - 1]) / 2);
      }
    } else if (action == "min") {
      result = QString("%1").arg(result_min);
    } else {
      result = QString("%1").arg(result_max);
    }

    QDataStream stream2(current);
    stream2 << action;
    stream2 << result;
    emit mw->showMessage(QString("Sending result %1 to client from thread %2")
                             .arg(result)
                             .arg((ptrdiff_t)QThread::currentThread()));
  }
}