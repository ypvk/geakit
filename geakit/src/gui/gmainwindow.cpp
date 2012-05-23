#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QGroupBox>
#include <QDebug>
#include <QToolBar>
#include <QStackedWidget>
#include <QFileDialog>

#include "ui_gmainwindow.h"

#include "gprojectdialog.h"
#include "gmainwindow.h"
#include "gsettingsdialog.h"
#include "gprojectdialog.h"
#include "gcommitview.h"
#include "gbranchview.h"
#include "gcodeview.h"

#include "data/account.h"

GMainWindow::GMainWindow(QWidget* parent):QMainWindow(parent), ui(new Ui::GMainWindow)
{
  ui->setupUi(this);
  this->setWindowFlags(this->windowFlags()&Qt::WindowMaximizeButtonHint&Qt::WindowMinimizeButtonHint);
  m_currentRepo = NULL;
  setupActions();
  setupMenus();
  loadSettings();
  buildGui();
  setupToolBar();
  initProjectItems();
}

GMainWindow::~GMainWindow(){
  delete m_account;
  git_config_free(m_config);
  if (NULL != m_currentRepo) {
    git_repository_free(m_currentRepo);
  }
  delete m_codeViewWidget;
  delete m_branchViewWidget;
  delete m_commitViewWidget;
}

void GMainWindow::onSettingsActionTriggered()
{
  GSettingsDialog* dlg = new GSettingsDialog(m_account, this);
  if(dlg->exec())
  {
    m_account->copy(dlg->account());
  }
  delete dlg;
}
/*
void GMainWindow::onProjectActionTriggered()
{
  GProjectDialog* dlg = new GProjectDialog(this, 0);
  dlg->exec();
  delete dlg;
}
*/
void GMainWindow::setupMenus()
{
  m_editMenu = menuBar()->addMenu(tr("&Edit"));
  m_editMenu->addAction(m_settingsAction);
  //m_editMenu->addAction(m_projectAction);
}

void GMainWindow::setupActions()
{
  m_settingsAction = new QAction(tr("Settings"), this);
  m_settingsAction->setIcon(QIcon(":icons/action_settings.png"));
  m_settingsAction->setShortcut(Qt::CTRL + Qt::Key_Comma);
  m_settingsAction->setStatusTip(tr("Settings of Geakit Clinet"));
  connect(m_settingsAction, SIGNAL(triggered()), this, SLOT(onSettingsActionTriggered()));

  m_projectsAction = new QAction(tr("projects"), this);
  m_projectsAction->setIcon(QIcon(":/icons/project.png"));
  m_projectsAction->setStatusTip(tr("Show The Projects in Local And Remote"));
  connect(m_projectsAction, SIGNAL(triggered()), this, SLOT(onProjectsActionTriggered()));

  m_codeViewAction = new QAction(tr("codeViews"), this);
  m_codeViewAction->setIcon(QIcon(":/icons/codeView.png"));
  m_codeViewAction->setStatusTip(tr("files in the git repository"));
  connect(m_codeViewAction, SIGNAL(triggered()), this, SLOT(onCodeViewActionTriggered()));

  m_branchViewAction = new QAction(tr("branchViews"), this);
  m_branchViewAction->setIcon(QIcon(":/icons/branchView.png"));
  m_branchViewAction->setStatusTip(tr("Show The branch in the remote and local"));
  connect(m_branchViewAction, SIGNAL(triggered()), this, SLOT(onBranchViewActionTriggered()));

  m_commitViewAction = new QAction(tr("commitViews"), this);
  m_commitViewAction->setIcon(QIcon(":/icons/commitView.png"));
  m_commitViewAction->setStatusTip(tr("log show the commit history"));
  connect(m_commitViewAction, SIGNAL(triggered()), this, SLOT(onCommitViewActionTriggered()));
}

void GMainWindow::loadSettings(){
  QString username = m_settings.value("account/username", "").toString();
  QString password = m_settings.value("account/password", "").toString();
  int error;
  const char* cfullname;
  const char* cemail;
  error = git_config_open_global(&m_config);
  //TODO do something if error occured
  error = git_config_get_string(m_config, "user.name", &cfullname);
  error = git_config_get_string(m_config, "user.email", &cemail);
  //TODO do something if error occured
  QString fullname = QString(cfullname);
  QString email = QString(cemail);
  m_account = new GAccount(username, password, fullname, email);
}

