#include <QApplication>

#include "gui/gprojectdialog.h"
#include "version.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("Geakit Inc.");
  QCoreApplication::setOrganizationDomain("geakit.com");
  QCoreApplication::setApplicationName("Geakit Clinet");
  QCoreApplication::setApplicationVersion(GEAKIT_VERSION);
  git_repository* repo;
//  const char* path = "/home/yuping/yp/git/git_myself";
  const char* path = "/home/yuping/yp/git/new_git";
//  const char* path = "/home/yuping/yp/geakit/geakit";
  git_repository_open(&repo, path);

  GProjectDialog w(0, repo);
  //GMainWindow w;
  w.show();
  app.exec();
  //git_repository_free(repo);
  return 0;
}
