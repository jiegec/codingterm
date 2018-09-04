#include "board.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QSvgRenderer>
#include <cmath>

const int horseStep[8][2] = {{-2, 1}, {-1, 2}, {1, 2},   {2, 1},
                             {2, -1}, {1, -2}, {-1, -2}, {-2, -1}};
const int horseBlock[8][2] = {{-1, 0}, {0, 1},  {0, 1},  {1, 0},
                              {1, 0},  {0, -1}, {0, -1}, {-1, 0}};
const int elephantStep[4][2] = {{-2, 2}, {2, 2}, {2, -2}, {-2, -2}};
const int elephantBlock[4][2] = {{-1, 1}, {1, 1}, {1, -1}, {-1, -1}};

inline int min(int a, int b) { return a > b ? b : a; }
inline int max(int a, int b) { return a < b ? b : a; }

const int SPACE = 40;
const int RADIUS = 15;

bool inline HAS_CHESS(int value) { return (value & TYPE_MASK) != 0; }

Board::Board(QWidget *parent) : QWidget(parent) {
  setMouseTracking(true);

  setRenderSide(SIDE_RED);
  setPlayerSide(SIDE_RED);
  setCurrentTurn(SIDE_RED);
  setInitialBoard();
}

void Board::setRenderSide(int side) {
  renderSide = side;
  update();
}

void Board::setPlayerSide(int side) {
  playerSide = side;
  update();
}

void Board::setCurrentTurn(int side) {
  currentTurn = side;
  update();
  emit onCurrentTurnChanged(side);
}

void Board::setInitialBoard() {
  memset(board, 0, sizeof(board));

  board[0][0] = TYPE_CHARIOT | SIDE_RED;
  board[1][0] = TYPE_HORSE | SIDE_RED;
  board[2][0] = TYPE_ELEPHANT | SIDE_RED;
  board[3][0] = TYPE_ADVISOR | SIDE_RED;
  board[4][0] = TYPE_GENERAL | SIDE_RED;
  board[5][0] = TYPE_ADVISOR | SIDE_RED;
  board[6][0] = TYPE_ELEPHANT | SIDE_RED;
  board[7][0] = TYPE_HORSE | SIDE_RED;
  board[8][0] = TYPE_CHARIOT | SIDE_RED;

  board[1][2] = TYPE_CANNON | SIDE_RED;
  board[7][2] = TYPE_CANNON | SIDE_RED;
  board[0][3] = TYPE_SOLDIER | SIDE_RED;
  board[2][3] = TYPE_SOLDIER | SIDE_RED;
  board[4][3] = TYPE_SOLDIER | SIDE_RED;
  board[6][3] = TYPE_SOLDIER | SIDE_RED;
  board[8][3] = TYPE_SOLDIER | SIDE_RED;

  board[0][9] = TYPE_CHARIOT | SIDE_BLACK;
  board[1][9] = TYPE_HORSE | SIDE_BLACK;
  board[2][9] = TYPE_ELEPHANT | SIDE_BLACK;
  board[3][9] = TYPE_ADVISOR | SIDE_BLACK;
  board[4][9] = TYPE_GENERAL | SIDE_BLACK;
  board[5][9] = TYPE_ADVISOR | SIDE_BLACK;
  board[6][9] = TYPE_ELEPHANT | SIDE_BLACK;
  board[7][9] = TYPE_HORSE | SIDE_BLACK;
  board[8][9] = TYPE_CHARIOT | SIDE_BLACK;

  board[1][7] = TYPE_CANNON | SIDE_BLACK;
  board[7][7] = TYPE_CANNON | SIDE_BLACK;
  board[0][6] = TYPE_SOLDIER | SIDE_BLACK;
  board[2][6] = TYPE_SOLDIER | SIDE_BLACK;
  board[4][6] = TYPE_SOLDIER | SIDE_BLACK;
  board[6][6] = TYPE_SOLDIER | SIDE_BLACK;
  board[8][6] = TYPE_SOLDIER | SIDE_BLACK;
}

QString Board::getImageFileName(int value) {
  QString result = ":/images/Xiangqi_%1_(Trad).svg";
  switch (value & TYPE_MASK) {
  case TYPE_ADVISOR:
    result = result.arg("Advisor");
    break;
  case TYPE_CANNON:
    result = result.arg("Cannon");
    break;
  case TYPE_CHARIOT:
    result = result.arg("Chariot");
    break;
  case TYPE_ELEPHANT:
    result = result.arg("Elephant");
    break;
  case TYPE_GENERAL:
    result = result.arg("General");
    break;
  case TYPE_HORSE:
    result = result.arg("Horse");
    break;
  case TYPE_SOLDIER:
    result = result.arg("Soldier");
    break;
  default:
    result = "";
    break;
  }

  return result;
}

