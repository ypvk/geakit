#include "gbranchview.h"
#include <QScrollArea>
#include <QMessageBox>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
#include <QGroupBox>
#include <QDebug>
#include <QProcess>
#include <QRegExp>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <iostream>
#include "data/account.h"
#include "gitcommand.h"
#include "gbranchnamedialog.h"
#include "gremotenamedialog.h"
#include "gbutton.h"

GBranchView::GBranchView(QWidget* parent, git_repository* repo) : QWidget(parent) 
{
  m_repo = repo;
  m_mainArea = new QScrollArea(this);
  m_pushButton = new QPushButton(tr("Push"), this);
  m_fetchButton = new QPushButton(tr("Fetch"), this);
  m_syncButton = new QPushButton(tr("Sync"), this);
  m_remoteNames = new QComboBox(this);
  m_newBranchButton = new QPushButton(tr("NewBranch"), this);
  m_newRemoteButton = new QPushButton(tr("NewRemote"), this);

  QString path(git_repository_workdir(m_repo));  
  m_command = new GitCommand(this, path);
  m_command->setRepository(m_repo);

  QWidget* mainWidget = new QWidget(this);
  m_mainLayout = new QVBoxLayout(this);

  //mainWidget->setStyleSheet("QWidget {background:white}");
  
  mainWidget->setObjectName(QString("branch-widget"));
  mainWidget->setStyleSheet("QWidget #branch-widget {background:white}");
  QHBoxLayout* m_actionButtonLayout = new QHBoxLayout;
  m_actionButtonLayout->addWidget(m_newBranchButton);
  m_actionButtonLayout->addWidget(m_newRemoteButton);
  m_actionButtonLayout->addStretch(1);
  m_actionButtonLayout->addWidget(m_remoteNames);
  m_actionButtonLayout->addWidget(m_pushButton);
  m_actionButtonLayout->addWidget(m_fetchButton);
  m_actionButtonLayout->addWidget(m_syncButton);

  m_mainLayout->addLayout(m_actionButtonLayout);
  mainWidget->setLayout(m_mainLayout);

  connect(m_command, SIGNAL(finishedProcess()), this, SLOT(onProcessFinished()));
 
  //connect the button signal
  connect(m_pushButton, SIGNAL(clicked()), this, SLOT(onPushButtonClicked()));
  connect(m_fetchButton, SIGNAL(clicked()), this, SLOT(onFetchButtonClicked()));
  connect(m_newBranchButton, SIGNAL(clicked()), this, SLOT(onNewBranchButtonClicked()));
  connect(m_newRemoteButton, SIGNAL(clicked()), this, SLOT(onNewRemoteButtonClicked()));
  connect(m_syncButton, SIGNAL(clicked()), this, SLOT(onSyncButtonClicked()));

  updateView();

  m_mainLayout->addStretch(1);
  m_mainArea->setWidget(mainWidget);
  
  m_mainArea->setWidgetResizable(true);
  //m_mainArea->setStyleSheet("QScrollArea {background:red;color:white;}");

  QVBoxLayout* finalLayout = new QVBoxLayout(this);
  finalLayout->addWidget(m_mainArea);
  setLayout(finalLayout);

}

GBranchView::~GBranchView() {
}
void GBranchView::onChangeButtonClicked(const QString& branchName) {
  //test if index doesn't commit
  int diffNum = m_command->gitDiffIndexToTree();
  int diffNum1 = m_command->gitDiffWorkDirToIndex();
  if (diffNum > 0 || diffNum1 > 0) {
    QString message;
    if (diffNum > 0 && diffNum1 == 0) {
      message = tr("add not commit:\n");
      message += m_command->diffFileInfosTree;
    }
    else if (diffNum1 > 0 && diffNum == 0) {
      message = tr("modified not add:\n");
      message += m_command->diffFileInfosIndex;
    }
    else {
      message = tr("add not commit:\n");
      message += m_command->diffFileInfosTree;
      message += tr("modified not add:\n");
      message += m_command->diffFileInfosIndex;
    }

    QMessageBox::information(this, tr("warning"), tr("changes not commit\n") + message + tr("please commit before change branch"));
    return;
  }
  bool result = m_command->gitChangeBranch(branchName);
  if (result) emit branchChanged();
  else {
    qDebug() << "error change the branch";
    return;
  }
}
void GBranchView::onMergeButtonClicked(const QString& branchName) {
  int diffNum = m_command->gitDiffIndexToTree();
  int diffNum1 = m_command->gitDiffWorkDirToIndex();
  if (diffNum > 0 || diffNum1 > 0) {
    QString message;
    if (diffNum > 0 && diffNum1 == 0) {
      message = tr("add not commit:\n");
      message += m_command->diffFileInfosTree;
    }
    else if (diffNum1 > 0 && diffNum == 0) {
      message = tr("modified not add:\n");
      message += m_command->diffFileInfosIndex;
    }
    else {
      message = tr("add not commit:\n");
      message += m_command->diffFileInfosTree;
      message += tr("modified not add:\n");
      message += m_command->diffFileInfosIndex;
    }

    QMessageBox::information(this, tr("warning"), tr("changes not commit\n") + message + tr("please commit before change branch"));
    return;
  }
  if (m_command->gitMergeBranch(branchName)) {
    qDebug() << "success merge " << branchName;
    emit contentsChanged();
    return;
  }
  else {
    QString message = m_command->diffFileInfosIndex;
    QMessageBox::information(this, tr("warning"), tr("merge conflict\n") + message + tr("reset or fix my yourself"));
    emit contentsChanged();
  }
}

