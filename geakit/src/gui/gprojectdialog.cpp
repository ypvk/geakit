#include <QTabWidget>
#include <QPushButton>
#include <QFrame>
#include <QtGui>

#include "gprojectdialog.h"
#include "gcodeview.h"
#include "gcommitview.h"
#include "gbranchview.h"

GProjectDialog::GProjectDialog(QWidget* parent, git_repository* m_repos) : QDialog(parent)
{
  this->m_repos = m_repos;
  m_projectTab = new QTabWidget(this);
  GCommitView* commitView = new GCommitView(this, m_repos);
  GCodeView* codeView = new GCodeView(this, m_repos); 
  GBranchView* branchView = new GBranchView(this, m_repos);

  m_projectTab->addTab(codeView, tr("Code"));
  m_projectTab->addTab(commitView,tr("Commit"));
  m_projectTab->addTab(branchView, tr("Branch"));

  m_projectTab->setTabPosition(QTabWidget::West);
  
  m_settingButton = new QPushButton(tr("Settings"));
  m_mergeButton = new QPushButton(tr("Merge"));
  
  QFrame* buttonFrame = new QFrame(this);
  QHBoxLayout* buttonLayout = new QHBoxLayout(this);
  buttonLayout->addStretch(1);
  buttonLayout->addWidget(m_mergeButton);
  buttonLayout->addWidget(m_settingButton);
  
  buttonFrame->setLayout(buttonLayout);
  
  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(buttonFrame);
  mainLayout->addWidget(m_projectTab);

  setLayout(mainLayout);
}
  


