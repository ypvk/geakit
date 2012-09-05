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
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QRegExp>

#include <api/grepositoryapi.h>

#include "ui_gmainwindow.h"

#include "gprojectdialog.h"
#include "gmainwindow.h"
#include "gsettingsdialog.h"
#include "gprojectdialog.h"
#include "gcommitview.h"
#include "gbranchview.h"
#include "gcodeview.h"
#include "gprojectsview.h"
#include "gitcommand.h"

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
  this->setWindowTitle(tr("Geakit Git Client"));
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
  delete m_reposAPI;
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
  QHash<QString, QVariant> projectsHash = m_settings.value("projectsLocal", "").toHash();
  QSize size = m_settings.value("size", QSize(800, 600)).toSize();
  QPoint point = m_settings.value("position", QPoint(0, 0)).toPoint();

  this->move(point);
  this->resize(size);

  QHash<QString, QVariant>::const_iterator it = projectsHash.constBegin();
  while (it != projectsHash.constEnd()) {
    m_projectsLocalHash.insert(it.key(), it.value().toString());
    ++it;
  }
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
  
  m_settings.setValue("size", size());
  m_settings.setValue("position", pos());

  QHash<QString, QVariant> projectsHash;
  m_projectsLocalHash = m_projectsWidget->projectsLocalHash();
  QHash<QString, QString>::const_iterator it = m_projectsLocalHash.constBegin();
  while (it != m_projectsLocalHash.constEnd()) {
    projectsHash.insert(it.key(), QVariant(it.value()));
    ++it;
  }
  m_settings.setValue("projectsLocal", projectsHash);
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
  m_widgets = new QStackedWidget(this);
  //this three widget is controlled by myself can be delete and new by myself;
  m_projectsWidget = new GProjectsView(this, m_account);
  m_codeViewWidget = NULL;
  m_branchViewWidget = NULL;
  m_commitViewWidget = NULL;

  m_widgets->addWidget(m_projectsWidget);

  m_widgets->setCurrentIndex(0);

  setCentralWidget(m_widgets);
  connect(m_projectsWidget, SIGNAL(openProject(QString)), this, SLOT(onOpenProject(QString)));
  connect(m_projectsWidget, SIGNAL(removeProject(QString)), this, SLOT(onRemoveProject(QString)));
  connect(m_projectsWidget, SIGNAL(workingStatusChanged(QString, QString)), this, SLOT(onWorkingStatusChanged(QString, QString)));
}
void GMainWindow::initProjectItems() {
  if ("" == m_account->username()) {
    QMessageBox::information(this, tr("warning"), tr("please set the name and the passwordfirst"));
    qDebug() << "error, no usrname and password is set";
    return;
  }
  m_command = new GitCommand(this);
  m_manager = new QNetworkAccessManager(this);
  m_reposAPI = new GRepositoryAPI(m_manager);
  m_reposAPI->setUsername(m_account->username());
  m_reposAPI->startConnect();
  connect(m_reposAPI, SIGNAL(complete(GRepositoryAPI::ResultCode)), this, SLOT(onAccessComplete(GRepositoryAPI::ResultCode)));
  m_projectsWidget->setProjectsOnlineEnabled(false);
  this->statusBar()->showMessage(tr("connectting ..."));

  m_projectsWidget->setProjectsLocalHash(m_projectsLocalHash);
  m_projectsWidget->initProjectsItems(m_projectsLocalHash, tr("local"));
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
void GMainWindow::onOpenProject(const QString& reposWorkdir) {

  if (NULL != m_currentRepo && QDir::toNativeSeparators(reposWorkdir + "/") != git_repository_workdir(m_currentRepo)) {
    git_repository_free(m_currentRepo);
  }
  else if (NULL != m_currentRepo && QDir::toNativeSeparators(reposWorkdir + "/")== git_repository_workdir(m_currentRepo)) {
    m_widgets->setCurrentIndex(1);
    qDebug() << "repository has been opened";
    return;
  }
  statusBar()->showMessage(tr("opening the repository..."));
  m_projectsWidget->setProjectsLocalEnabled(false);
  int error = git_repository_open(&m_currentRepo, reposWorkdir.toLocal8Bit().constData());
  if (error < GIT_SUCCESS) {
    qDebug() << "error open the repos";
    m_currentRepo = NULL;
    return;
  }
  updateView();
  statusBar()->showMessage("");
  m_projectsWidget->setProjectsLocalEnabled(true);
  m_widgets->setCurrentIndex(1);
}
void GMainWindow::onRemoveProject(const QString& reposWorkdir)
{
  if (NULL != m_currentRepo && QDir::toNativeSeparators(reposWorkdir + "/") == git_repository_workdir(m_currentRepo)) {
    git_repository_free(m_currentRepo);
    freeWidgets();
  }
}
void GMainWindow::updateView() {
  
  //remove the origin widget
  //free the memory first
  freeWidgets();

  m_codeViewWidget = new GCodeView(this, m_currentRepo);
  m_commitViewWidget = new GCommitView(this, m_currentRepo);
  m_branchViewWidget = new GBranchView(this, m_currentRepo);
 

//  m_branchViewWidget->setPassword(m_account->password());
  
  m_widgets->insertWidget(1, m_codeViewWidget);
  m_widgets->insertWidget(2, m_commitViewWidget);
  m_widgets->insertWidget(3, m_branchViewWidget);

  connect(m_codeViewWidget, SIGNAL(branchChanged()), m_commitViewWidget, SLOT(updateCommitView()));
  connect(m_codeViewWidget, SIGNAL(newCommit()), m_commitViewWidget, SLOT(updateCommitView()));
  connect(m_branchViewWidget, SIGNAL(branchChanged()), m_commitViewWidget, SLOT(updateCommitView()));
  connect(m_branchViewWidget, SIGNAL(branchChanged()), m_codeViewWidget, SLOT(onBranchChanged()));
  connect(m_codeViewWidget, SIGNAL(branchChanged()), this, SLOT(updateBranchView()));
  connect(m_branchViewWidget, SIGNAL(branchChanged()), this, SLOT(udpateBranchView()));
  
}
void GMainWindow::updateBranchView() {
  int index = m_widgets->currentIndex();
  if (NULL != m_branchViewWidget) {
    disconnect(m_branchViewWidget, 0, 0, 0);
    m_widgets->removeWidget(m_branchViewWidget);
    delete m_branchViewWidget;
    m_branchViewWidget = NULL;
  }
  m_branchViewWidget = new GBranchView(this, m_currentRepo);
  m_widgets->insertWidget(3, m_branchViewWidget);
  connect(m_branchViewWidget, SIGNAL(branchChanged()), m_commitViewWidget, SLOT(updateCommitView()));
  connect(m_branchViewWidget, SIGNAL(branchChanged()), m_codeViewWidget, SLOT(onBranchChanged()));
  connect(m_branchViewWidget, SIGNAL(branchChanged()), this, SLOT(updateBranchView()));
  m_widgets->setCurrentIndex(index);
}
void GMainWindow::onAccessComplete(GRepositoryAPI::ResultCode resultCode) {
  //m_projectsOnline->setEnabled(true); 
  m_projectsWidget->setProjectsOnlineEnabled(true);
  this->statusBar()->showMessage("");
  switch (resultCode)
  {
    case GRepositoryAPI::SUCCESSFUL:
      {
        qDebug() << m_reposAPI->getReposNum();
        QHash<QString, QString> reposHash = m_reposAPI->getRepos();
        m_projectsWidget->initProjectsItems(reposHash, QString("online"));
        break;
      }
    case GRepositoryAPI::ERROR:
      {
        QMessageBox::warning(this, tr("connect error"), tr("Can't connect to the website"));
        qDebug() << "Error Access";
        break;
      }
    case GRepositoryAPI::TIMEOUT:
      {
        QMessageBox::warning(this, tr("time out"), tr("time out"));
        qDebug() << "time out";
        break;
      }
  }
}
void GMainWindow::onWorkingStatusChanged(const QString& status, const QString& message)
{
  qDebug() << status << " " << message;
  if (status == "start") {
    statusBar()->showMessage(message);
  }
  if (status == "end") {
    statusBar()->showMessage("");
  }
  //to deal with more status
}
void GMainWindow::freeWidgets() {
   //free the memory first
  if (NULL != m_commitViewWidget) {
    m_widgets->removeWidget(m_commitViewWidget);
    delete m_commitViewWidget;
    m_commitViewWidget = NULL;
  }
  if (NULL != m_codeViewWidget) {
    disconnect(m_codeViewWidget, 0, 0, 0);
    m_widgets->removeWidget(m_codeViewWidget);
    delete m_codeViewWidget;
    m_codeViewWidget = NULL;
  }
  if (NULL != m_branchViewWidget) {
    disconnect(m_branchViewWidget, 0, 0, 0);
    m_widgets->removeWidget(m_branchViewWidget);
    delete m_branchViewWidget;
    m_branchViewWidget = NULL;
  }
}
