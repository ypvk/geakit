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
  m_localBranchArea = new QGroupBox(tr("local"), this);
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
  m_mainLayout->addWidget(m_localBranchArea);
  mainWidget->setLayout(m_mainLayout);
 
  //connect the button signal
  connect(m_pushButton, SIGNAL(clicked()), this, SLOT(onPushButtonClicked()));
  connect(m_fetchButton, SIGNAL(clicked()), this, SLOT(onFetchButtonClicked()));
  connect(m_newBranchButton, SIGNAL(clicked()), this, SLOT(onNewBranchButtonClicked()));
  connect(m_newRemoteButton, SIGNAL(clicked()), this, SLOT(onNewRemoteButtonClicked()));

  updateView();

  ////show all the reference branch
  //m_branchList = m_command->gitRefs();

  //setupLocalBranchsArea();

  //m_remoteList = m_command->gitRemoteNames();
  //m_remoteNames->addItems(m_remoteList);

////setup remoteBranchs Area
  //setupRemoteBranchsArea();

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
void GBranchView::onChangeButtonClicked(int id) {
  git_reference* newHead;
  int error = git_reference_create_symbolic(&newHead, m_repo, "HEAD", m_branchList[id].toLocal8Bit().constData(), 1);
  if (error < GIT_SUCCESS) {
    qDebug() << "error change the branch";
  }  
  git_reference_free(newHead);
  emit branchChanged();
}
void GBranchView::onMergeButtonClicked(int id) {
  QString branchName = m_branchList[id].split("/").last();

  QString cmd = "git merge " + branchName;

//  qDebug() << cmd;
  m_command->execute(cmd);
 // qDebug() << m_command->output();
}
void GBranchView::onRemoteButtonClicked(int id) {
  QString remoteName = m_branchList[id];
  qDebug() << remoteName;

  QRegExp rx("refs/remotes/(.+)");
  int pos = remoteName.indexOf(rx);
  if (pos >= 0){
//    qDebug() << rx.cap(1);
    QString cmd = "git merge " + rx.cap(1);
    m_command->execute(cmd);
  }
}
void GBranchView::onPushButtonClicked() {
  //get the selected remote
  if (m_password == "") {
    QMessageBox::warning(this, tr("warning"), tr("please set the password in the setting dialog"));
    return;
  }
  QString remoteName = m_remoteNames->currentText();
  QString remoteUrl = getRemoteUrl(remoteName);
  QString cmd = QString("git push %1").arg(remoteUrl);
  m_command->execute(cmd);
}

