
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QTranslator>
#include <QTimer>

class MainWindow : public QMainWindow, private Ui::MainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

signals:

protected:
  void changeEvent(QEvent *e) override;

private slots:
  void onStart();
  void onPause();
  void onReset();
  void onTick();

private:
  void setTime(int time);
  QTranslator translator;
  int currentTime;
  QTimer timer;
  bool isPausing;
};

#endif // MAINWINDOW_H