void GMainWindow::saveSettings(){
  m_settings.setValue("account/username", m_account->username());
  m_settings.setValue("account/password", m_account->password());
  int error;
  error = git_config_set_string(m_config, "user.name", m_account->fullname().toLocal8Bit().constData());
  error = git_config_set_string(m_config, "user.email", m_account->email().toLocal8Bit().constData());
  //TODO do something if error occured
}

void GMainWindow::closeEvent(QCloseEvent *event)
{
  saveSettings();
  event->accept();
}
void GMainWindow::buildGui() {
  m_projectsOnline = new QListWidget(this);
  m_projectsLocal = new QListWidget(this);

  m_addButton = new QPushButton(this);
  m_rmButton = new QPushButton(this);

  m_addButton->setIcon(QIcon(tr(":/icons/right.png")));
  m_rmButton->setIcon(QIcon(tr(":/icons/left.png")));

  m_addButton->setIconSize(QSize(40, 40));
  m_rmButton->setIconSize(QSize(40, 40));
  m_addButton->setFixedSize(QSize(37,37));
  m_rmButton->setFixedSize(QSize(37, 37));

  QHBoxLayout* mainLayout = new QHBoxLayout;
  QVBoxLayout* buttonLayout = new QVBoxLayout;

  buttonLayout->addWidget(m_addButton);
  buttonLayout->addWidget(m_rmButton);

  QGroupBox* projectsOnlineBox = new QGroupBox(this);
  projectsOnlineBox->setTitle(tr("projectsOnLine"));
  QHBoxLayout* groupLayout = new QHBoxLayout;
  groupLayout->addWidget(m_projectsOnline);
  projectsOnlineBox->setLayout(groupLayout);

  QGroupBox* projectsLocalBox = new QGroupBox(this);
  projectsLocalBox->setTitle(tr("projectsLocal"));
  QHBoxLayout* groupLayout1 = new QHBoxLayout;
  groupLayout1->addWidget(m_projectsLocal);
  projectsLocalBox->setLayout(groupLayout1);

  mainLayout->addWidget(projectsOnlineBox);
  mainLayout->addLayout(buttonLayout);
  mainLayout->addWidget(projectsLocalBox);
  
  m_widgets = new QStackedWidget(this);
  m_projectsWidget = new QWidget(this);
  //this three widget is controlled by myself can be delete and new by myself;
  m_codeViewWidget = new QWidget;
  m_branchViewWidget = new QWidget;
  m_commitViewWidget = new QWidget;

  m_widgets->addWidget(m_projectsWidget);
  m_widgets->addWidget(m_codeViewWidget);
  m_widgets->addWidget(m_branchViewWidget);
  m_widgets->addWidget(m_commitViewWidget);

  m_widgets->setCurrentIndex(0);

  m_projectsWidget->setLayout(mainLayout);
  setCentralWidget(m_widgets);
  
  connect(m_addButton, SIGNAL(clicked()), this, SLOT(addProjectToLocal()));
  connect(m_rmButton, SIGNAL(clicked()), this, SLOT(removeProjectInLocal()));

  connect(m_projectsLocal, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(openProject(QListWidgetItem*)));

}
void GMainWindow::initProjectItems() {
  /***************just get some tests******************/
  QListWidgetItem* projectItem = new QListWidgetItem(m_projectsOnline);
  projectItem->setText(tr("new_git"));
  QListWidgetItem* projectItem1 = new QListWidgetItem(m_projectsOnline);
  projectItem1->setText(tr("git_myself"));
}
void GMainWindow::addProjectToLocal() {
  /*************test, projects' name get from the settings*****************/
  QList<QListWidgetItem* > selectedProjects = m_projectsOnline->selectedItems();
  QList<QListWidgetItem*>::iterator it = selectedProjects.begin();
  while (it != selectedProjects.end()) {
    //QListWidgetItem* item = m_projectsOnline->takeItem(m_projectsOnline->row(*it));
    QString projectName = (*it)->text();
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Project Dir Local"), tr("/home/yuping/yp/git"));
    QListWidgetItem* item = new QListWidgetItem(m_projectsLocal);
    item->setText(dirName + "/" + projectName);
    m_projectsLocal->addItem(item);
    it++;
  }
}
void GMainWindow::removeProjectInLocal() {
  QList<QListWidgetItem* > selectedProjects = m_projectsLocal->selectedItems();
  QList<QListWidgetItem* >::iterator it = selectedProjects.begin();
  while ( it != selectedProjects.end()) {
    QListWidgetItem* item = m_projectsLocal->takeItem(m_projectsLocal->row(*it));
    //m_projectsOnline->addItem(item);
    delete item;
    it++; 
  }
}


