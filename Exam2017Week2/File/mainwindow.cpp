#include "mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QTextCodec>
#include <QThread>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setupUi(this);

  sendFileButton->setEnabled(false);
  connected_clients = 0;
}

void MainWindow::changeEvent(QEvent *e) {
  QWidget::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    retranslateUi(this);
    break;
  default:
    break;
  }
}

void MainWindow::onListen() {
  server = new MyTcpServer(this);

  connect(server, SIGNAL(newConnection(qintptr)), this,
          SLOT(onClientConnection(qintptr)));
  if (server->listen(QHostAddress::LocalHost, 1911)) {
    showMessage("Listening on localhost:1911");

    listenButton->setEnabled(false);
    connectButton->setEnabled(false);
    sendFileButton->setEnabled(false);
  } else {
    showMessage("Listening failed");
    delete server;
  }

  update();
}

void MainWindow::onConnect() {
  client = new ClientWorker(this);
  QThread *thread = new QThread();
  client->moveToThread(thread);
  thread->start();

  QTimer::singleShot(0, client, SLOT(connect()));
  connect(client, SIGNAL(allowSendFile()), this, SLOT(onAllowSendFile()));
  connect(client, SIGNAL(saveFile(QString, QByteArray)), this,
          SLOT(onSaveFile(QString, QByteArray)));

  listenButton->setEnabled(false);
  connectButton->setEnabled(false);
  sendFileButton->setEnabled(false);
  update();
}

void MainWindow::onSendFile() {
  QString fileName = QFileDialog::getOpenFileName(this, "Send File");
  if (fileName != "") {
    QThread *thread = QThread::create([=] {
      QFile file(fileName);

      if (file.open(QIODevice::ReadOnly)) {
        QByteArray array = file.readAll();
        QFileInfo fileInfo(fileName);
        QTimer::singleShot(
            0, client, [=] { client->sendFile(fileInfo.fileName(), array); });
      }
    });
    thread->start();

    listenButton->setEnabled(false);
    connectButton->setEnabled(false);
    sendFileButton->setEnabled(false);
    update();
  }
}

void MainWindow::showMessage(QString content) {
  messageLabel->setText(messageLabel->text() + content + "\n");
  update();
}

void MainWindow::onClientConnection(qintptr fd) {
  fds[connected_clients++] = fd;
  showMessage(QString("Got client #%1").arg(connected_clients));
  if (connected_clients == 2) {
    showMessage("Two clients now. Allow sending files");
    server->pauseAccepting();
    for (int i = 0; i < connected_clients; i++) {
      QThread *thread = new QThread();
      workers[i] = new ServerWorker(this);
      thread->start();
      workers[i]->moveToThread(thread);

      QTimer::singleShot(0, workers[i], [=] {
        workers[i]->setFd(fds[i]);
        workers[i]->allowSendFile();
      });
    }
    workers[0]->setAnother(workers[1]);
    workers[1]->setAnother(workers[0]);
  }
}

void MainWindow::onAllowSendFile() {
  showMessage("You can send file now");
  listenButton->setEnabled(false);
  connectButton->setEnabled(false);
  sendFileButton->setEnabled(true);
  update();
}

void MainWindow::onSaveFile(QString fileName, QByteArray content) {
  QString dir =
      QFileDialog::getExistingDirectory(this, "Save File " + fileName + " to");
  if (dir != "") {
    QFile file(dir + "/" + fileName);

    if (file.open(QIODevice::WriteOnly)) {
      file.write(content);
    }

    showMessage("File saved.");
  }
}