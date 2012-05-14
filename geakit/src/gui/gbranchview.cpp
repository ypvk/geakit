#include "gbranchview.h"
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QDebug>
#include <QProcess>
#include <QRegExp>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <iostream>
#include "gitcommand.h"
#include "gbutton.h"
#include <iostream>

GBranchView::GBranchView(QWidget* parent, git_repository* repo) : QWidget(parent) 
{
  m_repo = repo;
  m_mainArea = new QScrollArea(this);
  m_localBranchArea = new QGroupBox(tr("local"), this);
  QWidget* mainWidget = new QWidget(this);
  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(m_localBranchArea);
  mainWidget->setLayout(mainLayout);

  //show all the reference branch
  git_strarray strarray;
  int error;
  error = git_reference_listall(&strarray, m_repo, GIT_REF_LISTALL);
  if (error < GIT_SUCCESS) {
    qDebug() << "error list";
  }
 // QStringList branchList;
  for (int i = 0; i < strarray.count; i++ ) {
    branchList << strarray.strings[i];
  }
  //get the head refernce
  git_reference* head;
  error = git_repository_head(&head, m_repo);
  QString headName(git_reference_name(head));
  git_reference_free(head);

  QVBoxLayout* localLayout = new QVBoxLayout(m_localBranchArea);
//first build the local branch
  for (int i = 0; i < branchList.size(); i++) {
    if (branchList[i] == headName) {
      QLabel* label = new QLabel("*" + headName, this);
      localLayout->addWidget(label);
      }
    int pos = branchList[i].indexOf("refs/heads/");
    if (pos >= 0 && branchList[i] != headName) {
      QHBoxLayout* tmpLayout = new QHBoxLayout;
      QLabel* label = new QLabel(branchList[i], this);
      GButton* changeButton = new GButton(this, i);
      changeButton->setText(tr("Change Here"));
      GButton* mergeButton = new GButton(this, i);
      mergeButton->setText(tr("Merge to main"));
      tmpLayout->addWidget(label);
     // tmpLayout->addStretch(2);
     // tmpLayout->addSt(20);
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
  QStringList::const_iterator it = remoteNames.constBegin();
  for (; it != remoteNames.constEnd(); it++)
  {
    QGroupBox* groupBox = new QGroupBox(*it, this);
    m_remotAreaList << groupBox;
    mainLayout->addWidget(groupBox);
    
    QVBoxLayout* groupLayout = new QVBoxLayout(groupBox);

    for (int i = 0; i < branchList.size(); i++) {
      if (branchList[i].indexOf(*it) >= 0) {
        QHBoxLayout* tmpLayout = new QHBoxLayout;
        QLabel* label = new QLabel(branchList[i], this);
        GButton* mergeButton = new GButton(this, i);
        mergeButton->setText("Merge to Main");
        tmpLayout->addWidget(label);
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

  m_mainArea->setWidget(mainWidget);
  
  QVBoxLayout* finalLayout = new QVBoxLayout(this);
  finalLayout->addWidget(m_mainArea);
  setLayout(finalLayout);

}

GBranchView::~GBranchView() {
}
void GBranchView::onChangeButtonClicked(int id) {
  QString branchName = branchList[id].split("/").last();

  QString cmd = "git checkout " + branchName;
//  qDebug() <<cmd;
  m_command->execute(cmd);
  //qDebug() << m_command->output();
  emit renewObject();
}
void GBranchView::onMergeButtonClicked(int id) {
  QString branchName = branchList[id].split("/").last();

  QString cmd = "git merge " + branchName;

//  qDebug() << cmd;
  m_command->execute(cmd);
 // qDebug() << m_command->output();
}
void GBranchView::onRemoteButtonClicked(int id) {
  QString remoteName = branchList[id];
  qDebug() << remoteName;

  QRegExp rx("refs/remotes/(.+)");
  int pos = remoteName.indexOf(rx);
  if (pos >= 0){
//    qDebug() << rx.cap(1);
    QString cmd = "git merge " + rx.cap(1);
    m_command->execute(cmd);
  }
}

