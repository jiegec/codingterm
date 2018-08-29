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

#ifndef CHIP_H
#define CHIP_H

#include <QWidget>

class Worker;

#define INPUT_NUM 2
#define OUTPUT_NUM 3

class Chip : public QWidget {
  Q_OBJECT
public:
  Chip(QWidget *parent = nullptr, int side = 5);
  virtual ~Chip();

public slots:
  void onSideChanged(int value);
  void onResultChanged(QVector<double> result);
  void onTargetOutputFlow1Changed(int value);
  void onTargetOutputFlow2Changed(int value);
  void onTargetOutputFlow3Changed(int value);
  void onVisualizeConcentrationChanged(bool value);
  void beginFindTarget();
  void updateDisabledMatrix(bool new_disabled_v[9][9],
                            bool new_disabled_h[9][9],
                            bool isFinished);

signals:
  void statusChanged(QString);
  void dataChanged();
  void messageChanged(QString);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

private:
  friend class Worker;
  int convertPos(int x, int y, int &res_i, int &res_j);
  bool isMouseDown;
  bool movedOnMouseDown;
  QPointF mouseDownPos;
  bool isHoldingInput;
  int holdingInputIndex;
  bool isHoldingOutput;
  int holdingOutputIndex;
  bool isResizing;
  int resizingType;
  int resizingX;
  int resizingY;
  QThread *workerThread;
  Worker *worker;

  int side;
  int inputCol[INPUT_NUM];
  int inputWidth[INPUT_NUM];
  int outputCol[OUTPUT_NUM];
  int outputWidth[OUTPUT_NUM];
  QVector<double> result;
  QVector<double> direction;
  QVector<double> concentration;
  bool visualizeConcentration;
  int width_v[9][9];
  bool disabled_v[9][9];
  int width_h[9][9];
  bool disabled_h[9][9];

  double target_output_flow[3];
  QThread *findTargetThread;
  friend void findTargetWorker();
};

#endif