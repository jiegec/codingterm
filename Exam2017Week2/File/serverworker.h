#ifndef SERVER_WORKER_H
#define SERVER_WORKER_H

#include <QTcpSocket>

class MainWindow;

class ServerWorker : public QObject {
  Q_OBJECT
public:
  ServerWorker(MainWindow *mw);
  void setAnother(ServerWorker *another);

public slots:
  void allowSendFile();
  void setFd(qintptr fd);

private slots:
  void onDataAvailable();
  void sendDataTo(const QString &, const QByteArray &);

private:
  QTcpSocket *current;
  ServerWorker *another;
  MainWindow *mw;
};

#endif