void GBranchView::onPushButtonClicked() {
  //get the selected remote
  if (m_account->password() == "" || m_account->username() == "") {
    QMessageBox::warning(this, tr("warning"), tr("please set the password or username in the setting dialog"));
    return;
  }
  QString remoteName = m_remoteNames->currentText();
  QString remoteUrl = m_command->gitRemoteUrl(remoteName);
  m_command->setPassword(m_account->password());
  m_command->setUsername(m_account->username());
  this->setEnabled(false);
  bool result = m_command->gitPush(remoteUrl);
  return;
}

void GBranchView::onFetchButtonClicked() {
   //get the selected remote
  if (m_account->password() == "" || m_account->username() == "") {
    QMessageBox::warning(this, tr("warning"), tr("please set the password or in the setting dialog"));
    return;
  }
  QString remoteName = m_remoteNames->currentText();
  QString remoteUrl = m_command->gitRemoteUrl(remoteName);
  m_command->setPassword(m_account->password());
  m_command->setUsername(m_account->username());
  this->setEnabled(false);
  bool result = m_command->gitFetch(remoteUrl);
  return;
}
//QString GBranchView::getRemoteUrl(const QString& remoteName) {
  ////git_remote* m_remote;
  ////int error = git_remote_load(&m_remote, m_repo, remoteName.toLocal8Bit().constData());
  ////QString remoteUrl = QString(git_remote_url(m_remote));
  //QString remoteUrl = m_command->gitRemoteUrl(remoteName);
  //QStringList strList = remoteUrl.split("@");
  //QString realUrl = strList[0] + ":" + m_password + "@" + strList[1];
  //qDebug() << realUrl;
  ////git_remote_free(m_remote);
  //return realUrl;
//}
//void GBranchView::setPassword(const QString& password) {
  //m_password = password;
//}
//void GBranchView::setUsername(const QString& username) {
  //m_username = username;
