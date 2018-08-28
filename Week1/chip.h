#ifndef CHIP_H
#define CHIP_H

#include <QWidget>

#define INPUT_NUM 2
#define OUTPUT_NUM 3

class Chip : public QWidget {
  Q_OBJECT
public:
  Chip(QWidget *parent = nullptr, int side = 5);

public slots:
  void onSideChanged(int);

signals:
  void statusChanged(QString);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

private:
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

  int side;
  int inputCol[INPUT_NUM];
  int inputWidth[INPUT_NUM];
  int outputCol[OUTPUT_NUM];
  int outputWidth[OUTPUT_NUM];
  int width_v[9][9];
  bool disabled_v[9][9];
  int width_h[9][9];
  bool disabled_h[9][9];
};

#endif