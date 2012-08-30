#ifndef GITCOMMAND_H
#define GITCOMMAND_H

//#include <QWidget>
#include <QProcess>
#include <QThread>
#include <git2.h>

class GitCommand : public QThread
{
  Q_OBJECT

  public:
    explicit GitCommand(QObject* parent = 0, const QString& workDir = "");
    ~GitCommand();
    void execute(const QString& cmd);
    void setWorkDir(const QString& workdir);
//    void setPassword(const QString& password);
    const QString& output() const;
    bool removeGitDir(const QString& dirName);
    void run();
    void setCmd(const QString& cmd);
    void createBranch(const QString& branchName);
    void createRemote(const QString& remoteName, const QString& remoteUrl);
    void removeRemote(const QString& remoteName);
    void gitCheckout(const QString& shaId);
    void gitClone(const QString& projectName, const QString& url);
    void gitResetConfigUrl(const QString& url);
    void setRepository(git_repository* repo);
    void gitAdd(const QStringList& fileList);
    void gitReverse();
    QString gitRefHead();
    QStringList gitRefs(git_rtype ref_type = GIT_REF_LISTALL);
    QStringList gitBranches();
    QStringList gitTags();
    QStringList gitRemoteNames();
    QStringList gitRemoteBranches(const QString& remoteName);
    bool gitChangeBranch(const QString& branchName);
    bool gitDeleteBranch(const QString& branchName);
    const QString gitHeadCommitOid();
    void setWaitTime(int waitTime);
    QProcess* getProcess() const;
  signals:
    void finishedProcess();//add signals if process is finished
  private slots:
    void redFromStdOut();
    void redFromStdErr();
    void processError(QProcess::ProcessError error);
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessStateChanged(QProcess::ProcessState processState);
  private:
    QString m_cmd;
    int m_waitTime;//time to wait before is finished
    QString m_output;
    QString m_workDir;
    QProcess* m_process;
    git_repository* m_repo;
//    bool m_ispasswordNeeded;
//    QString m_password;
    //to reset the config url
    bool m_shouldResetTheUrl;
    QString m_projectName;
};
#endif
