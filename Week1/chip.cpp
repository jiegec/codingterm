// Copyright (C) 2018 Jiajie Chen
// 
// This file is part of Week1.
// 
// Week1 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Week1 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Week1.  If not, see <http://www.gnu.org/licenses/>.
// 

#include "chip.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <cmath>

#define MIN_WIDTH 200
#define BUFFER_HEIGHT 500
#define LENGTH 1600
#define SCALE 0.036
#define OFFSET (LENGTH + MIN_WIDTH)

Chip::Chip(QWidget *parent, int side) : QWidget(parent), side(side) {
  for (int i = 0; i < INPUT_NUM; i++) {
    inputCol[i] = i;
    inputWidth[i] = MIN_WIDTH * (i + 1);
  }
  for (int i = 0; i < OUTPUT_NUM; i++) {
    outputCol[i] = i;
    outputWidth[i] = MIN_WIDTH * (i + 1);
  }
  for (int i = 0; i <= 8; i++) {
    for (int j = 0; j <= 8; j++) {
      width_v[i][j] = MIN_WIDTH * (rand() % 4 + 1);
      disabled_v[i][j] = false;
      width_h[i][j] = MIN_WIDTH * (rand() % 4 + 1);
      disabled_h[i][j] = false;
    }
  }
  isResizing = false;
  isMouseDown = false;
  setMouseTracking(true);
}

void draw_vertical(QPainter &painter, int width) {
  {
    // main body
    const QPointF points[4] = {
        QPointF(-width / 2, BUFFER_HEIGHT + MIN_WIDTH / 2),
        QPointF(-width / 2, LENGTH - BUFFER_HEIGHT + MIN_WIDTH / 2),
        QPointF(width / 2, LENGTH - BUFFER_HEIGHT + MIN_WIDTH / 2),
        QPointF(width / 2, BUFFER_HEIGHT + MIN_WIDTH / 2)};
    painter.drawPolygon(points, 4);
  }
  {
    // top block
    const QPointF points[4] = {
        QPointF(-MIN_WIDTH / 2, MIN_WIDTH / 2),
        QPointF(-MIN_WIDTH / 2, BUFFER_HEIGHT + MIN_WIDTH / 2),
        QPointF(MIN_WIDTH / 2, BUFFER_HEIGHT + MIN_WIDTH / 2),
        QPointF(MIN_WIDTH / 2, MIN_WIDTH / 2)};
    painter.drawPolygon(points, 4);
  }
  {
    // bottom block
    const QPointF points[4] = {
        QPointF(-MIN_WIDTH / 2, LENGTH + MIN_WIDTH / 2),
        QPointF(-MIN_WIDTH / 2, LENGTH - BUFFER_HEIGHT + MIN_WIDTH / 2),
        QPointF(MIN_WIDTH / 2, LENGTH - BUFFER_HEIGHT + MIN_WIDTH / 2),
        QPointF(MIN_WIDTH / 2, LENGTH + MIN_WIDTH / 2)};
    painter.drawPolygon(points, 4);
  }
  // top arc
  painter.drawPie(QRectF(MIN_WIDTH - width / 2, MIN_WIDTH / 2,
                         width - MIN_WIDTH, BUFFER_HEIGHT * 2),
                  0 * 16, 90 * 16);
  painter.drawPie(
      QRectF(-width / 2, MIN_WIDTH / 2, width - MIN_WIDTH, BUFFER_HEIGHT * 2),
      90 * 16, 90 * 16);
  // bottom arc
  painter.drawPie(QRectF(-width / 2, LENGTH + MIN_WIDTH / 2 - BUFFER_HEIGHT * 2,
                         width - MIN_WIDTH, BUFFER_HEIGHT * 2),
                  180 * 16, 90 * 16);
  painter.drawPie(QRectF(MIN_WIDTH - width / 2,
                         LENGTH + MIN_WIDTH / 2 - BUFFER_HEIGHT * 2,
                         width - MIN_WIDTH, BUFFER_HEIGHT * 2),
                  270 * 16, 90 * 16);
}

