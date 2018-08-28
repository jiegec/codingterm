#include "chip.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <cmath>

#define DEFAULT_WIDTH 200
#define BUFFER_HEIGHT 500
#define LENGTH 1600
#define SCALE 0.036
#define OFFSET (LENGTH + DEFAULT_WIDTH)

Chip::Chip(QWidget *parent, int side) : QWidget(parent), side(side) {
  for (int i = 0; i < INPUT_NUM; i++) {
    inputCol[i] = i;
    inputWidth[i] = DEFAULT_WIDTH * (i + 1);
  }
  for (int i = 0; i < OUTPUT_NUM; i++) {
    outputCol[i] = i;
    outputWidth[i] = DEFAULT_WIDTH * (i + 1);
  }
  for (int i = 0; i <= 8; i++) {
    for (int j = 0; j <= 8; j++) {
      width_v[i][j] = DEFAULT_WIDTH * 1;
      width_h[i][j] = DEFAULT_WIDTH * 4.2;
    }
  }
  setMouseTracking(true);
}

void draw_vertical(QPainter &painter, int width) {
  {
    // main body
    const QPointF points[4] = {
        QPointF(-width / 2, BUFFER_HEIGHT + DEFAULT_WIDTH / 2),
        QPointF(-width / 2, LENGTH - BUFFER_HEIGHT + DEFAULT_WIDTH / 2),
        QPointF(width / 2, LENGTH - BUFFER_HEIGHT + DEFAULT_WIDTH / 2),
        QPointF(width / 2, BUFFER_HEIGHT + DEFAULT_WIDTH / 2)};
    painter.drawPolygon(points, 4);
  }
  {
    // top block
    const QPointF points[4] = {
        QPointF(-DEFAULT_WIDTH / 2, DEFAULT_WIDTH / 2),
        QPointF(-DEFAULT_WIDTH / 2, BUFFER_HEIGHT + DEFAULT_WIDTH / 2),
        QPointF(DEFAULT_WIDTH / 2, BUFFER_HEIGHT + DEFAULT_WIDTH / 2),
        QPointF(DEFAULT_WIDTH / 2, DEFAULT_WIDTH / 2)};
    painter.drawPolygon(points, 4);
  }
  {
    // bottom block
    const QPointF points[4] = {
        QPointF(-DEFAULT_WIDTH / 2, LENGTH + DEFAULT_WIDTH / 2),
        QPointF(-DEFAULT_WIDTH / 2, LENGTH - BUFFER_HEIGHT + DEFAULT_WIDTH / 2),
        QPointF(DEFAULT_WIDTH / 2, LENGTH - BUFFER_HEIGHT + DEFAULT_WIDTH / 2),
        QPointF(DEFAULT_WIDTH / 2, LENGTH + DEFAULT_WIDTH / 2)};
    painter.drawPolygon(points, 4);
  }
  // top arc
  painter.drawPie(QRectF(DEFAULT_WIDTH - width / 2, DEFAULT_WIDTH / 2,
                         width - DEFAULT_WIDTH, BUFFER_HEIGHT * 2),
                  0 * 16, 90 * 16);
  painter.drawPie(QRectF(-width / 2, DEFAULT_WIDTH / 2, width - DEFAULT_WIDTH,
                         BUFFER_HEIGHT * 2),
                  90 * 16, 90 * 16);
  // bottom arc
  painter.drawPie(QRectF(-width / 2,
                         LENGTH + DEFAULT_WIDTH / 2 - BUFFER_HEIGHT * 2,
                         width - DEFAULT_WIDTH, BUFFER_HEIGHT * 2),
                  180 * 16, 90 * 16);
  painter.drawPie(QRectF(DEFAULT_WIDTH - width / 2,
                         LENGTH + DEFAULT_WIDTH / 2 - BUFFER_HEIGHT * 2,
                         width - DEFAULT_WIDTH, BUFFER_HEIGHT * 2),
                  270 * 16, 90 * 16);
}