//}
void GBranchView::setAccount(GAccount* account)
{
  m_account = account;
}
void GBranchView::onNewBranchButtonClicked() {
  GBranchNameDialog dlg;
  if (QDialog::Accepted == dlg.exec()) {
    QString branchName = dlg.getBranchName();
    //check if the branchName has existed
    if (m_branchList.contains(branchName)){
      QMessageBox::warning(this, tr("warning"), tr("Branch Name Exits"));
      return;
    }
    else {
      m_command->createBranch(branchName);
      emit branchChanged();
    }
  }
}
/**************delete one new branch**********/
void GBranchView::onRmBranchButtonClicked(const QString& branchName) {
  bool result = m_command->gitDeleteBranch(branchName);
  if (result) qDebug() << "success remove " << branchName;
  else qDebug() << "error remove " << branchName;
}
/*************add new remote function***************/
void GBranchView::onNewRemoteButtonClicked() {
  GRemoteNameDialog dlg;
  if (QDialog::Accepted == dlg.exec()) {
    QString remoteName = dlg.remoteName();
    QString url = dlg.remoteUrl();
    if (m_remoteList.contains(remoteName))
        QMessageBox::warning(this, tr("warning"), tr("Branch Name Exits"));
    else {
      m_command->createRemote(remoteName, url);
      emit branchChanged();
    }
  }
}
void GBranchView::setupLocalBranchesArea()
{
  QString headName = m_command->gitRefHead();
  QGroupBox* localBranchArea = new QGroupBox(tr("local branch"), this);
  m_mainLayout->addWidget(localBranchArea);
  QVBoxLayout* localLayout = new QVBoxLayout(localBranchArea);
  for (int i = 0; i < m_branchList.size(); i++) {
    if (m_branchList[i] == headName) {
      QLabel* label = new QLabel("*" + headName, localBranchArea);
      localLayout->addWidget(label);
      }
    else {
      QHBoxLayout* tmpLayout = new QHBoxLayout;
      QLabel* label = new QLabel(m_branchList[i], localBranchArea);
      GButton* changeButton = new GButton(localBranchArea, m_branchList[i]);
      changeButton->setText(tr("Change Here"));
      GButton* mergeButton = new GButton(localBranchArea, m_branchList[i]);
      mergeButton->setText(tr("Merge to main"));
      GButton* rmBranchButton = new GButton(localBranchArea, m_branchList[i]);
      rmBranchButton->setText(tr("Delete"));

      tmpLayout->addWidget(label);
      tmpLayout->addStretch(1);
      tmpLayout->addWidget(changeButton);
      tmpLayout->addWidget(mergeButton);
      tmpLayout->addWidget(rmBranchButton);

      localLayout->addLayout(tmpLayout);
      connect(changeButton, SIGNAL(mButtonClicked(QString)), this, SLOT(onChangeButtonClicked(QString)));
      connect(rmBranchButton, SIGNAL(mButtonClicked(QString)), this, SLOT(onRmBranchButtonClicked(QString)));
      connect(mergeButton, SIGNAL(mButtonClicked(QString)), this, SLOT(onMergeButtonClicked(QString)));
    } 
  }
//  m_localBranchArea->setLayout(localLayout);
}
void GBranchView::setupRemoteBranchesArea()
{
  //freeRemoteArea(); 
  QStringList::const_iterator it = m_remoteList.constBegin();
  for (; it != m_remoteList.constEnd(); it++)
  {
    QGroupBox* groupBox = new QGroupBox(*it, this);
    //m_remoteAreaList << groupBox;
    m_mainLayout->addWidget(groupBox);
    
    QVBoxLayout* groupLayout = new QVBoxLayout(groupBox);

    QStringList remoteBranches = m_remoteBranchesHash.value(*it);
    for(int i = 0; i < remoteBranches.size(); i++) {
        QHBoxLayout* tmpLayout = new QHBoxLayout;
        QLabel* label = new QLabel(remoteBranches[i], groupBox);
        GButton* mergeButton = new GButton(groupBox, remoteBranches[i]);
        mergeButton->setText("Merge to Main");
        tmpLayout->addWidget(label);
        tmpLayout->addStretch(1);
        tmpLayout->addWidget(mergeButton);
        connect(mergeButton, SIGNAL(mButtonClicked(QString)), this, SLOT(onMergeButtonClicked(QString)));
        groupLayout->addLayout(tmpLayout);
      }
    }
}
//void GBranchView::freeRemoteArea()
//{
  //if (0 == m_remoteAreaList.size()) return;
  //QList<QGroupBox*>::const_iterator it = m_remoteAreaList.constBegin();
  //for(; it != m_remoteAreaList.constEnd(); it++) {
    //int index = m_mainLayout->indexOf(*it);
    //if (index >= 0) {
      //QLayoutItem* item = m_mainLayout->takeAt(index);
      //delete item;
    //}
  //}
  //m_remoteAreaList.clear();
//}
void GBranchView::updateView()
{
  m_branchList = m_command->gitBranches();
  m_remoteList = m_command->gitRemoteNames();
  QStringList::const_iterator it = m_remoteList.constBegin();
  while(it != m_remoteList.constEnd()) {
    QStringList remoteBranches = m_command->gitRemoteBranches(*it);
    m_remoteBranchesHash.insert(*it, remoteBranches);
    it ++;
  }
  m_remoteNames->clear();
  m_remoteNames->addItems(m_remoteList);
  setupLocalBranchesArea();
  setupRemoteBranchesArea();
}
void GBranchView::onProcessFinished()
{
  this->setEnabled(true); 
}
void GBranchView::gitSynchronize(const QString& branch, const QString& remote)
{
  if (m_account->password() == "" || m_account->username() == "") {
    QMessageBox::warning(this, tr("warning"), tr("please set the password or in the setting dialog"));
    return;
  }
  QString remoteUrl = m_command->gitRemoteUrl(remote);
  m_command->setPassword(m_account->password());
  m_command->setUsername(m_account->username());
  this->setEnabled(false);
  bool result = m_command->gitFetch(remoteUrl);
  QString remoteBranch = QString("%1/%2").arg(remote).arg(branch);
  result = m_command->gitMergeBranch(remoteBranch);
  result = m_command->gitPush(remoteUrl);
}
void GBranchView::onSyncButtonClicked()
{
  QString remoteName = m_remoteNames->currentText();
  QString branch = m_command->gitRefHead();
  this->gitSynchronize(branch, remoteName);
}
