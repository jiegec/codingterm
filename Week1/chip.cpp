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
#include "algo.h"
#include "worker.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QThread>
#include <algorithm>
#include <cmath>

#define MIN_WIDTH 200
#define BUFFER_HEIGHT 500
#define LENGTH 1600
#define SCALE 0.036
#define OFFSET (LENGTH + MIN_WIDTH)

Chip::Chip(QWidget *parent, int side) : QWidget(parent), side(side) {
  for (int i = 0; i < INPUT_NUM; i++) {
    inputCol[i] = i;
    inputWidth[i] = MIN_WIDTH;
  }
  for (int i = 0; i < OUTPUT_NUM; i++) {
    outputCol[i] = i;
    outputWidth[i] = MIN_WIDTH;
    target_output_flow[i] = 0;
  }

  for (int i = 0; i <= 8; i++) {
    for (int j = 0; j <= 8; j++) {
      width_v[i][j] = MIN_WIDTH;
      disabled_v[i][j] = false;
      width_h[i][j] = MIN_WIDTH;
      disabled_h[i][j] = false;
    }
  }

  findTargetThread = nullptr;
  isResizing = false;
  isMouseDown = false;
  visualizeConcentration = false;
  setMouseTracking(true);

  worker = new Worker(this);
  workerThread = new QThread(this);
  worker->moveToThread(workerThread);
  connect(this, SIGNAL(dataChanged()), worker, SLOT(calculate()));
  qRegisterMetaType<QVector<double>>("QVector<double>");
  connect(worker, SIGNAL(finished(QVector<double>)), this,
          SLOT(onResultChanged(QVector<double>)));
  workerThread->start();

  emit dataChanged();
}

Chip::~Chip() {
  if (findTargetThread) {
    findTargetThread->wait();
    delete findTargetThread;
  }
  if (workerThread) {
    workerThread->exit();
    workerThread->wait();
  }
}

void draw_vertical(QPainter &painter, int width, double flow, int direction) {
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

  // direction arrow
  QColor colorDirection("#F44336");
  if (flow > 0) {
    painter.setBrush(colorDirection);
    if (direction == 0) {
      const QPointF points[4] = {
          QPointF(-MIN_WIDTH / 2, LENGTH + MIN_WIDTH / 2 - MIN_WIDTH),
          QPointF(0, LENGTH + MIN_WIDTH / 2),
          QPointF(MIN_WIDTH / 2, LENGTH + MIN_WIDTH / 2 - MIN_WIDTH)};
      painter.drawPolygon(points, 3);
    } else {
      const QPointF points[4] = {
          QPointF(-MIN_WIDTH / 2, MIN_WIDTH / 2 + MIN_WIDTH),
          QPointF(0, MIN_WIDTH / 2),
          QPointF(MIN_WIDTH / 2, MIN_WIDTH / 2 + MIN_WIDTH)};
      painter.drawPolygon(points, 3);
    }
  }
}

QColor gradient(const QColor from, const QColor to, double ratio) {
  return QColor(from.red() + (to.red() - from.red()) * ratio,
                from.green() + (to.green() - from.green() * ratio),
                from.blue() + (to.blue() - from.blue()) * ratio);
}