void draw_horizontal(QPainter &painter, int width) {

  {
    // main body
    const QPointF points[4] = {
        QPointF(BUFFER_HEIGHT + DEFAULT_WIDTH / 2, -width / 2),
        QPointF(LENGTH - BUFFER_HEIGHT + DEFAULT_WIDTH / 2, -width / 2),
        QPointF(LENGTH - BUFFER_HEIGHT + DEFAULT_WIDTH / 2, width / 2),
        QPointF(BUFFER_HEIGHT + DEFAULT_WIDTH / 2, width / 2)};
    painter.drawPolygon(points, 4);
  }
  {
    // top block
    const QPointF points[4] = {
        QPointF(DEFAULT_WIDTH / 2, -DEFAULT_WIDTH / 2),
        QPointF(BUFFER_HEIGHT + DEFAULT_WIDTH / 2, -DEFAULT_WIDTH / 2),
        QPointF(BUFFER_HEIGHT + DEFAULT_WIDTH / 2, DEFAULT_WIDTH / 2),
        QPointF(DEFAULT_WIDTH / 2, DEFAULT_WIDTH / 2)};
    painter.drawPolygon(points, 4);
  }
  {
    // bottom block
    const QPointF points[4] = {
        QPointF(LENGTH + DEFAULT_WIDTH / 2, -DEFAULT_WIDTH / 2),
        QPointF(LENGTH - BUFFER_HEIGHT + DEFAULT_WIDTH / 2, -DEFAULT_WIDTH / 2),
        QPointF(LENGTH - BUFFER_HEIGHT + DEFAULT_WIDTH / 2, DEFAULT_WIDTH / 2),
        QPointF(LENGTH + DEFAULT_WIDTH / 2, DEFAULT_WIDTH / 2)};
    painter.drawPolygon(points, 4);
  }
  // top arc
  painter.drawPie(QRectF(DEFAULT_WIDTH / 2, DEFAULT_WIDTH - width / 2,
                         BUFFER_HEIGHT * 2, width - DEFAULT_WIDTH),
                  180 * 16, 90 * 16);
  painter.drawPie(QRectF(DEFAULT_WIDTH / 2, -width / 2, BUFFER_HEIGHT * 2,
                         width - DEFAULT_WIDTH),
                  90 * 16, 90 * 16);
  // bottom arc
  painter.drawPie(QRectF(LENGTH + DEFAULT_WIDTH / 2 - BUFFER_HEIGHT * 2,
                         -width / 2, BUFFER_HEIGHT * 2, width - DEFAULT_WIDTH),
                  0 * 16, 90 * 16);
  painter.drawPie(QRectF(LENGTH + DEFAULT_WIDTH / 2 - BUFFER_HEIGHT * 2,
                         DEFAULT_WIDTH - width / 2, BUFFER_HEIGHT * 2,
                         width - DEFAULT_WIDTH),
                  270 * 16, 90 * 16);
}

void Chip::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.scale(SCALE, SCALE);
  painter.setPen(Qt::NoPen);
  painter.translate(OFFSET, OFFSET);

  painter.setBrush(QColor("#FFC107"));
  for (int i = 0; i <= side; i++) {
    for (int j = 0; j <= side; j++) {
      painter.save();
      painter.translate(i * (LENGTH + DEFAULT_WIDTH),
                        j * (LENGTH + DEFAULT_WIDTH));
      const QPointF points[4] = {
          QPointF(-DEFAULT_WIDTH / 2, -DEFAULT_WIDTH / 2),
          QPointF(-DEFAULT_WIDTH / 2, DEFAULT_WIDTH / 2),
          QPointF(DEFAULT_WIDTH / 2, DEFAULT_WIDTH / 2),
          QPointF(DEFAULT_WIDTH / 2, -DEFAULT_WIDTH / 2)};
      painter.drawPolygon(points, 4);
      painter.restore();
    }
  }

  painter.setBrush(QColor("#9C27B0"));
  for (int i = 0; i <= side; i++) {
    for (int j = 0; j < side; j++) {
      painter.save();
      painter.translate(i * (LENGTH + DEFAULT_WIDTH),
                        j * (LENGTH + DEFAULT_WIDTH));
      draw_vertical(painter, width_v[i][j]);
      painter.restore();
    }
  }

  for (int i = 0; i < INPUT_NUM; i++) {
    painter.save();
    painter.translate(inputCol[i] * (LENGTH + DEFAULT_WIDTH),
                      -1 * (LENGTH + DEFAULT_WIDTH));
    draw_vertical(painter, inputWidth[i]);
    painter.restore();
  }

  painter.setBrush(QColor("#03A9F4"));
  for (int i = 0; i < side; i++) {
    for (int j = 0; j <= side; j++) {
      painter.save();
      painter.translate(i * (LENGTH + DEFAULT_WIDTH),
                        j * (LENGTH + DEFAULT_WIDTH));
      draw_horizontal(painter, width_h[i][j]);
      painter.restore();
    }
  }

  for (int i = 0; i < OUTPUT_NUM; i++) {
    painter.save();
    painter.translate(outputCol[i] * (LENGTH + DEFAULT_WIDTH),
                      side * (LENGTH + DEFAULT_WIDTH));
    draw_vertical(painter, outputWidth[i]);
    painter.restore();
  }
}

