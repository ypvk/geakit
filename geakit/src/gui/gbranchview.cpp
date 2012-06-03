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
#include "gbutton.h"
#include <iostream>

GBranchView::GBranchView(QWidget* parent, git_repository* repo) : QWidget(parent) 
{
  m_repo = repo;
  m_mainArea = new QScrollArea(this);
  m_localBranchArea = new QGroupBox(tr("local"), this);
 // m_localBranchArea->setStyleSheet("QGroupBox {background:white;}");
  m_pushButton = new QPushButton(tr("Push"), this);
  m_fetchButton = new QPushButton(tr("Fetch"), this);
  m_remoteNames = new QComboBox(this);
  m_newBranchButton = new QPushButton(tr("NewBranch"), this);
  m_password = "";
  QWidget* mainWidget = new QWidget(this);
  QVBoxLayout* mainLayout = new QVBoxLayout(this);

  //mainWidget->setStyleSheet("QWidget {background:white}");
  
  mainWidget->setObjectName(QString("branch-widget"));
  mainWidget->setStyleSheet("QWidget #branch-widget {background:white}");
  QHBoxLayout* m_actionButtonLayout = new QHBoxLayout;
  m_actionButtonLayout->addWidget(m_newBranchButton);
  m_actionButtonLayout->addStretch(1);
  m_actionButtonLayout->addWidget(m_remoteNames);
  m_actionButtonLayout->addWidget(m_pushButton);
  m_actionButtonLayout->addWidget(m_fetchButton);

  mainLayout->addLayout(m_actionButtonLayout);
  mainLayout->addWidget(m_localBranchArea);
  mainWidget->setLayout(mainLayout);
 
  //connect the button signal
  connect(m_pushButton, SIGNAL(clicked()), this, SLOT(onPushButtonClicked()));
  connect(m_fetchButton, SIGNAL(clicked()), this, SLOT(onFetchButtonClicked()));
  connect(m_newBranchButton, SIGNAL(clicked()), this, SLOT(onNewBranchButtonClicked()));
  //show all the reference branch
  git_strarray strarray;
  int error;
  error = git_reference_listall(&strarray, m_repo, GIT_REF_LISTALL);
  if (error < GIT_SUCCESS) {
    qDebug() << "error list";
  }
 // QStringList m_branchList;
  for (int i = 0; i < strarray.count; i++ ) {
    m_branchList << strarray.strings[i];
  }
  //get the head refernce
  git_reference* head;
  error = git_repository_head(&head, m_repo);
  QString headName(git_reference_name(head));
  git_reference_free(head);

  QVBoxLayout* localLayout = new QVBoxLayout(m_localBranchArea);
//first build the local branch
  for (int i = 0; i < m_branchList.size(); i++) {
    if (m_branchList[i] == headName) {
      QLabel* label = new QLabel("*" + headName, this);
      localLayout->addWidget(label);
      }
    //other branchs
    int pos = m_branchList[i].indexOf("refs/heads/");
    if (pos >= 0 && m_branchList[i] != headName) {
      QHBoxLayout* tmpLayout = new QHBoxLayout;
      QLabel* label = new QLabel(m_branchList[i], this);
      GButton* changeButton = new GButton(this, i);
      changeButton->setText(tr("Change Here"));
      GButton* mergeButton = new GButton(this, i);
      mergeButton->setText(tr("Merge to main"));
      tmpLayout->addWidget(label);
     // tmpLayout->addStretch(2);
     // tmpLayout->addSt(20);
      tmpLayout->addStretch(1);
      tmpLayout->addWidget(changeButton);
      tmpLayout->addWidget(mergeButton);
      localLayout->addLayout(tmpLayout);
      connect(changeButton, SIGNAL(clicked()), changeButton, SLOT(showId()));
      connect(changeButton, SIGNAL(buttonId(int)), this, SLOT(onChangeButtonClicked(int)));

      connect(mergeButton, SIGNAL(clicked()), mergeButton, SLOT(showId()));
      connect(mergeButton, SIGNAL(buttonId(int)), this, SLOT(onMergeButtonClicked(int)));
    } 
  }
  m_localBranchArea->setLayout(localLayout);
  git_strarray_free(&strarray);

  QString path(git_repository_workdir(m_repo));  

  m_command = new GitCommand(this, path);

  QString cmd = "git remote";
  m_command->execute(cmd);
  if ("" != m_command->output()) {
  QStringList remoteNames =  (m_command->output()).trimmed().split("\n");
  m_remoteList = remoteNames;
  m_remoteNames->addItems(m_remoteList);

  QStringList::const_iterator it = remoteNames.constBegin();
  for (; it != remoteNames.constEnd(); it++)
  {
    QGroupBox* groupBox = new QGroupBox(*it, this);
    m_remotAreaList << groupBox;
    mainLayout->addWidget(groupBox);
    
    QVBoxLayout* groupLayout = new QVBoxLayout(groupBox);

    for (int i = 0; i < m_branchList.size(); i++) {
      if (m_branchList[i].indexOf(*it) >= 0 && m_branchList[i].indexOf("HEAD") < 0) {
        QHBoxLayout* tmpLayout = new QHBoxLayout;
        QLabel* label = new QLabel(m_branchList[i], this);
        GButton* mergeButton = new GButton(this, i);
        mergeButton->setText("Merge to Main");
        tmpLayout->addWidget(label);
        tmpLayout->addStretch(1);
        tmpLayout->addWidget(mergeButton);
        connect(mergeButton, SIGNAL(clicked()), mergeButton, SLOT(showId()));
        connect(mergeButton, SIGNAL(buttonId(int)), this, SLOT(onRemoteButtonClicked(int)));
        groupLayout->addLayout(tmpLayout);
      }
    }
    groupBox->setLayout(groupLayout);

    /****************git remote cannot connect to the net(realize latter)**************************
    git_remote* m_remote;
    int error;
    error = git_remote_load(&m_remote, m_repo, (*it).toLocal8Bit().data());
    if (error < GIT_SUCCESS) {
      qDebug() << "error load";
      break;
    }
    error = git_remote_connect(m_remote, GIT_DIR_FETCH);
    if (error < GIT_SUCCESS) {
      qDebug() << "error connect";
    }
    error = git_remote_ls(m_remote, show_ref_cb, NULL);
    if (error < GIT_SUCCESS) {
      qDebug() <<"error" ;
    }
    git_remote_free(m_remote);
    ******************************************************************************/
  }
  }

  mainLayout->addStretch(1);
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
  /*******************relize it using QProcess********************
  QString branchName = m_branchList[id].split("/").last();

  QString cmd = "git checkout " + branchName;
//  qDebug() <<cmd;
  m_command->execute(cmd);
  //qDebug() << m_command->output();
  ************************end*********************************/
  /**************use libgit2 to relize it,(rewrite the HEAD symbolic********************/
  git_reference* newHead;
  int error = git_reference_create_symbolic(&newHead, m_repo, "HEAD", m_branchList[id].toLocal8Bit().constData(), 1);
  if (error < GIT_SUCCESS) {
    qDebug() << "error change the branch";
  }  
  emit renewObject();
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
      m_command->setRepository(m_repo);
      m_command->createBranch(branchName);
    }
  }
  emit renewObject();
}

