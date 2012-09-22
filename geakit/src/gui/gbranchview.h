#ifndef GBRANCHVIEW_H
#define GBRANCHVIEW_H

#include <QWidget>
#include <git2.h>
#include <QProcess>
#include <QList>
#include <QHash>
class QScrollArea;
class QGroupBox;
class QPushButton;
class GitCommand;
class QComboBox;
class QVBoxLayout;

class GBranchView : public QWidget
{
  Q_OBJECT
  public:
    explicit GBranchView(QWidget* parent = 0, git_repository* repo = 0);
    void setPassword(const QString& password);
    void setUsername(const QString& username);
    ~GBranchView();
  signals:
    void branchChanged();
  public slots:
    void updateView();
  private slots:
    void onChangeButtonClicked(const QString& branchName);
    void onMergeButtonClicked(const QString& branchName);
    //void onRemoteButtonClicked(const QString& branchName);
    void onPushButtonClicked();
    void onFetchButtonClicked();
    void onNewBranchButtonClicked();
    void onNewRemoteButtonClicked();
    void onRmBranchButtonClicked(const QString& branchName);
    void onSyncButtonClicked();
    void onProcessFinished();
  private:
    QString getRemoteUrl(const QString& remoteName);
    void setupLocalBranchesArea();
    void setupRemoteBranchesArea();
    void gitSynchronize(const QString& branch, const QString& remote);
 //   void freeRemoteArea();
  private:
    git_repository* m_repo;
    QPushButton* m_pushButton;
    QPushButton* m_fetchButton;
    QPushButton* m_syncButton;
    QPushButton* m_newBranchButton;
    QPushButton* m_newRemoteButton;
//    QList<QGroupBox*> m_remoteAreaList; 
    QScrollArea* m_mainArea;
    QStringList m_branchList;
    QStringList m_remoteList;
    QHash<QString, QStringList> m_remoteBranchesHash;
    QString m_password;
    QString m_username;
    QComboBox* m_remoteNames;
    QVBoxLayout* m_mainLayout;

    GitCommand* m_command;
};
#endif
