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

  connect(branchView, SIGNAL(renewObject()), this, SLOT(onBranchViewUpdate()));

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

  setMinimumSize(QSize(800,400));
  //setWindowFlags(Qt::WindowMinMaxButtonsHint);
  setLayout(mainLayout);
}
void GProjectDialog::onBranchViewUpdate() {
  delete m_projectTab->widget(2);
  GBranchView* branchView = new GBranchView(this, m_repos);
  m_projectTab->insertTab(2, branchView, tr("Branch"));
  connect(branchView, SIGNAL(renewObject()), this, SLOT(onBranchViewUpdate()));
  m_projectTab->setCurrentIndex(2);
}

GProjectDialog::~GProjectDialog() {
}


