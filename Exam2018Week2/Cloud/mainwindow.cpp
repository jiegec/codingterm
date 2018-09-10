#include "mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QTextCodec>
#include <QThread>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) { setupUi(this); }

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
  if (server->listen(QHostAddress::LocalHost, 2018)) {
    showMessage("Listening on localhost:2018");
  } else {
    showMessage("Listening failed");
    delete server;
  }

  update();
}

void MainWindow::showMessage(QString content) {
  serverMessageLabel->setText(serverMessageLabel->text() + content + "\n");
  clientMessageLabel->setText(clientMessageLabel->text() + content + "\n");
  update();
}

void MainWindow::onClientConnection(qintptr fd) {
  QThread *thread = new QThread();
  ServerWorker *worker = new ServerWorker(this);
  thread->start();
  worker->moveToThread(thread);

  QTimer::singleShot(0, worker, [=] { worker->setFd(fd); });

  showMessage("Got client connection");
}

void MainWindow::doAction(QString action) {

  QThread *thread = new QThread();
  ClientWorker *worker = new ClientWorker(this);
  thread->start();
  worker->moveToThread(thread);

  QString fileName = fileNameLineEdit->text();
  QTimer::singleShot(0, worker, [=] {
    worker->connect();
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
      worker->sendAction(action, file.readAll());
    }
  });

  showMessage("Got client connection");
}

void MainWindow::onMax() {
  doAction("max");
  maxButton->setEnabled(false);
  update();
}
void MainWindow::onMin() {
  doAction("min");
  minButton->setEnabled(false);
  update();
}
void MainWindow::onAverage() {
  doAction("average");
  averageButton->setEnabled(false);
  update();
}
void MainWindow::onMedium() {
  doAction("medium");
  mediumButton->setEnabled(false);
  update();
}

void MainWindow::onChoose() {
  QString fileName = QFileDialog::getOpenFileName(this, "Select file");
  if (fileName != "") {
    fileNameLineEdit->setText(fileName);
  }
}

void MainWindow::finished(QString action) {
  if (action == "min") {
    minButton->setEnabled(true);
  }
  if (action == "max") {
    maxButton->setEnabled(true);
  }
  if (action == "average") {
    averageButton->setEnabled(true);
  }
  if (action == "medium") {
    mediumButton->setEnabled(true);
  }
}