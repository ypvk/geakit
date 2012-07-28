#include <git2.h>
#include <gui/gbranchview.h>
#include <QApplication>
#include <QDebug>

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  const char* path = "/Users/yuping/yp/git/test_api";
  git_repository* repos;
  int error = git_repository_open(&repos, path);
  if (error < GIT_SUCCESS) {
    qDebug() << "error open the repos";
    return 0;
  }
  GBranchView* m_branchView = new GBranchView(NULL, repos);
  m_branchView->show();

  app.exec();
  git_repository_free(repos);
  delete m_branchView;
  return 0;
}