void GBranchView::onFetchButtonClicked() {
   //get the selected remote
  if (m_password == "") {
    QMessageBox::warning(this, tr("warning"), tr("please set the password in the setting dialog"));
    return;
  }
  QString remoteName = m_remoteNames->currentText();
  QString remoteUrl = getRemoteUrl(remoteName);
  QString cmd = QString("git fetch %1").arg(remoteUrl);
  m_command->execute(cmd);
}
QString GBranchView::getRemoteUrl(const QString& remoteName) {
  git_remote* m_remote;
  int error = git_remote_load(&m_remote, m_repo, remoteName.toLocal8Bit().constData());
  QString remoteUrl = QString(git_remote_url(m_remote));
  QStringList strList = remoteUrl.split("@");
  QString realUrl = strList[0] + ":" + m_password + "@" + strList[1];
  qDebug() << realUrl;
  git_remote_free(m_remote);
  return realUrl;
}
void GBranchView::setPassword(const QString& password) {
  m_password = password;
}
void GBranchView::onNewBranchButtonClicked() {
  GBranchNameDialog dlg;
  if (QDialog::Accepted == dlg.exec()) {
    QString branchName = dlg.getBranchName();
    bool isNameExit = false;
    //check if the branchName has existed
    QList<QString>::const_iterator it = m_branchList.constBegin();
    while (it != m_branchList.constEnd()) {
      if ((*it).indexOf(branchName) >= 0) {
        QMessageBox::warning(this, tr("warning"), tr("Branch Name Exits"));
        isNameExit = true;
        break;
      }
      ++it;
    }
    if (!isNameExit) {
      m_command->createBranch(branchName);
    }
  }
}
/**************delete one new branch**********/
void GBranchView::onRmBranchButtonClicked(int id) {
  git_reference* branch;
  int error = git_reference_lookup(&branch, m_repo, m_branchList[id].toLocal8Bit().constData());
  if (error < GIT_SUCCESS) {
    qDebug() << "Can't get the branch"; 
    return;
  }
  error = git_reference_delete(branch);
  if (error < GIT_SUCCESS) {
    qDebug() << "can't delete";
    return;
  }
}
/*************add new remote function***************/
void GBranchView::onNewRemoteButtonClicked() {
  GRemoteNameDialog dlg;
  if (QDialog::Accepted == dlg.exec()) {
    QString remoteName = dlg.remoteName();
    QString url = dlg.remoteUrl();
    bool isNameExit = false;
    QList<QString>::const_iterator it = m_remoteList.constBegin();
    while ( it != m_remoteList.constEnd()) {
      if ((*it) == remoteName) {
        QMessageBox::warning(this, tr("warning"), tr("Branch Name Exits"));
        isNameExit = true;
        break;
      }
      ++it;
    }
    if (!isNameExit) {
      m_command->createRemote(remoteName, url);
    }
  }
}
void GBranchView::setupLocalBranchesArea()
{
  //free the widgets in local area
  if (m_localBranchArea != NULL) {
    int index = m_mainLayout->indexOf(m_localBranchArea);
    if (index >= 0) {
      QLayoutItem* item = m_mainLayout->takeAt(index);
      delete item;
      m_localBranchArea = new QGroupBox(tr("local"), this);
      qDebug() << "renew local area";
    }
  }
  //get the reference head name
  QString headName = m_command->gitRefHead();
  QVBoxLayout* localLayout = new QVBoxLayout(m_localBranchArea);
//first build the local branch
  for (int i = 0; i < m_branchList.size(); i++) {
    if (m_branchList[i] == headName) {
      QLabel* label = new QLabel("*" + headName, m_localBranchArea);
      localLayout->addWidget(label);
      }
    else {
      QHBoxLayout* tmpLayout = new QHBoxLayout;
      QLabel* label = new QLabel(m_branchList[i], m_localBranchArea);
      GButton* changeButton = new GButton(m_localBranchArea, i);
      changeButton->setText(tr("Change Here"));
      GButton* mergeButton = new GButton(m_localBranchArea, i);
      mergeButton->setText(tr("Merge to main"));
      GButton* rmBranchButton = new GButton(m_localBranchArea, i);
      rmBranchButton->setText(tr("Delete"));

      tmpLayout->addWidget(label);
      tmpLayout->addStretch(1);
      tmpLayout->addWidget(changeButton);
      tmpLayout->addWidget(mergeButton);
      tmpLayout->addWidget(rmBranchButton);

      localLayout->addLayout(tmpLayout);
      connect(changeButton, SIGNAL(clicked()), changeButton, SLOT(showId()));
      connect(changeButton, SIGNAL(buttonId(int)), this, SLOT(onChangeButtonClicked(int)));
      connect(rmBranchButton, SIGNAL(clicked()), rmBranchButton, SLOT(showId()));
      connect(rmBranchButton, SIGNAL(buttonId(int)), this, SLOT(onRmBranchButtonClicked(int)));

      connect(mergeButton, SIGNAL(clicked()), mergeButton, SLOT(showId()));
      connect(mergeButton, SIGNAL(buttonId(int)), this, SLOT(onMergeButtonClicked(int)));
    } 
  }
}
void GBranchView::setupRemoteBranchesArea()
{
  freeRemoteArea(); 
  QStringList::const_iterator it = m_remoteList.constBegin();
  for (; it != m_remoteList.constEnd(); it++)
  {
    QGroupBox* groupBox = new QGroupBox(*it, this);
    m_remoteAreaList << groupBox;
    m_mainLayout->addWidget(groupBox);
    
    QVBoxLayout* groupLayout = new QVBoxLayout(groupBox);

    QStringList remoteBranches = m_remoteBranchesHash.value(*it);
    for(int i = 0; i < remoteBranches.size(); i++) {
        QHBoxLayout* tmpLayout = new QHBoxLayout;
        QLabel* label = new QLabel(remoteBranches[i], groupBox);
        GButton* mergeButton = new GButton(groupBox, i);
        mergeButton->setText("Merge to Main");
        tmpLayout->addWidget(label);
        tmpLayout->addStretch(1);
        tmpLayout->addWidget(mergeButton);
        connect(mergeButton, SIGNAL(clicked()), mergeButton, SLOT(showId()));
        connect(mergeButton, SIGNAL(buttonId(int)), this, SLOT(onRemoteButtonClicked(int)));
        groupLayout->addLayout(tmpLayout);
      }
    }
}
void GBranchView::freeRemoteArea()
{
  if (0 == m_remoteAreaList.size()) return;
  QList<QGroupBox*>::const_iterator it = m_remoteAreaList.constBegin();
  for(; it != m_remoteAreaList.constEnd(); it++) {
    int index = m_mainLayout->indexOf(*it);
    if (index >= 0) {
      QLayoutItem* item = m_mainLayout->takeAt(index);
      delete item;
    }
  }
  m_remoteAreaList.clear();
}
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