void Chip::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.scale(SCALE, SCALE);
  QPen origPen = painter.pen();
  painter.setPen(Qt::NoPen);
  painter.translate(OFFSET, OFFSET);

  QColor colorFull("#2962FF");
  QColor colorNone("#039BE5");
  QColor colorDisabled("#9E9E9E");

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

  int resultIndex = 0;
  // when side changes, the result array is no longer valid
  // but it will be recalculated anyway
  int resultLen = result.length();
  for (int i = 0; i <= side; i++) {
    for (int j = 0; j < side; j++) {
      QColor color = gradient(colorNone, colorFull,
                              visualizeConcentration
                                  ? concentration[resultIndex % resultLen]
                                  : result[resultIndex % resultLen] / 400.0);
      if (disabled_v[i][j]) {
        color = colorDisabled;
      }
      painter.setBrush(color);

      painter.save();
      painter.translate(i * (LENGTH + MIN_WIDTH), j * (LENGTH + MIN_WIDTH));
      draw_vertical(painter, width_v[i][j], result[resultIndex % resultLen],
                    direction[resultIndex % resultLen]);
      resultIndex++;
      painter.restore();
    }
  }

  // painter.setBrush(QColor("#03A9F4"));
  for (int i = 0; i < side; i++) {
    for (int j = 0; j <= side; j++) {
      QColor color = gradient(colorNone, colorFull,
                              visualizeConcentration
                                  ? concentration[resultIndex % resultLen]
                                  : result[resultIndex % resultLen] / 400.0);
      if (disabled_h[i][j]) {
        color = colorDisabled;
      }
      painter.setBrush(color);

      painter.save();
      painter.translate(i * (LENGTH + MIN_WIDTH), j * (LENGTH + MIN_WIDTH));
      // reuse vertical code for horizontal
      painter.rotate(270);

      draw_vertical(painter, width_h[i][j], result[resultIndex % resultLen],
                    direction[resultIndex % resultLen]);
      resultIndex++;
      painter.restore();
    }
  }

  for (int i = 0; i < INPUT_NUM; i++) {
    QColor color = gradient(colorNone, colorFull,
                            visualizeConcentration
                                ? concentration[resultIndex % resultLen]
                                : result[resultIndex % resultLen] / 400.0);

    painter.setBrush(color);

    painter.save();
    painter.translate(inputCol[i] * (LENGTH + MIN_WIDTH),
                      -1 * (LENGTH + MIN_WIDTH));
    draw_vertical(painter, inputWidth[i], result[resultIndex % resultLen], 0);
    painter.restore();

    resultIndex++;
  }

  QFont font = painter.font();
  font.setPixelSize(300);
  painter.setFont(font);
  for (int i = 0; i < OUTPUT_NUM; i++) {
    QColor color = gradient(colorNone, colorFull,
                            visualizeConcentration
                                ? concentration[resultIndex % resultLen]
                                : result[resultIndex % resultLen] / 400.0);
    painter.setBrush(color);

    painter.save();
    painter.translate(outputCol[i] * (LENGTH + MIN_WIDTH),
                      side * (LENGTH + MIN_WIDTH));
    draw_vertical(painter, outputWidth[i], result[resultIndex % resultLen], 0);
    painter.setPen(origPen);
    painter.drawText(-LENGTH / 2, LENGTH + MIN_WIDTH, LENGTH, LENGTH,
                     Qt::AlignHCenter | Qt::AlignTop,
                     QString("%1").arg(result[resultIndex % resultLen]));
    painter.setPen(Qt::NoPen);
    painter.restore();

    resultIndex++;
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
  emit dataChanged();
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
  const int THRESHOLD = 30;
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
        emit dataChanged();
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
        emit dataChanged();
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
      emit statusChanged(tr("Resizing to width %1.").arg((int)(off * 2)));
      update();
      emit dataChanged();
    }
  } else {
    int i = 0, j = 0;
    int result = convertPos(pos.x(), pos.y(), i, j);

    int resultIndex = 0;
    int resultLen = this->result.length();
    switch (result) {
    case TYPE_V:
    case TYPE_V_MID:
      if (disabled_v[i][j]) {
        emit messageChanged(tr("Edge under cursor is disabled."));
      } else {
        resultIndex = i * side + j;
        emit messageChanged(
            tr("Flow under cursor: %1. Concentration under cursor: %2.")
                .arg(this->result[resultIndex])
                .arg(this->concentration[resultIndex]));
      }
      break;
    case TYPE_H:
    case TYPE_H_MID:
      if (disabled_h[i][j]) {
        emit messageChanged(tr("Edge under cursor is disabled."));
      } else {
        resultIndex = side * (side + 1) + i * (side + 1) + j;
        emit messageChanged(
            tr("Flow under cursor: %1. Concentration under cursor: %2.")
                .arg(this->result[resultIndex])
                .arg(this->concentration[resultIndex]));
      }
      break;
    case TYPE_INPUT:
    case TYPE_INPUT_MID:
      resultIndex = resultLen - 5 + i;
      emit messageChanged(
          tr("Flow under cursor: %1. Concentration under cursor: %2.")
              .arg(this->result[resultIndex])
              .arg(this->concentration[resultIndex]));
      break;
    case TYPE_OUTPUT:
    case TYPE_OUTPUT_MID:
      resultIndex = resultLen - 3 + i;
      emit messageChanged(
          tr("Flow under cursor: %1. Concentration under cursor: %2.")
              .arg(this->result[resultIndex])
              .arg(this->concentration[resultIndex]));
      break;
    default:
      if (this->result[resultLen - 3] == 0 && this->result[resultLen - 2] == 0 &&
          this->result[resultLen - 1] == 0) {
        emit messageChanged(
            tr("Please ensure there is a path from input to output."));
      } else {
        emit messageChanged(tr(
            "On the left side of the interface, you "
            "can change the side of the chip, change the language and specify "
            "the "
            "target output flow for each output port and then ask it to search "
            "for a solution. On the right side, you can see the chip, where "
            "color "
            "reflects the current flow of edge. You can toggle the existence "
            "of "
            "edges by clicking them, and change their width by dragging their "
            "two "
            "sides."));
      }
      emit statusChanged(tr("Hover over an edge to see its flow."));
      break;
    }

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
      emit statusChanged(tr("Click to toggle."));
      break;
    case TYPE_V:
    case TYPE_INPUT:
    case TYPE_OUTPUT:
      setCursor(Qt::SizeHorCursor);
      emit statusChanged(tr("Drag to resize, current width is %1.").arg(width));
      break;
    case TYPE_H:
      setCursor(Qt::SizeVerCursor);
      emit statusChanged(tr("Drag to resize, current width is %1.").arg(width));
      break;
    case TYPE_INPUT_MID:
    case TYPE_OUTPUT_MID:
      setCursor(Qt::OpenHandCursor);
      emit statusChanged(tr("Drag to move."));
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
    case TYPE_V_MID:
      disabled_v[i][j] = !disabled_v[i][j];
      update();
      emit dataChanged();
      break;
    case TYPE_H_MID:
      disabled_h[i][j] = !disabled_h[i][j];
      update();
      emit dataChanged();
      break;
    default:
      break;
    }
  }
}

