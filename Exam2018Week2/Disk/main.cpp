#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QThread>
#include <cmath>
#include <iostream>

QStringList numbers;
const int MAX_K = 1024000;
int counter[4][MAX_K] = {{0}};
int sum[MAX_K] = {0};

inline int min(int a, int b) { return a > b ? b : a; }

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QString fileName = "/Volumes/Data/oopterm/Exam2018Week2/Disk/data.txt";

  QFile file(fileName);

  if (!file.open(QFile::ReadOnly)) {
    return 1;
  }
  QByteArray byteArray = file.readAll();
  QString content(byteArray);

  numbers = content.split("\n");

  int k;
  std::cin >> k;

  int count = numbers.size();
  int task_each_thread = (count + 3) / 4;
  QThread *workers[4];
  for (int i = 0; i < 4; i++) {
    int start = task_each_thread * i;
    int end = min(task_each_thread * (i + 1), count);
    workers[i] = QThread::create([=] {
      for (int ii = start; ii < end; ii++) {
        auto num = numbers[ii].toInt();
        if (ii < start + 3) {
          qWarning() << "<" << i << "," << num % k << ">";
        }
        counter[i][num % k]++;
      }
    });
    workers[i]->start();
  }
  for (int i = 0; i < 4; i++) {
    workers[i]->wait();
    delete workers[i];
  }
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < k; j++) {
      sum[j] += counter[i][j];
    }
  }
  double average = 0;
  for (int i = 0; i < k; i++) {
    average += sum[i];
  }
  average /= k;
  double square = 0;
  for (int i = 0; i < k; i++) {
    square += (sum[i] - average) * (sum[i] - average) / k;
  }
  qWarning() << "stderr = " << sqrt(square);
  return 0;
}
