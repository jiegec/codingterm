#include "board.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  Board board1(nullptr, false);
  board1.show();

  Board board2(nullptr, true);
  board2.show();

  return app.exec();
}