void Board::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  painter.translate(SPACE, SPACE);

  for (int i = 0; i < 9; i++) {
    if (i == 0 || i == 8) {
      painter.drawLine(i * SPACE, 0, i * SPACE, 9 * SPACE);
    } else {
      painter.drawLine(i * SPACE, 0, i * SPACE, 4 * SPACE);
      painter.drawLine(i * SPACE, 5 * SPACE, i * SPACE, 9 * SPACE);
    }
  }
  for (int j = 0; j < 10; j++) {
    painter.drawLine(0, j * SPACE, 8 * SPACE, j * SPACE);
  }

  painter.drawLine(3 * SPACE, 0 * SPACE, 5 * SPACE, 2 * SPACE);
  painter.drawLine(5 * SPACE, 0 * SPACE, 3 * SPACE, 2 * SPACE);
  painter.drawLine(3 * SPACE, 7 * SPACE, 5 * SPACE, 9 * SPACE);
  painter.drawLine(5 * SPACE, 7 * SPACE, 3 * SPACE, 9 * SPACE);

  QFont font = painter.font();
  font.setPointSize(30);
  painter.setFont(font);
  painter.drawText(1 * SPACE, 4 * SPACE, 2 * SPACE, 1 * SPACE,
                   Qt::AlignHCenter | Qt::AlignVCenter, "楚 河");
  painter.drawText(5 * SPACE, 4 * SPACE, 2 * SPACE, 1 * SPACE,
                   Qt::AlignHCenter | Qt::AlignVCenter, "汉 界");

  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 10; j++) {
      int renderI, renderJ;

      if (isDragging && hoverValid && draggingX == i && draggingY == j) {
        // do nothing
        continue;
      } else if (isDragging && hoverValid && hoverX == i && hoverY == j) {
        renderI = draggingX;
        renderJ = draggingY;
      } else {
        renderI = i;
        renderJ = j;
      }

      painter.save();
      if (renderSide == SIDE_BLACK) {
        painter.translate((8 - i) * SPACE, j * SPACE);
      } else {
        painter.translate(i * SPACE, (9 - j) * SPACE);
      }
      painter.translate(-RADIUS, -RADIUS);
      painter.scale(0.05, 0.05);

      QString element;
      if ((board[renderI][renderJ] & SIDE_MASK) == SIDE_RED) {
        element = "red";
      } else {
        element = "black";
      }

      QString fileName = getImageFileName(board[renderI][renderJ]);
      if (fileName != "") {
        QSvgRenderer renderer(fileName);
        renderer.render(&painter, element);
      }

      painter.restore();
    }
  }
}

bool Board::getIndexByPos(QPointF position, int &x, int &y) {
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 10; j++) {
      int xx = (i + 1) * SPACE;
      int yy = (10 - j) * SPACE;
      double dist = sqrt((xx - position.x()) * (xx - position.x()) +
                         (yy - position.y()) * (yy - position.y()));
      if (dist < RADIUS) {
        if (renderSide == SIDE_BLACK) {
          i = 8 - i;
          j = 9 - j;
        }
        x = i;
        y = j;
        return true;
      }
    }
  }
  return false;
}

void Board::mousePressEvent(QMouseEvent *event) {
  QPointF pos = event->localPos();
  int x = -1, y = -1;
  bool onChess = getIndexByPos(pos, x, y);
  if (onChess && currentTurn == playerSide) {
    isDragging = true;
    draggingX = x;
    draggingY = y;

    setCursor(Qt::ClosedHandCursor);
  }
}

void Board::mouseMoveEvent(QMouseEvent *event) {
  QPointF pos = event->localPos();
  int x = -1, y = -1;
  bool onChess = getIndexByPos(pos, x, y);
  if (isDragging) {
    if (onChess) {
      if (x == draggingX && y == draggingY) {
        hoverX = x;
        hoverY = y;
        hoverValid = false;
      } else if (isMoveValid(board, draggingX, draggingY, x, y, currentTurn)) {
        hoverX = x;
        hoverY = y;
        hoverValid = true;
      }
    }
    update();
  } else {
    if (onChess && currentTurn == playerSide) {
      setCursor(Qt::OpenHandCursor);
    }
  }
}

