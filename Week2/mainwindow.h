#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QTranslator>
#include <QTcpSocket>
#include <QTcpServer>

class MainWindow : public QMainWindow, private Ui::MainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

signals:

protected:
  void changeEvent(QEvent *e) override;

private slots:
  void onNewGame();
  void onSaveGame();
  void onLoadGame();
  void onUserMove(int fromX, int fromY, int toX, int toY);
  void onSocketAvailable();
  void onCurrentTurnChanged(int);
  void onCheck(int);
  void onCheckmate(int);
  void onSurrender();
  void onTimerChanged(int);
  void onTimeout();

private:
  QTranslator translator;
  QVector<const char *> languages;
  int numMoves;
  QTcpSocket *socket;
  QTcpServer *server;
};

#endif // MAINWINDOW_H
