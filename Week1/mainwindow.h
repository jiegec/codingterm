#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMainWindow>

class MainWindow : public QMainWindow, private Ui::MainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

signals:
  void sideChanged(int);

protected:
  void changeEvent(QEvent *e) override;

private slots:
  void onSideChanged(int);

private:
  int side;
};

#endif // MAINWINDOW_H