void Board::mouseReleaseEvent(QMouseEvent *event) {
  QPointF pos = event->localPos();
  int x = -1, y = -1;
  bool onChess = getIndexByPos(pos, x, y);
  if (onChess && isDragging) {
    if (isMoveValid(board, draggingX, draggingY, x, y, currentTurn)) {
      if (x != draggingX || y != draggingY) {
        board[x][y] = board[draggingX][draggingY];
        board[draggingX][draggingY] = 0;
        setCurrentTurn(!currentTurn);
        update();
        emit onUserMove(draggingX, draggingY, x, y);
      }
    }
  }
  isDragging = false;
  setCursor(Qt::ArrowCursor);
}

bool Board::isDangerForSide(int board[9][10], int side) {
  int generalX, generalY;
  bool flag = false;
  for (int i = 0; i < 9 && !flag; i++) {
    for (int j = 0; j < 10; j++) {
      if (board[i][j] == (TYPE_GENERAL | side)) {
        generalX = i;
        generalY = j;
        flag = true;
        break;
      }
    }
  }

  int opponent = !side;
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 10; j++) {
      if ((board[i][j] & SIDE_MASK) == opponent) {
        // opponent chess

        int type = board[i][j] & TYPE_MASK;
        int from = 0, to = 0;
        int count = 0;
        switch (type) {
        case TYPE_ADVISOR:
          // impossible
          break;
        case TYPE_CANNON:
          if (i == generalX) {
            // same col
            from = min(j, generalY) + 1;
            to = max(j, generalY) - 1;
            for (int index = from; index <= to && count < 2; index++) {
              if (HAS_CHESS(board[i][index])) {
                count++;
              }
            }
          } else if (j == generalY) {
            // same row
            from = min(i, generalX) + 1;
            to = max(i, generalX) - 1;
            for (int index = from; index <= to && count < 2; index++) {
              if (HAS_CHESS(board[index][j])) {
                count++;
              }
            }
          }
          if (count == 1) {
            return true;
          }

          break;
        case TYPE_CHARIOT:
          if (i == generalX) {
            // same col
            from = min(j, generalY) + 1;
            to = max(j, generalY) - 1;
            for (int index = from; index <= to && count < 1; index++) {
              if (HAS_CHESS(board[i][index])) {
                count++;
              }
            }
            if (count == 0) {
              return true;
            }
          } else if (j == generalY) {
            // same row
            from = min(i, generalX) + 1;
            to = max(i, generalX) - 1;
            for (int index = from; index <= to && count < 1; index++) {
              if (HAS_CHESS(board[index][j])) {
                count++;
              }
            }
            if (count == 0) {
              return true;
            }
          }
          break;
        case TYPE_ELEPHANT:
          // impossible
          break;
        case TYPE_GENERAL:
          // same col
          if (i == generalX) {
            // same col
            from = min(j, generalY) + 1;
            to = max(j, generalY) - 1;
            for (int index = from; index <= to && count < 1; index++) {
              if (HAS_CHESS(board[i][index])) {
                count++;
              }
            }
            if (count == 0) {
              return true;
            }
          }
          break;
        case TYPE_HORSE:
          for (int dir = 0; dir < 8; dir++) {
            int ii = i + horseStep[dir][0];
            int jj = j + horseStep[dir][1];
            if (ii == generalX && jj == generalY) {
              int iBlock = i + horseBlock[dir][0];
              int jBlock = j + horseBlock[dir][1];
              if (!HAS_CHESS(board[iBlock][jBlock])) {
                return true;
              }
            }
          }
          break;
        case TYPE_SOLDIER:
          if (j == generalY) {
            // same row
            if (abs(i - generalX) == 1) {
              if (opponent == SIDE_BLACK && j <= 4) {
                return true;
              } else if (opponent == SIDE_RED && j >= 5) {
                return true;
              }
            }
          } else if (i == generalX) {
            // same col
            if (opponent == SIDE_RED && j + 1 == generalY) {
              return true;
            } else if (opponent == SIDE_BLACK && j - 1 == generalY) {
              return true;
            }
          }
        default:
          break;
        }
      }
    }
  }

  return false;
}