void Chip::onSideChanged(int value) {
  if (value < side) {
    for (int i = 0; i < INPUT_NUM; i++) {
      if (inputCol[i] > value) {
        for (int j = 0; j <= value; j++) {
          int k;
          for (k = 0; k < INPUT_NUM; k++) {
            if (inputCol[k] == j) {
              break;
            }
          }
          if (k == INPUT_NUM) {
            inputCol[i] = j;
            break;
          }
        }
      }
    }
    for (int i = 0; i < OUTPUT_NUM; i++) {
      if (outputCol[i] > value) {
        for (int j = 0; j <= value; j++) {
          int k;
          for (k = 0; k < OUTPUT_NUM; k++) {
            if (outputCol[k] == j) {
              break;
            }
          }
          if (k == OUTPUT_NUM) {
            outputCol[i] = j;
            break;
          }
        }
      }
    }
  }
  side = value;
  emit update();
}

#define NOTFOUND 0
#define TYPE_V 1
#define TYPE_H 2
#define TYPE_INPUT 3
#define TYPE_OUTPUT 4

int Chip::convertPos(int x, int y, int &res_i, int &res_j) {
  // vertical
  for (int i = 0; i <= side; i++) {
    for (int j = 0; j < side; j++) {
      int xx = OFFSET + i * (LENGTH + DEFAULT_WIDTH);
      int yy = OFFSET + j * (LENGTH + DEFAULT_WIDTH);
      if ((xx - width_v[i][j] / 2) * SCALE <= x &&
          x <= (xx + width_v[i][j] / 2) * SCALE &&
          (yy + DEFAULT_WIDTH / 2) * SCALE <= y &&
          y <= (yy + LENGTH - DEFAULT_WIDTH / 2) * SCALE) {
        res_i = i;
        res_j = j;
        return TYPE_V;
      }
    }
  }

  // horizontal
  for (int i = 0; i < side; i++) {
    for (int j = 0; j <= side; j++) {
      int xx = OFFSET + i * (LENGTH + DEFAULT_WIDTH);
      int yy = OFFSET + j * (LENGTH + DEFAULT_WIDTH);
      if ((xx + DEFAULT_WIDTH / 2) * SCALE <= x &&
          x <= (xx + LENGTH - DEFAULT_WIDTH / 2) * SCALE &&
          (yy - width_h[i][j] / 2) * SCALE <= y &&
          y <= (yy + width_h[i][j] / 2) * SCALE) {
        res_i = i;
        res_j = j;
        return TYPE_H;
      }
    }
  }

  // input
  for (int i = 0; i < INPUT_NUM; i++) {
    int xx = OFFSET + inputCol[i] * (LENGTH + DEFAULT_WIDTH);
    int yy = OFFSET + (-1) * (LENGTH + DEFAULT_WIDTH);
    if ((xx - inputWidth[i] / 2) * SCALE <= x &&
        x <= (xx + inputWidth[i] / 2) * SCALE &&
        (yy + DEFAULT_WIDTH / 2) * SCALE <= y &&
        y <= (yy + LENGTH - DEFAULT_WIDTH / 2) * SCALE) {
      res_i = i;
      return TYPE_INPUT;
    }
  }

  // output
  for (int i = 0; i < OUTPUT_NUM; i++) {
    int xx = OFFSET + outputCol[i] * (LENGTH + DEFAULT_WIDTH);
    int yy = OFFSET + side * (LENGTH + DEFAULT_WIDTH);
    if ((xx - outputWidth[i] / 2) * SCALE <= x &&
        x <= (xx + outputWidth[i] / 2) * SCALE &&
        (yy + DEFAULT_WIDTH / 2) * SCALE <= y &&
        y <= (yy + LENGTH - DEFAULT_WIDTH / 2) * SCALE) {
      res_i = i;
      return TYPE_OUTPUT;
    }
  }

  return 0;
}

