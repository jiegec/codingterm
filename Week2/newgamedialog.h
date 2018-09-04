#ifndef NEWGAMEDIALOG_H
#define NEWGAMEDIALOG_H

#include "board.h"
#include "ui_newgamedialog.h"
#include <QDialog>
#include <QNetworkAccessManager>
#include <QTcpServer>
#include <QTranslator>
#include <QCloseEvent>

class NewGameDialog : public QDialog, private Ui::NewGameDialog {
  Q_OBJECT

public:
  explicit NewGameDialog(QWidget *parent = nullptr);
  QTcpSocket *socket;
  QTcpServer *server;
  int side;

signals:

protected:

private slots:
  void onListen();
  void onCancel();
  void onNewConnection();

  void onConnect();
  void onSocketConnected();
  void onSocketError();

private:
  QTranslator translator;
  QNetworkAccessManager network;
};

#endif // MAINWINDOW_H
