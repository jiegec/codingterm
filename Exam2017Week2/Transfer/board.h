#ifndef BOARD_H
#define BOARD_H

#include <QWidget>
#include <QUdpSocket>

class Board : public QWidget {
  Q_OBJECT
public:
  Board(QWidget *parent = nullptr, bool isClient = true);

public slots:
  void readUdp();

signals:
protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

private:
  QVector<QVector<QPointF>> points;
  QUdpSocket *socket;
  bool isDragging;
  bool isClient;
};

#endif