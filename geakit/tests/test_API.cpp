#include <api/gaccountapi.h>
#include <QApplication>

int main (int argc, char* argv[]) {
  QApplication app(argc, argv);
  GAccountAPI api;
  return app.exec();
} 
