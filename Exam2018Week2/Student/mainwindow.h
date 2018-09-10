#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QTranslator>
#include <QUdpSocket>

struct Student {
  QString student_id;
  QString name;
  QString gender;
  QString score;
};

class MainWindow : public QMainWindow, private Ui::MainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

signals:

protected:
  void changeEvent(QEvent *e) override;

private slots:
  void onListen();
  void onSubmit();
  void readPacket();

private:
  QTranslator translator;
  QUdpSocket *server;
  QUdpSocket *client;
  QVector<Student> students;
};

#endif // MAINWINDOW_H
