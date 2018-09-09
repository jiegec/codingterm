#include "board.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QMouseEvent>
#include <QNetworkDatagram>
#include <QPainter>
#include <cmath>

Board::Board(QWidget *parent, bool isClient) : QWidget(parent) {
  setMouseTracking(true);

  this->isClient = isClient;
  socket = new QUdpSocket(this);
  socket->bind(QHostAddress::LocalHost, isClient ? 8023 : 8024);

  connect(socket, SIGNAL(readyRead()), this, SLOT(readUdp()));
}

void Board::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  for (auto line : points) {
    painter.drawPolyline(line.constData(), line.size());
  }
}

void Board::mousePressEvent(QMouseEvent *event) {
  QPointF pos = event->localPos();
  isDragging = true;
  QVector<QPointF> vec;
  vec.push_back(pos);
  points.push_back(vec);
  update();

  QByteArray arr;
  QDataStream stream(&arr, QIODevice::WriteOnly);
  stream << pos << true;
  socket->writeDatagram(arr, QHostAddress::LocalHost, isClient ? 8024 : 8023);
}

void Board::mouseMoveEvent(QMouseEvent *event) {
  QPointF pos = event->localPos();
  if (isDragging) {
    points[points.size() - 1].push_back(pos);
    update();
    QByteArray arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    stream << pos << false;
    socket->writeDatagram(arr, QHostAddress::LocalHost, isClient ? 8024 : 8023);
  } else {
  }
}

void Board::mouseReleaseEvent(QMouseEvent *event) {
  QPointF pos = event->localPos();
  if (isDragging) {
    points[points.size() - 1].push_back(pos);
    isDragging = false;
    QByteArray arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    stream << pos << false;
    socket->writeDatagram(arr, QHostAddress::LocalHost, isClient ? 8024 : 8023);
  }
}

void Board::readUdp() {
  while (socket->hasPendingDatagrams()) {
    QNetworkDatagram datagram = socket->receiveDatagram();
    QDataStream stream(datagram.data());
    QPointF pos;
    bool isFirst;
    stream >> pos >> isFirst;
    if (isFirst) {
      QVector<QPointF> vec;
      vec.push_back(pos);
      points.push_back(vec);
    } else {
      if (points.size())
        points[points.size() - 1].push_back(pos);
    }
    qWarning() << pos;
    update();
  }
}