void GMainWindow::setupToolBar() {
  m_toolBar = new QToolBar(tr("mainToolBar"), this);
  m_toolBar->setIconSize(QSize(50,50));
  m_toolBar->setStyleSheet("QToolBar {background:#E6E6FA;}");
  this->addToolBar(Qt::LeftToolBarArea, m_toolBar);
 // m_toolBar->setAllowedAreas(Qt::LeftToolBarArea);
  m_toolBar->setMovable(false);

  m_toolBar->addAction(m_projectsAction);
  m_toolBar->addAction(m_codeViewAction);
  m_toolBar->addAction(m_commitViewAction);
  m_toolBar->addAction(m_branchViewAction);
}
void GMainWindow::onProjectsActionTriggered() {
  m_widgets->setCurrentIndex(0);
}
void GMainWindow::onCodeViewActionTriggered() {
  m_widgets->setCurrentIndex(1);
}
void GMainWindow::onBranchViewActionTriggered() {
  m_widgets->setCurrentIndex(3);
}
void GMainWindow::onCommitViewActionTriggered() {
  m_widgets->setCurrentIndex(2);
}
void GMainWindow::openProject(QListWidgetItem* project) {
  if (NULL != m_currentRepo) {
    git_repository_free(m_currentRepo);
  }
  int error = git_repository_open(&m_currentRepo, (project->text()).toLocal8Bit().constData());
  qDebug() << project->text();
  if (error < GIT_SUCCESS) {
    qDebug() << "error open the repos";
    m_currentRepo = NULL;
    return;
  }

  updateView();
  m_widgets->setCurrentIndex(1);
}
void GMainWindow::updateView() {
  
  //remove the origin widget
  //free the memory first
  if (NULL != m_commitViewWidget) {
    m_widgets->removeWidget(m_commitViewWidget);
    delete m_commitViewWidget;
  }
  if (NULL != m_codeViewWidget) {
    m_widgets->removeWidget(m_codeViewWidget);
    delete m_codeViewWidget;
  }
  if (NULL != m_branchViewWidget) {
    m_widgets->removeWidget(m_branchViewWidget);
    delete m_branchViewWidget;
  }
  m_codeViewWidget = new QWidget;
  m_branchViewWidget = new QWidget;
  m_commitViewWidget = new QWidget;

  GCommitView* m_commitView = new GCommitView(this, m_currentRepo);
  GCodeView* m_codeView = new GCodeView(this, m_currentRepo);
  GBranchView* m_branchView = new GBranchView(this, m_currentRepo);
//here we don't use the button, just show here, function maybe realize later
  /*
  QPushButton* m_button1 = new QPushButton(tr("Merge"), m_commitView);
  QPushButton* m_button2 = new QPushButton(tr("Settings"), m_commitView);

  QHBoxLayout* buttonLayout1 = new QHBoxLayout;
  QHBoxLayout* buttonLayout2 = new QHBoxLayout;
  QHBoxLayout* buttonLayout3 = new QHBoxLayout;
  buttonLayout1->addStretch(1);
  buttonLayout1->addWidget(m_button1);
  buttonLayout1->addWidget(m_button2);
  buttonLayout2->addStretch(1);
  buttonLayout2->addWidget(m_button1);
  buttonLayout2->addWidget(m_button2);
  buttonLayout3->addStretch(1);
  buttonLayout3->addWidget(m_button1);
  buttonLayout3->addWidget(m_button2);
*/
  QVBoxLayout* commitLayout = new QVBoxLayout;
  QVBoxLayout* codeLayout = new QVBoxLayout;
  QVBoxLayout* branchLayout = new QVBoxLayout;
  
  //codeLayout->addLayout(buttonLayout1);
  codeLayout->addWidget(m_codeView);
 // commitLayout->addLayout(buttonLayout2);
  commitLayout->addWidget(m_commitView);
 // branchLayout->addLayout(buttonLayout3);
  branchLayout->addWidget(m_branchView);

  m_codeViewWidget->setLayout(codeLayout);
  m_branchViewWidget->setLayout(branchLayout);
  m_commitViewWidget->setLayout(commitLayout); 
  
  m_widgets->insertWidget(1, m_codeViewWidget);
  m_widgets->insertWidget(2, m_commitViewWidget);
  m_widgets->insertWidget(3, m_branchViewWidget);
  connect(m_codeView, SIGNAL(newCommit()), m_commitView, SLOT(updateCommitView()));
  
  connect(m_branchView, SIGNAL(renewObject()), this, SLOT(onBranchViewChanged()));
}
void GMainWindow::onBranchViewChanged() {
  updateView();
  m_widgets->setCurrentIndex(3);
}
