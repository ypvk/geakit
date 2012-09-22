#include "gitcommand.h"
#include <QDir>
#include <QDebug>

const static QString TAG_HEAD = "refs/tags/";
const static QString BRANCH_HEAD = "refs/heads/";

GitCommand::GitCommand(QObject* parent, const QString& workDir) : QThread(parent), m_workDir(workDir)
{
  m_process = new QProcess(this);
  m_process->setWorkingDirectory(workDir);
  QStringList env = QProcess::systemEnvironment();
  env << "GIT_FLUSH=0";
  m_process->setEnvironment(env);

  m_waitTime = 10000;//default waitfor 10s;
  //m_shouldResetTheUrl = false;
  connect(m_process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(onProcessStateChanged(QProcess::ProcessState)));
  connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(redFromStdOut()));
  connect(m_process, SIGNAL(readyReadStandardError()), this, SLOT(redFromStdErr()));
  connect(m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
  connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));

  //define the pull_Fetch_file
  QString filePath;
#ifdef Q_OS_WIN
  filePath = "_netrc";
#else
  filePath = ".netrc";
#endif
  QString homePath = QDir::home().path();
  m_pullFetchFile = QDir::toNativeSeparators(homePath + "/" + filePath);
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
    qDebug() << "process, exitCode: " << exitCode;
  }
    this->removeEnviroment();
    emit finishedProcess();
}
void GitCommand::onProcessStateChanged(QProcess::ProcessState processState) {
  qDebug() << "status: " << processState;
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
  while(false == m_process->waitForFinished(m_waitTime));
  //m_process->waitForFinished(m_waitTime);
}
const QString& GitCommand::output() const {
  return m_output;
}
void GitCommand::setPassword(const QString& password) {
  m_password = password;
}

