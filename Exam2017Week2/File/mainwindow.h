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
  void onAllowSendFile();

private slots:
  void onListen();
  void onConnect();
  void onSendFile();
  void onClientConnection(qintptr);
  void onSaveFile(QString, QByteArray);

private:
  QTranslator translator;
  MyTcpServer *server;
  qintptr fds[2];
  ServerWorker *workers[2];
  ClientWorker *client;
  int connected_clients;
};

#endif // MAINWINDOW_H
