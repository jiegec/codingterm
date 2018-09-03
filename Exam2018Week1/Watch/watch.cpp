#include "watch.h"
#include <QDebug>
#include <QPainter>
#include <cmath>

Watch::Watch(QWidget *parent) : QWidget(parent) { currentTime = 0; }
void Watch::setTime(int value) {
  currentTime = value;
  repaint();
}

void Watch::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  int windowWidth = width();
  painter.translate(width() / 2, height() / 2);

  for (int i = 1; i <= 60; i++) {
    painter.save();

    double x = sin((180 - i * 6.0) / 180 * 3.14159),
           y = cos((180 - i * 6.0) / 180 * 3.14159);
    if (i % 5 == 0) {
      painter.drawText(x * (windowWidth / 2 - 30) - 10,
                       y * (windowWidth / 2 - 30) - 10, 20, 20,
                       Qt::AlignHCenter | Qt::AlignTop, QString("%1").arg(i));
    }

    painter.rotate(i * 360.0 / 60);
    int length = 10;
    int width = 2;
    if (i % 5 == 0) {
      length = 20;
      width = 5;
    }
    QPointF points[4] = {QPointF(-width / 2, -windowWidth / 2),
                         QPointF(width / 2, -windowWidth / 2),
                         QPointF(width / 2, -windowWidth / 2 + length),
                         QPointF(-width / 2, -windowWidth / 2 + length)};
    painter.drawPolygon(points, 4);
    painter.restore();
  }

  painter.save();
  painter.rotate(currentTime / 60.0 * 360);
  qWarning() << currentTime;
  painter.setBrush(QColor("#FF0000"));
  int width = 10;
  QPointF points[4] = {QPointF(-width / 2, 0),
                       QPointF(0, -(windowWidth / 2 - 10)),
                       QPointF(width / 2, 0), QPointF(0, 10)};
  painter.drawPolygon(points, 4);
  painter.restore();

  painter.setBrush(QColor("#000000"));
  painter.drawEllipse(-3, -3, 6, 6);
}