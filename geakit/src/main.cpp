#include <QApplication>

#include "gui/gmainwindow.h"
#include "version.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("Geakit Inc.");
  QCoreApplication::setOrganizationDomain("geakit.com");
  QCoreApplication::setApplicationName("Geakit Clinet");
  QCoreApplication::setApplicationVersion(GEAKIT_VERSION);
  GMainWindow w;
  w.show();
  return app.exec();
}