void Chip::onResultChanged(QVector<double> result) {
  int len = result.length() / 3;
  this->result = result;
  this->result.resize(len);

  this->direction = result;
  this->direction.remove(0, len);
  this->direction.resize(len);

  this->concentration = result;
  this->concentration.remove(0, len * 2);

  update();
  if (result[len - 3] == 0 && result[len - 2] == 0 && result[len - 1] == 0) {
    emit messageChanged(
        tr("Please ensure there is a path from input to output."));
  } else {
    emit messageChanged(tr("Calculation completed."));
  }
}

void Chip::onTargetOutputFlow1Changed(int value) {
  target_output_flow[0] = value;
}
void Chip::onTargetOutputFlow2Changed(int value) {
  target_output_flow[1] = value;
}
void Chip::onTargetOutputFlow3Changed(int value) {
  target_output_flow[2] = value;
}

Chip *chip;
struct {
  int side;
  int width_v[9][9];
  bool disabled_v[9][9];
  int width_h[9][9];
  bool disabled_h[9][9];
  int inputCol[INPUT_NUM];
  int inputWidth[INPUT_NUM];
  int outputCol[OUTPUT_NUM];
  int outputWidth[OUTPUT_NUM];
  int target_output_flow[OUTPUT_NUM];
  QVector<double> result;
} workerData;

struct State {
  bool disabled_v[9][9];
  bool disabled_h[9][9];
  double loss;
};

bool operator<(const struct State &a, const struct State &b) {
  return a.loss < b.loss;
}

