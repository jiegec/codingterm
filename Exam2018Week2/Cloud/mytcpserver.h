#ifndef MY_TCP_SERVER_H
#define MY_TCP_SERVER_H

#include <QTcpServer>

class MyTcpServer : public QTcpServer {
  Q_OBJECT

public:
  MyTcpServer(QObject *parent = nullptr) : QTcpServer(parent) {}

signals:
  void newConnection(qintptr fd);

protected:
  virtual void incomingConnection(qintptr fd) override;
};

#endif