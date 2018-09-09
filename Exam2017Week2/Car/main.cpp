#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QSemaphore>
#include <QTextCodec>
#include <QThread>

QStringList cars;
QStringList queue[5][5];

inline int min(int a, int b) { return a > b ? b : a; }

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QString fileName = QFileDialog::getOpenFileName(nullptr, "Open File");
  if (fileName == "") {
    return 1;
  }

  QFile file(fileName);

  if (!file.open(QFile::ReadOnly)) {
    return 1;
  }
  QByteArray byteArray = file.readAll();
  QTextCodec::ConverterState state;
  const char *codecNames[2] = {"GBK", "UTF-8"};
  QString content;
  for (int i = 0; i < 2; i++) {
    QTextCodec *codec = QTextCodec::codecForName(codecNames[i]);
    content = codec->toUnicode(byteArray.constData(), byteArray.size(), &state);
    if (state.invalidChars == 0) {
      break;
    }
  }

  cars = content.split("\n");

  int count = cars.size();
  int task_each_thread = (count + 4) / 5;
  QThread *workers[5];
  QSemaphore *semaphore = new QSemaphore(0);
  QSemaphore *semaphore2 = new QSemaphore(0);
  for (int i = 0; i < 5; i++) {
    int start = task_each_thread * i;
    int end = min(task_each_thread * (i + 1), count);
    workers[i] = QThread::create([=] {
      qWarning() << i << "begin 1" << "from" << start << "to" << end;
      for (int ii = start; ii < end; ii++) {
        auto arr = cars[ii].toLatin1();
        if (arr.size()) {
          char last = arr[arr.size() - 1];
          int num = last - '0';
          queue[i][num % 5].push_back(cars[ii]);
        }
      }
      qWarning() << i << "ended 1";
      semaphore->release(1);
      semaphore2->acquire(1);
      qWarning() << i << "begin 2";
      QFile file(QString("output%1").arg(i));

      if (!file.open(QFile::WriteOnly)) {
        return;
      }

      QTextStream stream(&file);
      for (int ii = 0; ii < 5; ii++) {
        for (auto &car : queue[ii][i]) {
          stream << car << "\n";
        }
      }
      file.close();
    });
    workers[i]->start();
  }
  semaphore->acquire(5);
  semaphore2->release(5);
  for (int i = 0; i < 5; i++) {
    workers[i]->wait();
    delete workers[i];
  }
  return 0;
}