bool Board::isMoveValid(int board[9][10], int fromX, int fromY, int toX,
                        int toY, int side) {
  if (fromX == toX && fromY == toY)
    return false;
  if (!HAS_CHESS(board[fromX][fromY]) ||
      (board[fromX][fromY] & SIDE_MASK) != side)
    return false;
  if (HAS_CHESS(board[toX][toY]) && (board[toX][toY] & SIDE_MASK) == side)
    return false;

  int opponent = !side;
  int type = board[fromX][fromY] & TYPE_MASK;
  int from = 0, to = 0;
  int count = 0;
  int dir;
  switch (type) {
  case TYPE_ADVISOR:
    if (fromX == 4) {
      if (abs(fromX - toX) != 1 || abs(fromY - toY) != 1) {
        return false;
      }
    } else {
      // fromX = 3, 5
      if (side == SIDE_RED) {
        if (toX != 4 || toY != 1) {
          return false;
        }
      } else if (side == SIDE_BLACK) {
        if (toX != 4 || toY != 8) {
          return false;
        }
      }
    }
    break;
  case TYPE_CANNON:
    if (fromX == toX) {
      // same col
      from = min(fromY, toY) + 1;
      to = max(fromY, toY) - 1;
      for (int index = from; index <= to && count < 2; index++) {
        if (HAS_CHESS(board[fromX][index])) {
          count++;
        }
      }
    } else if (fromY == toY) {
      // same row
      from = min(fromX, toX) + 1;
      to = max(fromX, toX) - 1;
      for (int index = from; index <= to && count < 2; index++) {
        if (HAS_CHESS(board[index][fromY])) {
          count++;
        }
      }
    } else {
      return false;
    }

    if (count >= 2) {
      return false;
    }

    if (count == 1 && !HAS_CHESS(board[toX][toY])) {
      return false;
    }

    if (count == 0 && HAS_CHESS(board[toX][toY]) &&
        (board[toX][toY] & SIDE_MASK) == opponent) {
      return false;
    }

    break;
  case TYPE_CHARIOT:
    if (fromX == toX) {
      // same col
      from = min(fromY, toY) + 1;
      to = max(fromY, toY) - 1;
      for (int index = from; index <= to && count < 1; index++) {
        if (HAS_CHESS(board[fromX][index])) {
          count++;
        }
      }
    } else if (fromY == toY) {
      // same row
      from = min(fromX, toX) + 1;
      to = max(fromX, toX) - 1;
      for (int index = from; index <= to && count < 1; index++) {
        if (HAS_CHESS(board[index][fromY])) {
          count++;
        }
      }
    } else {
      return false;
    }

    if (count >= 1) {
      return false;
    }
    break;
  case TYPE_ELEPHANT:
    for (dir = 0; dir < 4; dir++) {
      int xx = fromX + elephantStep[dir][0];
      int yy = fromY + elephantStep[dir][1];
      if (xx == toX && yy == toY) {
        if (HAS_CHESS(board[fromX + elephantBlock[dir][0]]
                           [fromY + elephantBlock[dir][1]])) {
          return false;
        }
        break;
      }
    }
    if (dir == 4) {
      return false;
    }
    if (side == SIDE_RED && toY > 4) {
      return false;
    } else if (side == SIDE_BLACK && toY < 5) {
      return false;
    }
    break;
  case TYPE_GENERAL:
    if (abs(fromX - toX) + abs(fromY - toY) != 1) {
      return false;
    }
    break;
  case TYPE_HORSE:
    for (dir = 0; dir < 8; dir++) {
      int xx = fromX + horseStep[dir][0];
      int yy = fromY + horseStep[dir][1];
      if (xx == toX && yy == toY) {
        if (HAS_CHESS(board[fromX + horseBlock[dir][0]]
                           [fromY + horseBlock[dir][1]])) {
          return false;
        }
        break;
      }
    }
    if (dir == 8) {
      return false;
    }
    break;
  case TYPE_SOLDIER:
    if (fromY == toY) {
      // same row
      if (abs(fromX - toX) != 1) {
        return false;
      }

      if (side == SIDE_RED && fromY <= 4) {
        return false;
      } else if (side == SIDE_BLACK && fromY >= 5) {
        return false;
      }
    } else if (fromX == toX) {
      // same col
      if (side == SIDE_RED && fromY + 1 != toY) {
        return false;
      } else if (side == SIDE_BLACK && fromY - 1 != toY) {
        return false;
      }
    } else {
      return false;
    }
    break;
  default:
    return false;
    break;
  }

  int temp[9][10];
  memcpy(temp, board, sizeof(temp));
  temp[toX][toY] = temp[fromX][fromY];
  temp[fromX][fromY] = 0;
  return !isDangerForSide(temp, side);
}

void Board::doMove(int fromX, int fromY, int toX, int toY) {
  if (isMoveValid(board, fromX, fromY, toX, toY, currentTurn)) {
    board[toX][toY] = board[fromX][fromY];
    board[fromX][fromY] = 0;
    setCurrentTurn(!currentTurn);
    update();
  }
}