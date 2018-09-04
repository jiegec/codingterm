#include "mainwindow.h"
#include "newgamedialog.h"
#include <QDebug>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), languages({"en", "zh"}) {
  setupUi(this);

  tr("en"), tr("zh");
  if (translator.load(":/translations/")) {
    qWarning() << "Loaded translation for default locale";
  } else {
    translator.load(":/translation/en");
    qWarning() << "Using en for fallback locale";
  }
  QApplication::installTranslator(&translator);

  newGame();
}

void MainWindow::newGame() {
  NewGameDialog dialog;
  dialog.exec();

  if (!dialog.ok) {
    close();
  }
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
