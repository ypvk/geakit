#include "gitcommand.h"
#include <QDir>
#include <QDebug>

GitCommand::GitCommand(QObject* parent, const QString& workDir) : QThread(parent), m_workDir(workDir)
{
  m_process = new QProcess(this);
  m_process->setWorkingDirectory(workDir);
  QStringList env = QProcess::systemEnvironment();
  env << "GIT_FLUSH=0";
  m_process->setEnvironment(env);

  m_waitTime = 10000;//default waitfor 10s;
  m_shouldResetTheUrl = false;
  connect(m_process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(onProcessStateChanged(QProcess::ProcessState)));
  connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(redFromStdOut()));
  connect(m_process, SIGNAL(readyReadStandardError()), this, SLOT(redFromStdErr()));
  connect(m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
  connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
}
void GitCommand::redFromStdOut() {
  m_output.clear();
  m_output = m_process->readAllStandardOutput();
}
void GitCommand::redFromStdErr() {
 // m_content.append(m_process->readAllStandardError());
  qDebug() << "error code : " << m_process->readAllStandardError();
}
void GitCommand::processError(QProcess::ProcessError error) 
{
  if (error == QProcess::FailedToStart)
  {
    qDebug() << "error start";
  }
}
void GitCommand::processFinished(int exitCode, QProcess::ExitStatus exitStatus) {
  if (exitStatus == QProcess::CrashExit) 
  {
    qDebug() << "crashed";
  }
  else 
  {
    qDebug() << "in the process, exitCode: " << exitCode;
    if (m_shouldResetTheUrl) {
      QString dir = QDir::toNativeSeparators(m_workDir + "/" + m_projectName);
      gitResetConfigUrl(dir);
      m_shouldResetTheUrl = false;
    }
  }
    emit finishedProcess();
}
void GitCommand::onProcessStateChanged(QProcess::ProcessState processState) {
  qDebug() << "status: " << processState;
 /* if (QProcess::Running == processState) {
   //m_process->write(m_password.toLocal8Bit());
   //m_process->closeWriteChannel();
    qDebug() << "stats running";
  } 
  */
}
void GitCommand::execute(const QString& cmd) {
  QStringList argList = cmd.split(" ");
  QString firstCmd = argList[0];
  argList.pop_front();
/*
  qDebug() << firstCmd;
  for( int i = 0; i < argList.size(); i++) {
    qDebug() << argList[i];
  }
  */
  m_process->start(firstCmd, argList);
  if (!m_process->waitForStarted())
  {
    qDebug() << "error start process ";
    return;
  }
//  while(false == m_process->waitForFinished(m_waitTime));
  m_process->waitForFinished(m_waitTime);
}
const QString& GitCommand::output() const {
  return m_output;
}
/*
void GitCommand::setPassword(const QString& password) {
  m_password = password;
  m_ispasswordNeeded = true; 
}
*/
void GitCommand::setWorkDir(const QString& workDir) {
  m_workDir = workDir;
  m_process->setWorkingDirectory(m_workDir);
}
bool GitCommand::removeGitDir(const QString& dirName) {
  QDir dir(dirName);
  if (!dir.exists()) {
    return true;
  }
  bool error = false;
  QStringList files = dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
  QList<QString>::iterator it = files.begin();
  for ( ;it != files.end(); it++) {
    QString filePath = QDir::convertSeparators(dir.path() + "/" + (*it));
    QFileInfo fileInfo(filePath);
    if (fileInfo.isFile() || fileInfo.isSymLink()) {
      QFile::setPermissions(filePath, QFile::WriteOwner);
      if (!QFile::remove(filePath)) {
        qDebug() << "error";
        error = true;
      }
    }
    else if (fileInfo.isDir()) {
      if (!removeGitDir(filePath)) {
          error = true;
          }
    }
  }

  if (!dir.rmdir(QDir::convertSeparators(dir.path()))) {
    qDebug() << "error remove the dir.path()";
    error = true;
  }
  return !error;

}
void GitCommand::setCmd(const QString& cmd) {
  m_cmd = cmd;
}
//asyncrinized
void GitCommand::run() {

  QProcess::execute(m_cmd);
}
QProcess* GitCommand::getProcess() const {
  return m_process;
}
void GitCommand::setWaitTime(int waitTime) {
  m_waitTime = waitTime;
}

void GitCommand::setRepository(git_repository* repo) {
  m_repo = repo;
}
void GitCommand::createBranch(const QString& branchName) {
  git_reference* head;
  int error = git_repository_head(&head, m_repo);
  if (error < GIT_SUCCESS) {
    qDebug() << "error get the head";
    return;
  }
  const git_oid* head_oid = git_reference_oid(head);
  char my_oid[41] = {0};
  git_oid_fmt(my_oid, head_oid);
  qDebug() << "oid is : " << my_oid;
  git_reference* newBranch;
  QString wholeName = QString("refs/heads/%1").arg(branchName);
  error = git_reference_create_oid(&newBranch, m_repo, wholeName.toLocal8Bit().constData(), head_oid, 1);
  if (error < GIT_SUCCESS) {
    qDebug() << "error create new branch";
    return;
  }
  git_reference_free(head);
  git_reference_free(newBranch);
}

