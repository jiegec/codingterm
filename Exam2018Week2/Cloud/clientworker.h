#ifndef CLIENT_WORKER_H
#define CLIENT_WORKER_H

#include <QTcpSocket>

class MainWindow;

class ClientWorker : public QObject {
  Q_OBJECT

public:
  ClientWorker(MainWindow *mw);

signals:
  void allowSendFile();

public slots:
  void sendAction(QString, QByteArray);
  void connect();

private slots:
  void onDataAvailable();

private:
  QTcpSocket *socket;
  MainWindow *mw;
};

#endif