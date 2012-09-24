#include <QApplication>
#include <gui/gitcommand.h>
#include <QDebug>

int main (int argc, char* argv[])
{
  QApplication app(argc, argv);
  QString dir = "/Users/yuping/yp/git/test_api";
  GitCommand* cmd = new GitCommand(0, dir);

  git_repository* repo;
  git_repository_open(&repo, dir.toLocal8Bit().constData());
  cmd->setRepository(repo);

  qDebug() << "0" << cmd->gitRefs();
  qDebug() << "1" << cmd->gitRefs(GIT_REF_PACKED);
  qDebug() << "2" << cmd->gitRefs(GIT_REF_OID);
  qDebug() << "3" << cmd->gitRefs(GIT_REF_SYMBOLIC);
  qDebug() << "4" << cmd->gitBranches();
  qDebug() << "5" << cmd->gitTags();
  qDebug() << "6" << cmd->gitRefHead();
  qDebug() << "7" << cmd->gitRemoteBranches("origin");
  //qDebug() << "8" << cmd->gitChangeBranch("abc");
  //qDebug() << "10" << "gitCheckoutHEAD() " << cmd->gitCheckoutHEAD();
  //qDebug() << "11" << "gitCheckoutIndex() " << cmd->gitCheckoutIndex();
  //qDebug() << "12" << "gitCheckoutTree() " << cmd->gitCheckoutTree();
  QString remoteUrl = cmd->gitRemoteUrl("origin");
  qDebug() << "13" << "gitFetch()" << cmd->gitFetch(remoteUrl);
  qDebug() << "14" << "gitDiffWorkDirToIndex";
  cmd->gitDiffWorkDirToIndex();
  qDebug() << "15" << "gitDiffIndexToTree";
  cmd->gitDiffIndexToTree();
  try {
  qDebug() << "9" << cmd->gitRemoteUrl("origin"); 
  }catch(...)
  {qDebug() << "error";}
  git_repository_free(repo);
  delete cmd;
  return 0;
}
  
