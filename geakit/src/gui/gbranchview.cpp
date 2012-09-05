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
  m_remoteNames = new QComboBox(this);
  m_newBranchButton = new QPushButton(tr("NewBranch"), this);
  m_newRemoteButton = new QPushButton(tr("NewRemote"), this);

  QString path(git_repository_workdir(m_repo));  
  m_command = new GitCommand(this, path);
  m_command->setRepository(m_repo);

  m_password = "";
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

  m_mainLayout->addLayout(m_actionButtonLayout);
  mainWidget->setLayout(m_mainLayout);
 
  //connect the button signal
  connect(m_pushButton, SIGNAL(clicked()), this, SLOT(onPushButtonClicked()));
  connect(m_fetchButton, SIGNAL(clicked()), this, SLOT(onFetchButtonClicked()));
  connect(m_newBranchButton, SIGNAL(clicked()), this, SLOT(onNewBranchButtonClicked()));
  connect(m_newRemoteButton, SIGNAL(clicked()), this, SLOT(onNewRemoteButtonClicked()));

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
  //git_reference* newHead;
  //int error = git_reference_create_symbolic(&newHead, m_repo, "HEAD", m_branchList[id].toLocal8Bit().constData(), 1);
  //if (error < GIT_SUCCESS) {
    //qDebug() << "error change the branch";
  //}  
  //git_reference_free(newHead);
  qDebug() << branchName;
  bool result = m_command->gitChangeBranch(branchName);
  if (result) emit branchChanged();
  else {
    qDebug() << "error change the branch";
    return;
  }
}
void GBranchView::onMergeButtonClicked(const QString& branchName) {
  if (m_command->gitMergeBranch(branchName)) {
    qDebug() << "success merge " << branchName;
    return;
  }
  else {
    qDebug() << "error merge " << branchName;
    return;
  }
  //QString branchName = m_branchList[id].split("/").last();

  //QString cmd = "git merge " + branchName;

////  qDebug() << cmd;
  //m_command->execute(cmd);
 //// qDebug() << m_command->output();
}
//void GBranchView::onRemoteButtonClicked(const QString& branchName) {
  ////QString remoteName = m_branchList[id];
  ////qDebug() << remoteName;

  ////QRegExp rx("refs/remotes/(.+)");
  ////int pos = remoteName.indexOf(rx);
  ////if (pos >= 0){
//////    qDebug() << rx.cap(1);
    ////QString cmd = "git merge " + rx.cap(1);
    ////m_command->execute(cmd);
  ////}
//}
void GBranchView::onPushButtonClicked() {
  //get the selected remote
  if (m_password == "") {
    QMessageBox::warning(this, tr("warning"), tr("please set the password in the setting dialog"));
    return;
  }
  QString remoteName = m_remoteNames->currentText();
  QString remoteUrl = getRemoteUrl(remoteName);
  bool result = m_command->gitPush(remoteUrl);
  if (result) qDebug() << "success push";
  else qDebug() << "error push";
  return;
}

void GBranchView::onFetchButtonClicked() {
   //get the selected remote
  if (m_password == "") {
    QMessageBox::warning(this, tr("warning"), tr("please set the password in the setting dialog"));
    return;
  }
  QString remoteName = m_remoteNames->currentText();
  QString remoteUrl = getRemoteUrl(remoteName);
  //QString cmd = QString("git fetch %1").arg(remoteUrl);
  //m_command->execute(cmd);
  bool result = m_command->gitFetch(remoteUrl);
  if (result) qDebug() << "success fetch";
  else qDebug() << "error fetch";
  return;
}
QString GBranchView::getRemoteUrl(const QString& remoteName) {
  //git_remote* m_remote;
  //int error = git_remote_load(&m_remote, m_repo, remoteName.toLocal8Bit().constData());
  //QString remoteUrl = QString(git_remote_url(m_remote));
  QString remoteUrl = m_command->gitRemoteUrl(remoteName);
  QStringList strList = remoteUrl.split("@");
  QString realUrl = strList[0] + ":" + m_password + "@" + strList[1];
  qDebug() << realUrl;
  //git_remote_free(m_remote);
  return realUrl;
}
void GBranchView::setPassword(const QString& password) {
  m_password = password;
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
    }
  }
}
/**************delete one new branch**********/
void GBranchView::onRmBranchButtonClicked(const QString& branchName) {
  //git_reference* branch;
  //int error = git_reference_lookup(&branch, m_repo, m_branchList[id].toLocal8Bit().constData());
  //if (error < GIT_SUCCESS) {
    //qDebug() << "Can't get the branch"; 
    //return;
  //}
  //error = git_reference_delete(branch);
  //if (error < GIT_SUCCESS) {
    //qDebug() << "can't delete";
    //return;
  //}
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
