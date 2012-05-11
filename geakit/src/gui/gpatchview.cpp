#include "gpatchview.h"
#include <QTextEdit>
#include <QDebug>

GPatchView::GPatchView(QWidget* parent,git_repository* repo) : QWidget(parent) {
 // m_commit = commit;
  m_repo = repo;
  m_process = new QProcess(this);
  m_text = new QTextEdit(this);
  m_text->setReadOnly(true);

  connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(redFromStdOut()));
  connect(m_process, SIGNAL(readyReadStandardError()), this, SLOT(redFromStdErr()));
  connect(m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
  connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
}
void GPatchView::setCommit(git_commit* commit) {
  m_commit = commit;
}
void GPatchView::redFromStdOut() {
  m_content = m_process->readAllStandardOutput();
  QRegExp rx("diff --git(.|\n)*");
  if (m_content.indexOf(rx) >= 0) m_content = rx.cap(0);
 // qDebug() << m_content;
  m_text->setText(m_content);
}
void GPatchView::redFromStdErr() {
 // m_content.append(m_process->readAllStandardError());
  qDebug() << m_content;
}
void GPatchView::processError(QProcess::ProcessError error) 
{
  if (error == QProcess::FailedToStart)
  {
    qDebug() << "error start";
  }
}
void GPatchView::processFinished(int exitCode, QProcess::ExitStatus exitStatus) {
  if (exitStatus == QProcess::CrashExit) 
  {
    qDebug() << "crashed";
  }
  else 
  {
    //qDebug() << "right end";
  }
}
void GPatchView::execute() 
{
  QString cmd = "git";
  QStringList argList;
  argList << "log" << "-1" << "-p";

  const git_oid* oid = git_commit_id(m_commit);
  char sha[40];
  git_oid_fmt(sha, oid);
  qDebug() << sha;
  QString qSha(sha);
  argList << qSha;

  QString path(git_repository_workdir(m_repo));  

  qDebug() << path;
  m_process->setWorkingDirectory(path);
  QStringList env = QProcess::systemEnvironment();
  env << "GIT_FLUSH=0";//skip the fflush() in the 'git_log'
  m_process->setEnvironment(env);

  m_process->start(cmd, argList);
 // while (false == m_process->waitForFinished());
  if (!m_process->waitForStarted())
  {
    qDebug() << "error start process";
    return;
  }
  while(false == m_process->waitForFinished(20));//20ms to prossee;

}
GPatchView::~GPatchView() {
}