void GitCommand::gitAdd(const QStringList& fileList) {
  git_index* index;
  int error = git_repository_index(&index, m_repo);
  if (error < GIT_SUCCESS) {
    qDebug() << "error get the index";
  }
  QStringList::const_iterator it = fileList.constBegin();
  while ( it != fileList.constEnd()) {
    error = git_index_add(index, (*it).toLocal8Bit().constData(), (GIT_IDXENTRY_ADDED & GIT_IDXENTRY_STAGEMASK) >> GIT_IDXENTRY_STAGESHIFT);
    if (error < GIT_SUCCESS) qDebug() << "add failue";
    it++;
  }
    error = git_index_write(index);
    git_index_free(index);
}
void GitCommand::gitReverse() {
  git_index* index;
  git_commit* headCommit;
  git_reference* head;
  const git_oid* oid;
  git_tree* headTree;

  int error = git_repository_head(&head, m_repo);
  error = git_repository_index(&index, m_repo);
  oid = git_reference_oid(head);
  error = git_commit_lookup(&headCommit, m_repo, oid);
  if (error < GIT_SUCCESS) {
    qDebug() << "error get the Commit";
  }
  error = git_commit_tree(&headTree, headCommit);
  if (error < GIT_SUCCESS) {
    qDebug() << "error get the tree";
  }
  error = git_index_read_tree(index, headTree);
  error = git_index_write(index);
  git_index_free(index); 
  git_tree_free(headTree);
  git_commit_free(headCommit);
  git_reference_free(head);
    //int error = git_repository_index(&index, m_repo);
  //TODO if error < GIT_SUCCESS
  //git_index_clear(index);
  git_index_free(index);
}
void GitCommand::createRemote(const QString& remoteName, const QString& remoteUrl) {
  /***************without git_remote_save(git_remote* remote) using shell instead*******
  git_remote* remoteNew;
  int error = git_remote_new(&remoteNew, m_repo, remoteUrl.toLocal8Bit().constData(), remoteName.toLocal8Bit().constData());
  if (error < GIT_SUCCESS) {
    qDebug() << "error create new remote";
    return;
  }
  qDebug() << "remote Name: " << git_remote_name(remoteNew);
  git_remote_free(remoteNew);
  *****************************end***************************************************/
  QString workDir(git_repository_workdir(m_repo));
  this->setWorkDir(workDir);
  QString cmd = QString("git remote add %1 %2").arg(remoteName).arg(remoteUrl);
  this->execute(cmd);
}
void GitCommand::removeRemote(const QString& remoteName) {
  // use shell first then may use libgit2 instead
  QString workDir(git_repository_workdir(m_repo));
  this->setWorkDir(workDir);
  QString cmd = QString("git remote rm %1").arg(remoteName);
  this->execute(cmd);
}
void GitCommand::gitCheckout(const QString& shaId) {
  //use shell first then may use libgit2 instead
  QString workDir(git_repository_workdir(m_repo));
  this->setWorkDir(workDir);
  QString cmd = QString("git checkout %1").arg(shaId);
  this->execute(cmd);
}
void GitCommand::gitClone(const QString& projectName, const QString& url)
{
  QString cmd = QString("git clone %1").arg(url);
  qDebug() << "cloning ... ";
  //do it asycronize
  m_shouldResetTheUrl = true;
  m_projectName = projectName;

  this->setWaitTime(0);
  this->execute(cmd);
}
void GitCommand::gitResetConfigUrl(const QString& reposWorkdir)
{
  git_repository* tmpRepos;
  git_config* tmpConfig;
  int error = git_repository_open(&tmpRepos, reposWorkdir.toLocal8Bit().constData());
  if ( error < GIT_SUCCESS) {
    qDebug() << "open repository " << reposWorkdir;
    return;
  }
  error = git_repository_config(&tmpConfig, tmpRepos);
  if ( error < GIT_SUCCESS) {
    qDebug() << "open config failed";
  }
  const char* name = "remote.origin.url";
  const char* value;
  error = git_config_get_string(tmpConfig, name, &value);
  if (error == GIT_SUCCESS) {
    qDebug() << name << ": " << value << endl;
  }
  QString url(value);
  //remove the password in the url
  QRegExp rx = QRegExp("(:[^//].+)?@");
  int pos = url.indexOf(rx);
  if (pos >= 0) {
    //qDebug() << rx.cap(1);
    url.replace(rx.cap(1), "");
  }
  //now set the value to the config file
  error = git_config_set_string(tmpConfig, name, url.toLocal8Bit().constData());
  //free the memory
  git_config_free(tmpConfig);
  git_repository_free(tmpRepos);
}
QStringList GitCommand::gitRefs()
{
  git_strarray strarray;
  int error;
  error = git_reference_listall(&strarray, m_repo, GIT_REF_LISTALL);
  if (error < GIT_SUCCESS) {
    qDebug() << "error List";
    return QStringList();
  }
  QStringList branchList;
  for (int i = 0; i < strarray.count; i++)
  {
    branchList << strarray.strings[i];
  }
  git_strarray_free(&strarray);
  return branchList;
}
QString GitCommand::gitRefHead() 
{
  git_reference* head;
  int error = git_repository_head(&head, m_repo);
  if (error < GIT_SUCCESS) {
    qDebug() << "error get the head";
    return QString();
  }
  QString headName = git_reference_name(head);
  git_reference_free(head);
  return headName;
}
QStringList GitCommand::gitRemoteNames()
{
  QString cmd = "git remote";
  this->setWaitTime(1000);//1s to work
  this->execute(cmd);
  if ("" != this->output()) {
    QStringList remoteNames = (this->output()).trimmed().split('\n');
    return remoteNames;
  }
  return QStringList();
}

GitCommand::~GitCommand() {
}