void GitCommand::setUsername(const QString& username) {
  m_username = username;
}
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
    QString filePath = QDir::toNativeSeparators(dir.path() + "/" + (*it));
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

  if (!dir.rmdir(QDir::toNativeSeparators(dir.path()))) {
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
  if (error < GIT_OK) {
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
  if (error < GIT_OK) {
    qDebug() << "error create new branch";
    return;
  }
  git_reference_free(head);
  git_reference_free(newBranch);
}

void GitCommand::gitAdd(const QStringList& fileList) {
  git_index* index;
  int error = git_repository_index(&index, m_repo);
  if (error < GIT_OK) {
    qDebug() << "error get the index";
  }
  QStringList::const_iterator it = fileList.constBegin();
  while ( it != fileList.constEnd()) {
    error = git_index_add(index, (*it).toLocal8Bit().constData(), (GIT_IDXENTRY_ADDED & GIT_IDXENTRY_STAGEMASK) >> GIT_IDXENTRY_STAGESHIFT);
    if (error < GIT_OK) qDebug() << "add failue";
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
  if (error < GIT_OK) {
    qDebug() << "error get the Commit";
  }
  error = git_commit_tree(&headTree, headCommit);
  if (error < GIT_OK) {
    qDebug() << "error get the tree";
  }
  error = git_index_read_tree(index, headTree, NULL);
  error = git_index_write(index);
  git_index_free(index); 
  git_tree_free(headTree);
  git_commit_free(headCommit);
  git_reference_free(head);
    //int error = git_repository_index(&index, m_repo);
  //TODO if error < GIT_OK
  //git_index_clear(index);
  git_index_free(index);
}
void GitCommand::createRemote(const QString& remoteName, const QString& remoteUrl) {
  /***************without git_remote_save(git_remote* remote) using shell instead*******
  git_remote* remoteNew;
  int error = git_remote_new(&remoteNew, m_repo, remoteUrl.toLocal8Bit().constData(), remoteName.toLocal8Bit().constData());
  if (error < GIT_OK) {
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
  //m_shouldResetTheUrl = true;
  m_projectName = projectName;

  this->setWaitTime(0);
  this->execute(cmd);
}
void GitCommand::gitResetConfigUrl(const QString& reposWorkdir)
{
  git_repository* tmpRepos;
  git_config* tmpConfig;
  int error = git_repository_open(&tmpRepos, reposWorkdir.toLocal8Bit().constData());
  if ( error < GIT_OK) {
    qDebug() << "open repository " << reposWorkdir;
    return;
  }
  error = git_repository_config(&tmpConfig, tmpRepos);
  if ( error < GIT_OK) {
    qDebug() << "open config failed";
  }
  const char* name = "remote.origin.url";
  const char* value;
  error = git_config_get_string(&value, tmpConfig, name);
  if (error == GIT_OK) {
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
QStringList GitCommand::gitRefs(git_ref_t ref_type)
{
  git_strarray strarray;
  int error;
  error = git_reference_list(&strarray, m_repo, ref_type);
  if (error < GIT_OK) {
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
QStringList GitCommand::gitBranches()
{
  git_ref_t ref_type = GIT_REF_OID;
  QStringList all_refs = gitRefs(ref_type);
  QStringList::const_iterator it = all_refs.constBegin();
  QStringList branches;
  while(it != all_refs.constEnd())
  {
    if (it->startsWith(BRANCH_HEAD))
      branches << it->section('/', -1);
    it ++;
  }
  return branches;
} 
QStringList GitCommand::gitTags()
{
  git_ref_t ref_type = GIT_REF_OID;
  QStringList all_refs = gitRefs(ref_type);
  QStringList::const_iterator it = all_refs.constBegin();
  QStringList tags;
  while(it != all_refs.constEnd())
  {
    if (it->startsWith(TAG_HEAD))
      tags << it->section('/', -1);
    it ++;
  }
  return tags;
}
QString GitCommand::gitRefHead() 
{
  git_reference* head;
  int error = git_repository_head(&head, m_repo);
  if (error < GIT_OK) {
    qDebug() << "error get the head";
    return QString();
  }
  QString headName = git_reference_name(head);
  git_reference_free(head);
  return headName.section('/', -1);
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
const QString GitCommand::gitHeadCommitOid()
{
  git_reference* head;
  int error = git_repository_head(&head, m_repo);
  if (error < GIT_OK)
  { 
    qDebug() << "error get head oid";
    return QString();
  }
  const git_oid* refoid = git_reference_oid(head);
  char headCommitId[41] = {0};
  git_oid_fmt(headCommitId, refoid);
  git_reference_free(head);
  return QString(headCommitId);
}
bool GitCommand::gitChangeBranch(const QString& branchName)
{
  git_reference* newHead;
  int error = git_reference_create_symbolic(&newHead, m_repo, "HEAD", (BRANCH_HEAD + branchName).toLocal8Bit().constData(), 1);
  if (error < GIT_OK) {
    qDebug() << "error change the branch";
    return false;
  }  
  git_reference_free(newHead);
  return true;
}
bool GitCommand::gitDeleteBranch(const QString& branchName)
{
  git_reference* branch;
  int error = git_reference_lookup(&branch, m_repo, branchName.toLocal8Bit().constData());
  if (error < GIT_OK) {
    qDebug() << "Can't get the branch"; 
    return false;
  }
  error = git_reference_delete(branch);
  if (error < GIT_OK) {
    qDebug() << "can't delete";
    return false;
  }
  git_reference_free(branch);
  return true;
}
QStringList GitCommand::gitRemoteBranches(const QString& remoteName)
{
  QStringList remoteBranches;
  QString headPoint = "HEAD";
  QStringList branches = gitRefs();
  QStringList::const_iterator it = branches.constBegin();
  for (; it != branches.constEnd(); it++) {
    if (it->contains(remoteName) && !it->contains(headPoint)){
      remoteBranches << it->section('/', -2, -1);
    }
  }
  return remoteBranches;
}
QList <QStringList> GitCommand::gitCommitDatas()
{

  QList <QStringList> commitDatas;
  git_reference* m_reference;
  int error;
  error = git_repository_head(&m_reference, m_repo);

  const git_oid* oid = git_reference_oid(m_reference);
    
  git_revwalk* walk;
  git_commit* wcommit;

  git_revwalk_new(&walk, m_repo);
  git_revwalk_sorting(walk, GIT_SORT_TIME);// | GIT_SORT_REVERSE);
  git_revwalk_push(walk, oid);
  git_oid tmpOid;
  while ((git_revwalk_next(&tmpOid, walk)) == GIT_OK) {
    error = git_commit_lookup(&wcommit, m_repo, &tmpOid);
    if (error == -1) {qDebug() << "error get the commit!"; break;}

    QStringList commitData;
    const git_signature* cauth;
    const char* cmsg;
    char sha[41] = {0};
    const char* time;
    cmsg = git_commit_message(wcommit);
    cauth = git_commit_author(wcommit);
    git_oid_fmt(sha, oid);
    git_time gTime = cauth->when;

    QString qSha(sha);
    QString message(cmsg);
    QString author(cauth->name);
    QString shaShort;
    for (int i = 0; i < 10; i++)
      shaShort.append(sha[i]);
    time_t tmpTime = (time_t)gTime.time;
    time = ctime(&tmpTime);
    QString qtime(time);
    
    commitData << qSha << shaShort << author << qtime << message;
    commitDatas << commitData;
    git_commit_free(wcommit);
  }
  git_revwalk_free(walk);
  git_reference_free(m_reference);
  return commitDatas;
}

bool GitCommand::gitMergeBranch(const QString& branchName)
{
  QString cmd = "git merge " + branchName;
  this->execute(cmd);
  qDebug() << this->output();
  return true;
}

bool GitCommand::gitPush(const QString& url)
{
  if (!this->setupEnvironment()) {
    return false;
  }
  QString cmd = QString("git push %1").arg(url);
  this->setWaitTime(0);
  this->execute(cmd);
  qDebug() << this->output();
  //TODO
  return true;
}
QString GitCommand::gitRemoteUrl(const QString& remoteName)
{
  git_remote* m_remote;
  int error = git_remote_load(&m_remote, m_repo, remoteName.toLocal8Bit().constData());
  if (error < GIT_OK) {
    qDebug() << "error get remote";
    return QString();
  }
  QString remoteUrl = QString(git_remote_url(m_remote));
  git_remote_free(m_remote);
  return remoteUrl;
}
bool GitCommand::gitFetch(const QString& url)
{
  if (!this->setupEnvironment()) {
    return false;
  }
  QString cmd = QString("git fetch %1").arg(url);
  this->setWaitTime(0);
  this->execute(cmd);
  qDebug() << this->output();
  //TODO
  return true;
}
bool GitCommand::setupEnvironment()
{  
  QFile file(m_pullFetchFile);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    qDebug() << "error open file";
    return false;
  } 
  QTextStream out(&file);
  out << "machine " << "github.com" << "\n";
  out << "login " << m_username << "\n";
  out << "password " << m_password << "\n";
  file.close();
  return true;
}

void GitCommand::removeEnviroment()
{
  QFile file(m_pullFetchFile);
  if (file.exists())
  {
    file.remove();
  }
}
GitCommand::~GitCommand() {
}

