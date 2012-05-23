#include "gitcommand.h"
#include <QDebug>

GitCommand::GitCommand(QWidget* parent, const QString& workDir) : QWidget(parent), m_workDir(workDir)
{
  m_process = new QProcess(this);
  m_process->setWorkingDirectory(workDir);
  QStringList env = QProcess::systemEnvironment();
  env << "GIT_FLUSH=0";
  m_process->setEnvironment(env);

  m_ispasswordNeeded = false;
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
    //qDebug() << "right end";
  }
}
void GitCommand::onProcessStateChanged(QProcess::ProcessState processState) {
  if (m_ispasswordNeeded && QProcess::Running == processState) {
   m_process->write(m_password.toLocal8Bit());
   m_process->closeWriteChannel();
  } 
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
  while(false == m_process->waitForFinished(200));//20ms to process;
}
const QString& GitCommand::output() const {
  return m_output;
}
void GitCommand::setPassword(const QString& password) {
  m_password = password;
  m_ispasswordNeeded = true; 
}
GitCommand::~GitCommand() {
}

