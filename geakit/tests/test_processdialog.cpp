#include <QApplication>
#include <QDebug>
#include "gui/gprocessdialog.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  QString dir = "/Users/yuping/yp/git/test_api";
  GitCommand* cmd = new GitCommand(0, dir);

  git_repository* repo;
  git_repository_open(&repo, dir.toLocal8Bit().constData());
  cmd->setRepository(repo);
  
  GProcessDialog dlg;
  dlg.setTitleName("Push");
  dlg.setContent("Push");
  dlg.show();
  app.exec();
  return 0;
}
