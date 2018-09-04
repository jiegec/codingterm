#ifndef BOARD_H
#define BOARD_H

#include <QWidget>

#define SIDE_RED 0b0
#define SIDE_BLACK 0b1
#define SIDE_MASK 0b1

#define TYPE_ADVISOR 0b0010
#define TYPE_CANNON 0b0100
#define TYPE_CHARIOT 0b0110
#define TYPE_ELEPHANT 0b1000
#define TYPE_GENERAL 0b1010
#define TYPE_HORSE 0b1100
#define TYPE_SOLDIER 0b1110
#define TYPE_MASK 0b1110


class Board : public QWidget {
  Q_OBJECT
public:
  Board(QWidget *parent = nullptr);

  void setRenderSide(int side);
  void setPlayerSide(int side);
  void setInitialBoard();
  void setCurrentTurn(int side);
  QByteArray dumpBoard();
  void loadBoard(QByteArray);
  void setSinglePlayer(bool);

public slots:
  void doMove(int fromX, int fromY, int toX, int toY);

signals:
  void onUserMove(int fromX, int fromY, int toX, int toY);
  void onCurrentTurnChanged(int side);
  void onCheck(int side);
  void onCheckmate(int side);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

private:
  QString getImageFileName(int);
  bool isDangerForSide(int board[9][10], int side);
  bool isCheckmateForSide(int board[9][10], int side);
  bool isMoveValid(int board[9][10], int fromX, int fromY, int toX, int toY, int side);
  bool getIndexByPos(QPointF position, int &x, int &y);
  void checkStatus();
  int board[9][10];
  int renderSide;
  int currentTurn;
  int playerSide;

  bool isDragging;
  int draggingX;
  int draggingY;
  int hoverX;
  int hoverY;
  int hoverValid;
  bool singlePlayer;
};

#endif