void findTargetWorker() {
  const int eliteLen = 10;
  const int bufferLen = 128;
  struct State elite[eliteLen];
  struct State buffer[bufferLen];
  for (int i = 0; i <= workerData.side; i++) {
    for (int j = 0; j <= workerData.side; j++) {
      elite[0].disabled_v[i][j] = workerData.disabled_v[i][j];
      elite[0].disabled_h[i][j] = workerData.disabled_h[i][j];
    }
  }
  elite[0].loss = 0.0;
  int resultLen = workerData.result.length();
  for (int i = 0; i < OUTPUT_NUM; i++) {
    elite[0].loss += (workerData.target_output_flow[i] -
                      workerData.result[resultLen - 3 + i]) *
                     (workerData.target_output_flow[i] -
                      workerData.result[resultLen - 3 + i]);
  }

  struct State bestState = elite[0];
  int eliteCount = 1;
  int minSolutionCount = 1;
  const int maxRound = 100;
  for (int round = 0; round < maxRound; round++) {
    int buffer_num = 0;
    for (int i = 0; i < eliteCount; i++) {
      for (int j = 0; j < eliteCount; j++) {
        struct State &newstate = buffer[buffer_num++];
        for (int x = 0; x <= workerData.side; x++) {
          for (int y = 0; y <= workerData.side; y++) {
            newstate.disabled_v[x][y] = (rand() % 2)
                                            ? elite[i].disabled_v[x][y]
                                            : elite[j].disabled_v[x][y];
            if (rand() % 10 == 0) {
              newstate.disabled_v[x][y] = !newstate.disabled_v[x][y];
            }

            newstate.disabled_h[x][y] = (rand() % 2)
                                            ? elite[i].disabled_h[x][y]
                                            : elite[j].disabled_h[x][y];
            if (rand() % 10 == 0) {
              newstate.disabled_h[x][y] = !newstate.disabled_h[x][y];
            }
          }
        }

        std::vector<double> result;
        std::vector<double> length;
        for (int i = 0; i <= workerData.side; i++) {
          for (int j = 0; j < workerData.side; j++) {
            if (newstate.disabled_v[i][j]) {
              length.push_back(0);
            } else {
              length.push_back(workerData.width_v[i][j]);
            }
          }
        }
        for (int i = 0; i < workerData.side; i++) {
          for (int j = 0; j <= workerData.side; j++) {
            if (newstate.disabled_h[i][j]) {
              length.push_back(0);
            } else {
              length.push_back(workerData.width_h[i][j]);
            }
          }
        }
        for (int i = 0; i < INPUT_NUM; i++) {
          length.push_back(workerData.inputWidth[i]);
        }
        for (int i = 0; i < OUTPUT_NUM; i++) {
          length.push_back(workerData.outputWidth[i]);
        }
        result =
            caluconspeed(workerData.side + 1, length, workerData.inputCol[0],
                         workerData.inputCol[1], workerData.outputCol[0],
                         workerData.outputCol[1], workerData.outputCol[2]);
        int resultLen = result.size() / 3;
        newstate.loss = 0;
        for (int i = 0; i < OUTPUT_NUM; i++) {
          newstate.loss +=
              (result[resultLen - 3 + i] - workerData.target_output_flow[i]) *
              (result[resultLen - 3 + i] - workerData.target_output_flow[i]);
        }

        if (newstate.loss < bestState.loss) {
          qWarning() << newstate.loss;
          bestState = newstate;
          emit chip->statusChanged(
              Chip::tr("Found a solution #%1 of loss %2.(%3/%4)")
                  .arg(minSolutionCount++)
                  .arg(newstate.loss)
                  .arg(round + 1)
                  .arg(maxRound));
          emit chip->updateDisabledMatrix(newstate.disabled_v,
                                          newstate.disabled_h, false);
        }
      }
    }

    for (int i = 0; i < eliteCount; i++) {
      buffer[buffer_num++] = elite[i];
    }
    eliteCount = eliteLen;

    std::sort(buffer, buffer + buffer_num);
    for (int i = 0; i < eliteCount; i++) {
      elite[i] = buffer[i];
    }

    if (round % 10 == 0)
      emit chip->statusChanged(
          Chip::tr("Working on it.(%1/%2)").arg(round).arg(maxRound));
  }

  emit chip->statusChanged(Chip::tr("Done"));
  emit chip->updateDisabledMatrix(bestState.disabled_v, bestState.disabled_h,
                                  true);
}

void Chip::beginFindTarget() {
  if (findTargetThread != nullptr) {
    emit statusChanged(tr("Don't push me now. I am working on it."));
    return;
  }

  chip = this;

  workerData.side = side;
  for (int i = 0; i <= side; i++) {
    for (int j = 0; j <= side; j++) {
      workerData.width_v[i][j] = width_v[i][j];
      workerData.disabled_v[i][j] = disabled_v[i][j];
      workerData.width_h[i][j] = width_h[i][j];
      workerData.disabled_h[i][j] = disabled_h[i][j];
    }
  }
  for (int i = 0; i < INPUT_NUM; i++) {
    workerData.inputCol[i] = inputCol[i];
    workerData.inputWidth[i] = inputWidth[i];
  }
  for (int i = 0; i < OUTPUT_NUM; i++) {
    workerData.outputCol[i] = outputCol[i];
    workerData.outputWidth[i] = outputWidth[i];
    workerData.target_output_flow[i] = target_output_flow[i];
  }
  workerData.result = result;

  findTargetThread = QThread::create(findTargetWorker);
  findTargetThread->start();
}

void Chip::updateDisabledMatrix(bool new_disabled_v[9][9],
                                bool new_disabled_h[9][9], bool isFinished) {

  for (int i = 0; i <= side; i++) {
    for (int j = 0; j <= side; j++) {
      disabled_v[i][j] = new_disabled_v[i][j];
      disabled_h[i][j] = new_disabled_h[i][j];
    }
  }

  if (isFinished) {
    findTargetThread = nullptr;
  }

  update();
  emit dataChanged();
}

void Chip::onVisualizeConcentrationChanged(bool value) {
  visualizeConcentration = value;
  update();
}