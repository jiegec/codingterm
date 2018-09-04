#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QTranslator>

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

private:
  QTranslator translator;
  QVector<const char *> languages;
};

#endif // MAINWINDOW_H
