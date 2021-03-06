#include "newgamedialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegExp>

NewGameDialog::NewGameDialog(QWidget *parent) : QDialog(parent) {
  setupUi(this);

  auto addresses = QNetworkInterface::allAddresses();
  interfaceAddresses->setText("");
  for (auto address : addresses) {
    if (address.protocol() == QAbstractSocket::IPv4Protocol)
      interfaceAddresses->setText(interfaceAddresses->text() + "\n\t" +
                                  address.toString());
  }

  publicAddress->setText("Retriving");
  connect(&network, &QNetworkAccessManager::finished, this,
          [=](QNetworkReply *reply) {
            if (reply->error()) {
              publicAddress->setText("Unavailable");
            } else {
              publicAddress->setText("\t" + reply->readAll());
            }
          });

  network.get(QNetworkRequest(QUrl("https://ipecho.net/plain")));

  cancelButton->setEnabled(false);
  server = nullptr;
  socket = nullptr;
}

void NewGameDialog::onListen() {
  server = new QTcpServer;

  QRegExp ip("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-"
             "9]|[01]?[0-9][0-9]?)$");
  if (!ip.exactMatch(hostIpAddress->text())) {
    QMessageBox msgBox;
    msgBox.setText("Bad ip address");
    msgBox.exec();
    return;
  }

  if (server->listen(QHostAddress(hostIpAddress->text()),
                     hostPort->text().toInt())) {
    listenButton->setEnabled(false);
    cancelButton->setEnabled(true);
    tabWidget->setTabEnabled(1, false);
    tabWidget->setTabEnabled(2, false);

    connect(server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
  } else {
    delete server;
    server = nullptr;

    QMessageBox msgBox;
    msgBox.setText(tr("Failed to listen."));
    msgBox.exec();
  }
}

void NewGameDialog::onCancel() {
  if (server) {
    server->close();
    delete server;
  }

  listenButton->setEnabled(true);
  cancelButton->setEnabled(false);
  tabWidget->setTabEnabled(1, true);
  tabWidget->setTabEnabled(2, true);
}

void NewGameDialog::onNewConnection() {
  socket = server->nextPendingConnection();
  side = SIDE_RED;
  server->close();
  accept();
}

void NewGameDialog::onConnect() {
  QRegExp ip("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-"
             "9]|[01]?[0-9][0-9]?)$");
  if (!ip.exactMatch(clientIpAddress->text())) {
    QMessageBox msgBox;
    msgBox.setText("Bad ip address");
    msgBox.exec();
    return;
  }

  socket = new QTcpSocket();
  socket->connectToHost(clientIpAddress->text(), clientPort->text().toInt());
  connect(socket, SIGNAL(connected()), this, SLOT(onSocketConnected()));
  connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this,
          SLOT(onSocketError()));

  connectButton->setEnabled(false);
  tabWidget->setTabEnabled(0, false);
  tabWidget->setTabEnabled(2, false);
}

void NewGameDialog::onSocketConnected() {
  side = SIDE_BLACK;
  accept();
}

void NewGameDialog::onSocketError() {
  delete socket;
  socket = nullptr;

  QMessageBox msgBox;
  msgBox.setText(tr("Failed to connect."));
  msgBox.exec();

  connectButton->setEnabled(true);
  tabWidget->setTabEnabled(0, true);
  tabWidget->setTabEnabled(2, true);
}

void NewGameDialog::onChooseFile() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"));
  if (fileName == "") {
    return;
  }

  chooseFileTextEdit->setText(fileName);
}

void NewGameDialog::onLoadFile() {
  loadFilePath = chooseFileTextEdit->text();
  accept();
}