void Chip::mouseMoveEvent(QMouseEvent *event) {
  const QPointF pos = event->localPos();
  if (isMouseDown) {
    // find nearest available place
    if (isHoldingInput) {
      int min_xoff = 0x7fffffff, min_place;
      for (int i = 0; i <= side; i++) {
        int j;
        for (j = 0; j < INPUT_NUM; j++) {
          if (inputCol[j] == i && j != holdingInputIndex) {
            // this is occupied.
            break;
          }
        }
        if (j == INPUT_NUM) {
          // empty slot
          int xx = OFFSET + i * (LENGTH + DEFAULT_WIDTH);
          int xoff = fabs(xx * SCALE - pos.x());
          if (xoff < min_xoff) {
            min_xoff = xoff;
            min_place = i;
          }
        }
      }
      // min_place must exist
      inputCol[holdingInputIndex] = min_place;
      update();
    } else if (isHoldingOutput) {
      int min_xoff = 0x7fffffff, min_place;
      for (int i = 0; i <= side; i++) {
        int j;
        for (j = 0; j < OUTPUT_NUM; j++) {
          if (outputCol[j] == i && j != holdingOutputIndex) {
            // this is occupied.
            break;
          }
        }
        if (j == OUTPUT_NUM) {
          // empty slot
          int xx = OFFSET + i * (LENGTH + DEFAULT_WIDTH);
          int xoff = fabs(xx * SCALE - pos.x());
          if (xoff < min_xoff) {
            min_xoff = xoff;
            min_place = i;
          }
        }
      }
      // min_place must exist
      outputCol[holdingOutputIndex] = min_place;
      update();
    }
  } else {
    int i = 0, j = 0;
    int result = convertPos(pos.x(), pos.y(), i, j);
    switch (result) {
    case TYPE_V:
    case TYPE_H:
      setCursor(Qt::PointingHandCursor);
      break;
    case TYPE_INPUT:
    case TYPE_OUTPUT:
      setCursor(Qt::OpenHandCursor);
      break;
    default:
      setCursor(Qt::ArrowCursor);
      break;
    }
  }
}
void Chip::mousePressEvent(QMouseEvent *event) {
  isMouseDown = true;
  mouseDownPos = event->localPos();

  const QPointF pos = event->localPos();
  int i = 0, j = 0;
  int result = convertPos(pos.x(), pos.y(), i, j);
  if (result) {
    qWarning() << "press" << convertPos(pos.x(), pos.y(), i, j) << i << j;
    isHoldingInput = (result == TYPE_INPUT);
    holdingInputIndex = i;
    isHoldingOutput = (result == TYPE_OUTPUT);
    holdingOutputIndex = i;
    if (isHoldingInput || isHoldingOutput) {
      setCursor(Qt::ClosedHandCursor);
    }
  }
}
void Chip::mouseReleaseEvent(QMouseEvent *event) {
  isMouseDown = false;
  setCursor(Qt::ArrowCursor);

  const QPointF pos = event->localPos();
  int i = 0, j = 0;
  int result = convertPos(pos.x(), pos.y(), i, j);
  if (result) {
    qWarning() << "release" << convertPos(pos.x(), pos.y(), i, j) << i << j;
  }
}