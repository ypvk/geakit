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
    void setPassword(const QString& password);
    void setUsername(const QString& username);
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
    void gitReset();
    int gitDiffWorkDirToIndex();
    int gitDiffIndexToTree();
    void gitReverse();
    QString gitRefHead();
    QStringList gitRefs(git_ref_t ref_type = GIT_REF_LISTALL);
    QStringList gitBranches();
    QStringList gitTags();
    QStringList gitRemoteNames();
    QStringList gitRemoteBranches(const QString& remoteName);
    QList<QStringList> gitCommitDatas();
    bool gitChangeBranch(const QString& branchName);
    bool gitDeleteBranch(const QString& branchName);
    bool gitMergeBranch(const QString& branchName);
    bool gitPush(const QString& url);
    bool gitFetch(const QString& url);
    bool gitCheckoutHEAD();
    bool gitCheckoutIndex();
    bool gitCheckoutTree();
    bool gitCommit(const QString& message, const QString& name, const QString& email);
    bool branchExists(const QString& branch, bool is_remote = false);
    QString gitRemoteUrl(const QString& remoteName);
    const QString gitHeadCommitOid();
    void setWaitTime(int waitTime);
    QProcess* getProcess() const;
    void kill();
  public:
    static QString diffFileInfosIndex;
    static QString diffFileInfosTree;
  private:
    int runSyc(const QString& cmd);
    bool setupEnvironment();
    void removeEnviroment();
    static int printer(void*, git_diff_delta*, git_diff_range*, char, const char*, size_t);
  signals:
    void finishedProcess();//add signals if process is finished
    void processErrored();
    void processSuccess();
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
    QString m_projectName;
    QString m_pullFetchFile;
    QString m_username;
    QString m_password;
};
#endif
