#ifndef SERVER_WORKER_H
#define SERVER_WORKER_H

#include <QTcpSocket>

class MainWindow;

class ServerWorker : public QObject {
  Q_OBJECT
public:
  ServerWorker(MainWindow *mw);

public slots:
  void setFd(qintptr fd);

private slots:
  void onDataAvailable();

private:
  QTcpSocket *current;
  MainWindow *mw;
  uint next_block_size;
};

#endif