#include <QApplication>
#include <QDebug>
#include "gui/gprocessdialog.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  
  GProcessDialog dlg;
  dlg.setTitleName("Push");
  dlg.setContent("Push");
  dlg.show();
  app.exec();
  return 0;
}
