#ifndef WATCH_H
#define WATCH_H

#include <QWidget>

class Watch : public QWidget {
  Q_OBJECT
public:
  Watch(QWidget *parent = nullptr);

public slots:
  void setTime(int);

signals:

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  int currentTime;
};

#endif
