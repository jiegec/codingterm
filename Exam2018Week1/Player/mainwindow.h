
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QPushButton>
#include <QSignalMapper>
#include <QTranslator>
#include <QKeyEvent>
#include <QMediaPlayer>

class MainWindow : public QMainWindow, private Ui::MainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

signals:

protected:
  void changeEvent(QEvent *e) override;
  void keyPressEvent(QKeyEvent *e) override;

private slots:
  void onButtonPressed(int);
  void onPlayerStateChanged(QMediaPlayer::State state);

private:
  QTranslator translator;
  QPushButton *buttons[7];
  QSignalMapper mapper;
  QString text;
};

#endif // MAINWINDOW_H