void draw_horizontal(QPainter &painter, int width) {

  {
    // main body
    const QPointF points[4] = {
        QPointF(BUFFER_HEIGHT + MIN_WIDTH / 2, -width / 2),
        QPointF(LENGTH - BUFFER_HEIGHT + MIN_WIDTH / 2, -width / 2),
        QPointF(LENGTH - BUFFER_HEIGHT + MIN_WIDTH / 2, width / 2),
        QPointF(BUFFER_HEIGHT + MIN_WIDTH / 2, width / 2)};
    painter.drawPolygon(points, 4);
  }
  {
    // top block
    const QPointF points[4] = {
        QPointF(MIN_WIDTH / 2, -MIN_WIDTH / 2),
        QPointF(BUFFER_HEIGHT + MIN_WIDTH / 2, -MIN_WIDTH / 2),
        QPointF(BUFFER_HEIGHT + MIN_WIDTH / 2, MIN_WIDTH / 2),
        QPointF(MIN_WIDTH / 2, MIN_WIDTH / 2)};
    painter.drawPolygon(points, 4);
  }
  {
    // bottom block
    const QPointF points[4] = {
        QPointF(LENGTH + MIN_WIDTH / 2, -MIN_WIDTH / 2),
        QPointF(LENGTH - BUFFER_HEIGHT + MIN_WIDTH / 2, -MIN_WIDTH / 2),
        QPointF(LENGTH - BUFFER_HEIGHT + MIN_WIDTH / 2, MIN_WIDTH / 2),
        QPointF(LENGTH + MIN_WIDTH / 2, MIN_WIDTH / 2)};
    painter.drawPolygon(points, 4);
  }
  // top arc
  painter.drawPie(QRectF(MIN_WIDTH / 2, MIN_WIDTH - width / 2,
                         BUFFER_HEIGHT * 2, width - MIN_WIDTH),
                  180 * 16, 90 * 16);
  painter.drawPie(
      QRectF(MIN_WIDTH / 2, -width / 2, BUFFER_HEIGHT * 2, width - MIN_WIDTH),
      90 * 16, 90 * 16);
  // bottom arc
  painter.drawPie(QRectF(LENGTH + MIN_WIDTH / 2 - BUFFER_HEIGHT * 2, -width / 2,
                         BUFFER_HEIGHT * 2, width - MIN_WIDTH),
                  0 * 16, 90 * 16);
  painter.drawPie(QRectF(LENGTH + MIN_WIDTH / 2 - BUFFER_HEIGHT * 2,
                         MIN_WIDTH - width / 2, BUFFER_HEIGHT * 2,
                         width - MIN_WIDTH),
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
      painter.translate(i * (LENGTH + MIN_WIDTH), j * (LENGTH + MIN_WIDTH));
      const QPointF points[4] = {QPointF(-MIN_WIDTH / 2, -MIN_WIDTH / 2),
                                 QPointF(-MIN_WIDTH / 2, MIN_WIDTH / 2),
                                 QPointF(MIN_WIDTH / 2, MIN_WIDTH / 2),
                                 QPointF(MIN_WIDTH / 2, -MIN_WIDTH / 2)};
      painter.drawPolygon(points, 4);
      painter.restore();
    }
  }

  painter.setBrush(QColor("#9C27B0"));
  for (int i = 0; i <= side; i++) {
    for (int j = 0; j < side; j++) {
      painter.save();
      painter.translate(i * (LENGTH + MIN_WIDTH), j * (LENGTH + MIN_WIDTH));
      if (disabled_v[i][j]) {
        painter.setBrush(QColor("#9E9E9E"));
      }
      draw_vertical(painter, width_v[i][j]);
      painter.restore();
    }
  }

  for (int i = 0; i < INPUT_NUM; i++) {
    painter.save();
    painter.translate(inputCol[i] * (LENGTH + MIN_WIDTH),
                      -1 * (LENGTH + MIN_WIDTH));
    draw_vertical(painter, inputWidth[i]);
    painter.restore();
  }

  painter.setBrush(QColor("#03A9F4"));
  for (int i = 0; i < side; i++) {
    for (int j = 0; j <= side; j++) {
      painter.save();
      painter.translate(i * (LENGTH + MIN_WIDTH), j * (LENGTH + MIN_WIDTH));
      if (disabled_h[i][j]) {
        painter.setBrush(QColor("#9E9E9E"));
      }
      draw_horizontal(painter, width_h[i][j]);
      painter.restore();
    }
  }

  for (int i = 0; i < OUTPUT_NUM; i++) {
    painter.save();
    painter.translate(outputCol[i] * (LENGTH + MIN_WIDTH),
                      side * (LENGTH + MIN_WIDTH));
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
#define TYPE_V_MID 2
#define TYPE_H 3
#define TYPE_H_MID 4
#define TYPE_INPUT 5
#define TYPE_INPUT_MID 6
#define TYPE_OUTPUT 7
#define TYPE_OUTPUT_MID 8

int Chip::convertPos(int x, int y, int &res_i, int &res_j) {
  const int THRESHOLD = 50;
  // vertical
  for (int i = 0; i <= side; i++) {
    for (int j = 0; j < side; j++) {
      int xx = OFFSET + i * (LENGTH + MIN_WIDTH);
      int yy = OFFSET + j * (LENGTH + MIN_WIDTH);
      if ((xx - width_v[i][j] / 2) * SCALE <= x &&
          x <= (xx + width_v[i][j] / 2) * SCALE &&
          (yy + MIN_WIDTH / 2) * SCALE <= y &&
          y <= (yy + LENGTH - MIN_WIDTH / 2) * SCALE) {
        res_i = i;
        res_j = j;
        if (fabs((xx - width_v[i][j] / 2) - x / SCALE) <= THRESHOLD ||
            fabs((xx + width_v[i][j] / 2) - x / SCALE) <= THRESHOLD) {
          return TYPE_V;
        }
        return TYPE_V_MID;
      }
    }
  }

  // horizontal
  for (int i = 0; i < side; i++) {
    for (int j = 0; j <= side; j++) {
      int xx = OFFSET + i * (LENGTH + MIN_WIDTH);
      int yy = OFFSET + j * (LENGTH + MIN_WIDTH);
      if ((xx + MIN_WIDTH / 2) * SCALE <= x &&
          x <= (xx + LENGTH - MIN_WIDTH / 2) * SCALE &&
          (yy - width_h[i][j] / 2) * SCALE <= y &&
          y <= (yy + width_h[i][j] / 2) * SCALE) {
        res_i = i;
        res_j = j;
        if (fabs((yy - width_h[i][j] / 2) - y / SCALE) <= THRESHOLD ||
            fabs((yy + width_h[i][j] / 2) - y / SCALE) <= THRESHOLD) {
          return TYPE_H;
        }
        return TYPE_H_MID;
      }
    }
  }

  // input
  for (int i = 0; i < INPUT_NUM; i++) {
    int xx = OFFSET + inputCol[i] * (LENGTH + MIN_WIDTH);
    int yy = OFFSET + (-1) * (LENGTH + MIN_WIDTH);
    if ((xx - inputWidth[i] / 2) * SCALE <= x &&
        x <= (xx + inputWidth[i] / 2) * SCALE &&
        (yy + MIN_WIDTH / 2) * SCALE <= y &&
        y <= (yy + LENGTH - MIN_WIDTH / 2) * SCALE) {
      res_i = i;
      if (fabs((xx - inputWidth[i] / 2) - x / SCALE) <= THRESHOLD ||
          fabs((xx + inputWidth[i] / 2) - x / SCALE) <= THRESHOLD) {
        return TYPE_INPUT;
      }
      return TYPE_INPUT_MID;
    }
  }

  // output
  for (int i = 0; i < OUTPUT_NUM; i++) {
    int xx = OFFSET + outputCol[i] * (LENGTH + MIN_WIDTH);
    int yy = OFFSET + side * (LENGTH + MIN_WIDTH);
    if ((xx - outputWidth[i] / 2) * SCALE <= x &&
        x <= (xx + outputWidth[i] / 2) * SCALE &&
        (yy + MIN_WIDTH / 2) * SCALE <= y &&
        y <= (yy + LENGTH - MIN_WIDTH / 2) * SCALE) {
      res_i = i;
      if (fabs((xx - outputWidth[i] / 2) - x / SCALE) <= THRESHOLD ||
          fabs((xx + outputWidth[i] / 2) - x / SCALE) <= THRESHOLD) {
        return TYPE_OUTPUT;
      }
      return TYPE_OUTPUT_MID;
    }
  }

  return 0;
}

void Chip::mouseMoveEvent(QMouseEvent *event) {
  const QPointF pos = event->localPos();
  if (isMouseDown) {
    movedOnMouseDown = true;

    // if dragging input/output
    // find nearest available place
    if (isHoldingInput) {
      int min_xoff = 0x7fffffff, min_place = -1;
      for (int i = 0; i <= side; i++) {
        int j;
        for (j = 0; j < INPUT_NUM; j++) {
          if (j == holdingInputIndex) {
            continue;
          }
          if (inputCol[j] == i) {
            // this is occupied.
            break;
          }
          int gap = (LENGTH + MIN_WIDTH) * abs(inputCol[j] - i) - MIN_WIDTH;
          if ((inputWidth[holdingInputIndex] + inputWidth[j]) / 2 >= gap) {
            // no enough space.
            break;
          }
        }
        if (j == INPUT_NUM) {
          // empty slot
          int xx = OFFSET + i * (LENGTH + MIN_WIDTH);
          int xoff = fabs(xx * SCALE - pos.x());
          if (xoff < min_xoff) {
            min_xoff = xoff;
            min_place = i;
          }
        }
      }

      if (min_place != -1) {
        inputCol[holdingInputIndex] = min_place;
        update();
      }
    } else if (isHoldingOutput) {
      int min_xoff = 0x7fffffff, min_place;
      for (int i = 0; i <= side; i++) {
        int j;
        for (j = 0; j < OUTPUT_NUM; j++) {
          if (j == holdingOutputIndex) {
            continue;
          }
          if (outputCol[j] == i) {
            // this is occupied.
            break;
          }
          int gap = (LENGTH + MIN_WIDTH) * abs(outputCol[j] - i) - MIN_WIDTH;
          if ((outputWidth[holdingOutputIndex] + outputWidth[j]) / 2 >= gap) {
            // no enough space.
            break;
          }
        }
        if (j == OUTPUT_NUM) {
          // empty slot
          int xx = OFFSET + i * (LENGTH + MIN_WIDTH);
          int xoff = fabs(xx * SCALE - pos.x());
          if (xoff < min_xoff) {
            min_xoff = xoff;
            min_place = i;
          }
        }
      }

      if (min_place != -1) {
        outputCol[holdingOutputIndex] = min_place;
        update();
      }
    } else if (isResizing) {
      // resize to cursor
      int xx = OFFSET + resizingX * (LENGTH + MIN_WIDTH);
      int yy = OFFSET + resizingY * (LENGTH + MIN_WIDTH);
      float off = 0;
      switch (resizingType) {
      case TYPE_V:
        off = abs(pos.x() / SCALE - xx);
        off = fmax(off, MIN_WIDTH / 2);
        if (resizingX > 0) {
          off = fmin(off, LENGTH - width_v[resizingX - 1][resizingY] / 2);
        }
        if (resizingX < side) {
          off = fmin(off, LENGTH - width_v[resizingX + 1][resizingY] / 2);
        }
        width_v[resizingX][resizingY] = off * 2;
        break;
      case TYPE_H:
        off = fabs(pos.y() / SCALE - yy);
        off = fmax(off, MIN_WIDTH / 2);
        if (resizingY > 0) {
          off = fmin(off, LENGTH - width_h[resizingX][resizingY - 1] / 2);
        }
        if (resizingY < side) {
          off = fmin(off, LENGTH - width_h[resizingX][resizingY + 1] / 2);
        }
        width_h[resizingX][resizingY] = off * 2;
        break;
      case TYPE_INPUT:
        xx = OFFSET + inputCol[resizingX] * (LENGTH + MIN_WIDTH);
        off = abs(pos.x() / SCALE - xx);
        off = fmax(off, MIN_WIDTH / 2);
        for (int i = 0; i < INPUT_NUM; i++) {
          if (inputCol[resizingX] != inputCol[i]) {
            int gap =
                (LENGTH + MIN_WIDTH) * abs(inputCol[resizingX] - inputCol[i]) -
                MIN_WIDTH;
            off = fmin(off, gap - inputWidth[i] / 2);
          }
        }
        inputWidth[resizingX] = off * 2;
        break;
      case TYPE_OUTPUT:
        xx = OFFSET + outputCol[resizingX] * (LENGTH + MIN_WIDTH);
        off = abs(pos.x() / SCALE - xx);
        off = fmax(off, MIN_WIDTH / 2);
        for (int i = 0; i < OUTPUT_NUM; i++) {
          if (outputCol[resizingX] != outputCol[i]) {
            int gap = (LENGTH + MIN_WIDTH) *
                          abs(outputCol[resizingX] - outputCol[i]) -
                      MIN_WIDTH;
            off = fmin(off, gap - outputWidth[i] / 2);
          }
        }
        outputWidth[resizingX] = off * 2;
        break;
      default:
        break;
      }
      emit statusChanged(tr("Resizing to width %1").arg((int)(off * 2)));
      update();
    }
  } else {
    int i = 0, j = 0;
    int result = convertPos(pos.x(), pos.y(), i, j);

    int width = 0;
    if (result == TYPE_H) {
      width = width_h[i][j];
    } else if (result == TYPE_V) {
      width = width_v[i][j];
    } else if (result == TYPE_INPUT) {
      width = inputWidth[i];
    } else if (result == TYPE_OUTPUT) {
      width = outputWidth[i];
    }

    switch (result) {
    case TYPE_V_MID:
    case TYPE_H_MID:
      setCursor(Qt::PointingHandCursor);
      emit statusChanged(tr("Click to toggle"));
      break;
    case TYPE_V:
    case TYPE_INPUT:
    case TYPE_OUTPUT:
      setCursor(Qt::SizeHorCursor);
      emit statusChanged(tr("Drag to resize, current width is %1").arg(width));
      break;
    case TYPE_H:
      setCursor(Qt::SizeVerCursor);
      emit statusChanged(tr("Drag to resize, current width is %1").arg(width));
      break;
    case TYPE_INPUT_MID:
    case TYPE_OUTPUT_MID:
      setCursor(Qt::OpenHandCursor);
      emit statusChanged(tr("Drag to move"));
      break;
    default:
      setCursor(Qt::ArrowCursor);
      emit statusChanged(tr(""));
      break;
    }
  }
}
void Chip::mousePressEvent(QMouseEvent *event) {
  isMouseDown = true;
  movedOnMouseDown = false;
  mouseDownPos = event->localPos();

  const QPointF pos = event->localPos();
  int i = 0, j = 0;
  int result = convertPos(pos.x(), pos.y(), i, j);
  if (result) {
    isHoldingInput = (result == TYPE_INPUT_MID);
    holdingInputIndex = i;
    isHoldingOutput = (result == TYPE_OUTPUT_MID);
    holdingOutputIndex = i;
    if (isHoldingInput || isHoldingOutput) {
      setCursor(Qt::ClosedHandCursor);
    }
    resizingType = result;
    if (result == TYPE_V || result == TYPE_H) {
      isResizing = true;
    } else if (result == TYPE_INPUT || result == TYPE_OUTPUT) {
      isResizing = true;
      if (result == TYPE_INPUT) {
        j = -1;
      } else if (result == TYPE_OUTPUT) {
        j = side;
      }
    } else {
      isResizing = false;
    }
    resizingX = i;
    resizingY = j;
  }
}
void Chip::mouseReleaseEvent(QMouseEvent *event) {
  isMouseDown = false;

  const QPointF pos = event->localPos();
  int i = 0, j = 0;
  int result = convertPos(pos.x(), pos.y(), i, j);
  if (!movedOnMouseDown) {
    switch (result) {
    case TYPE_V:
    case TYPE_V_MID:
      disabled_v[i][j] = !disabled_v[i][j];
      update();
      break;
    case TYPE_H:
    case TYPE_H_MID:
      disabled_h[i][j] = !disabled_h[i][j];
      update();
      break;
    default:
      break;
    }
  }
}