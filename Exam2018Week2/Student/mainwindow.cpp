#include "mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QNetworkDatagram>
#include <QTextCodec>

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
  server = new QUdpSocket(this);
  server->bind(QHostAddress::LocalHost, 2333);

  connect(server, SIGNAL(readyRead()), this, SLOT(readPacket()));

  listenButton->setEnabled(false);
  update();
}

void MainWindow::onSubmit() {
  QString name = nameLineEdit->text();
  if (name.size() > 10) {
    QMessageBox msgBox;
    msgBox.setText("Invalid input");
    msgBox.exec();
    return;
  }

  QString student_id = studentIdLineEdit->text();
  if (student_id.size() != 10) {
    QMessageBox msgBox;
    msgBox.setText("Invalid input");
    msgBox.exec();
    return;
  }

  QString gender = genderComboBox->currentText();
  QString score = scoreLineEdit->text();
  int score_num = score.toInt();
  if (score_num < 0 || score_num > 100) {
    QMessageBox msgBox;
    msgBox.setText("Invalid input");
    msgBox.exec();
    return;
  }

  client = new QUdpSocket(this);
  QByteArray buffer;
  QDataStream stream(&buffer, QIODevice::WriteOnly);

  stream << name << student_id << gender << score;

  client->writeDatagram(buffer, QHostAddress::LocalHost, 2333);

  return;
}

void MainWindow::readPacket() {
  while (server->hasPendingDatagrams()) {
    QNetworkDatagram datagram = server->receiveDatagram();
    QDataStream stream(datagram.data());
    Student student;
    stream >> student.name >> student.student_id >> student.gender >>
        student.score;
    messageLabel->setText(
        messageLabel->text() +
        QString("Got student with name %1 student id %2 gender %3 score %4")
            .arg(student.name)
            .arg(student.student_id)
            .arg(student.gender)
            .arg(student.score) +
        "\n");
    bool flag = true;
    for (auto obj : students) {
      if (obj.student_id == student.student_id) {
        flag = false;
        break;
      }
    }
    if (flag) {
      QFile file("/Volumes/Data/oopterm/Exam2018Week2/Student/output.txt");
      file.open(QIODevice::WriteOnly);
      QTextStream stream(&file);
      students.push_back(student);
      for (auto obj : students) {
        stream << obj.name << ",";
        stream << obj.student_id << ",";
        stream << obj.gender << ",";
        stream << obj.score << "\r\n";
      }
      stream.flush();
      file.close();
      messageLabel->setText(messageLabel->text() + "Saved to file." + "\n");
    } else {
      messageLabel->setText(messageLabel->text() + "Already exists" + "\n");
    }
  }
}