#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mytcpserver.h"
#include "serverworker.h"
#include "clientworker.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTranslator>

class MainWindow : public QMainWindow, private Ui::MainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

signals:

protected:
  void changeEvent(QEvent *e) override;

public slots:
  void showMessage(QString);
  void finished(QString);

private slots:
  void onListen();
  void onChoose();
  void onMin();
  void onMax();
  void onAverage();
  void onMedium();
  void doAction(QString);
  void onClientConnection(qintptr);

private:
  QTranslator translator;
  MyTcpServer *server;
};

#endif // MAINWINDOW_H
