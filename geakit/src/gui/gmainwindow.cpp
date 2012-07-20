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
  
  QHash<QString, QVariant> projectsHash;
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
  m_codeViewWidget = m_branchViewWidget = m_commitViewWidget = NULL;
  /*
  m_codeViewWidget = new QWidget;
  m_branchViewWidget = new QWidget;
  m_commitViewWidget = new QWidget;
*/
  m_widgets->addWidget(m_projectsWidget);
  /*
  m_widgets->addWidget(m_codeViewWidget);
  m_widgets->addWidget(m_branchViewWidget);
  m_widgets->addWidget(m_commitViewWidget);
*/
  m_widgets->setCurrentIndex(0);

  m_projectsWidget->setLayout(mainLayout);
  setCentralWidget(m_widgets);
  
  connect(m_addButton, SIGNAL(clicked()), this, SLOT(addProjectToLocal()));
  connect(m_rmButton, SIGNAL(clicked()), this, SLOT(removeProjectInLocal()));

  connect(m_projectsLocal, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(openProject(QListWidgetItem*)));

}
void GMainWindow::initProjectItems() {
  /***************just get some tests******************
  QListWidgetItem* projectItem = new QListWidgetItem(m_projectsOnline);
  projectItem->setText(tr("new_git"));
  QListWidgetItem* projectItem1 = new QListWidgetItem(m_projectsOnline);
  projectItem1->setText(tr("git_myself"));
  **************************end************************/
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
  //connect(m_command->getProcess(), SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onProcessFinished(int, QProcess::ExitStatus)));
  connect(m_command, SIGNAL(finishedProcess()), this, SLOT(onProcessFinished()));
  m_projectsOnline->setEnabled(false);
  this->statusBar()->showMessage(tr("connectting ..."));

  /****************projects local************************/
  QHash<QString, QString>::const_iterator it = m_projectsLocalHash.constBegin();
  while (it != m_projectsLocalHash.constEnd()) {
    QListWidgetItem* project = new QListWidgetItem(m_projectsLocal);
    QString text = QString("%1:\t%2").arg(it.key()).arg(it.value());
    project->setText(text);
    ++ it;
  }
  /*******************************deal in the slot****************
  qDebug() << "after connect";
  qDebug() << m_api.getReposNum();
  QHash<QString, QString> reposHash = m_api.getRepos();
  QHash<QString, QString>::const_iterator it = reposHash.constBegin();
  while (it != reposHash.constEnd()) {
    qDebug() << it.key() << ":" << it.value();
    it ++ ;
  }
  *******************************end****************************/
}
void GMainWindow::addProjectToLocal() {
  /*************test, projects' name get from the settings*****************/
  QList<QListWidgetItem* > selectedProjects = m_projectsOnline->selectedItems();
  QList<QListWidgetItem*>::iterator it = selectedProjects.begin();
  while (it != selectedProjects.end()) {
    //QListWidgetItem* item = m_projectsOnline->takeItem(m_projectsOnline->row(*it));
    QString text = (*it)->text();
    qDebug() << text;
    QRegExp rx = QRegExp("([^:]+)?:");
    int pos = text.indexOf(rx);
    if (pos < 0) return;
    QString projectName = rx.cap(1);
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Project Dir Local"), tr("/home/yuping/yp/git"));
    QString path = dirName + "/" + projectName;
    //save the value
    if (m_projectsLocalHash.value(projectName) != "") {
      QMessageBox::warning(this, tr("warning"), tr("projects has been here, try another place"));
      return;
    }
    m_projectsLocalHash.insert(projectName, path);

    //now use git clone to clone the repository to local
    m_command->setWorkDir(dirName);
    //here use https://name@github.com/name/repos_name.git as the url;
    QString reposUrl = QString("https://%1:%3@github.com/%1/%2.git").arg(m_account->username()).arg(projectName).arg(m_account->password());
    //reposUrl = reposUrl + projectName + ".git";
    qDebug() << reposUrl;
    QString cmd = QString("git clone %1").arg(reposUrl);
    qDebug() << cmd;
    //add tips
    this->statusBar()->showMessage(tr("clone the repos"));
    //m_command->setCmd(cmd);
    //do it asycronize
    m_command->setWaitTime(0);
    m_command->execute(cmd);
    m_projectsLocal->setEnabled(false);
    //add the latestUpdateRepos String to reset the remote url in the config file(remove the password)
    m_latestUpdatedRepo = projectName;
    //qDebug() << m_command->output();
   // this->statusBar()->showMessage("");
    QListWidgetItem* item = new QListWidgetItem(m_projectsLocal);
    QString tmpText = QString("%1:\t%2/%1").arg(projectName).arg(dirName);
    item->setText(tmpText);
    m_projectsLocal->addItem(item);
    it++;
  }
}
void GMainWindow::removeProjectInLocal() {
  QList<QListWidgetItem* > selectedProjects = m_projectsLocal->selectedItems();
  QList<QListWidgetItem* >::iterator it = selectedProjects.begin();
  while ( it != selectedProjects.end()) {
    int reply = QMessageBox::question(this, tr("warning"), tr("Do you really want to delete the project?"), QMessageBox::Ok, QMessageBox::No);
    if ( QMessageBox::No == reply) {
      qDebug() << "Don't remove it";
      return;
    }
    QListWidgetItem* item = m_projectsLocal->takeItem(m_projectsLocal->row(*it));
    //m_projectsOnline->addItem(item);
    QString projectString = item->text();
    QStringList tmpList = projectString.split(":");
    qDebug() << tmpList[0];
    m_projectsLocalHash.remove(tmpList[0]);
    bool result = m_command->removeGitDir(tmpList[1].trimmed()); 
    qDebug() << "remove result:" << result;
    if (NULL != m_currentRepo) {
      QString workdir = QString(git_repository_workdir(m_currentRepo));
      qDebug() << workdir;
      //remove the last char;
      workdir.resize(workdir.length() - 1);
      qDebug() << tmpList[1].trimmed();
      if (workdir == tmpList[1].trimmed()) {
        git_repository_free(m_currentRepo);
        m_currentRepo = NULL;
        freeWidgets(); 
      }
    }
    /*******also can remove the projects on disk***************/
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
  QStringList tmpList = (project->text()).split(":");
  QString reposPath = tmpList[1].trimmed();
  int error = git_repository_open(&m_currentRepo, reposPath.toLocal8Bit().constData());
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
  freeWidgets();
  /*
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
  */
  m_codeViewWidget = new QWidget;
  m_branchViewWidget = new QWidget;
  m_commitViewWidget = new QWidget;

  GCommitView* m_commitView = new GCommitView(this, m_currentRepo);
  GCodeView* m_codeView = new GCodeView(this, m_currentRepo);
  GBranchView* m_branchView = new GBranchView(this, m_currentRepo);

  m_branchView->setPassword(m_account->password());
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
void GMainWindow::onAccessComplete(GRepositoryAPI::ResultCode resultCode) {
  m_projectsOnline->setEnabled(true); 
  this->statusBar()->showMessage("");
  switch (resultCode)
  {
    case GRepositoryAPI::SUCCESSFUL:
      {
        qDebug() << m_reposAPI->getReposNum();
        QHash<QString, QString> reposHash = m_reposAPI->getRepos();
        QHash<QString, QString>::const_iterator it = reposHash.constBegin();
        while (it != reposHash.constEnd()) {
          qDebug() << it.key() << ":" << it.value();
          QListWidgetItem* projectItem = new QListWidgetItem(m_projectsOnline);
          QString text = QString("%1:\t%2").arg(it.key()).arg(it.value());
          projectItem->setText(text);
          it ++ ;
        }
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
void GMainWindow::onProcessFinished(/*int exitCode, QProcess::ExitStatus exitStatus*/) {
  qDebug() << "finished the process!!";
  /*
  if (exitStatus == QProcess::CrashExit)
  {
    qDebug() << "crashed";
  }
  else {*/
    m_projectsLocal->setEnabled(true);
    statusBar()->showMessage("");
    resetConfigUrl();
    //QString workdir = QString(git_repository_workdir());
    //freeWidgets();
 // }
}
void GMainWindow::freeWidgets() {
   //free the memory first
  if (NULL != m_commitViewWidget) {
    m_widgets->removeWidget(m_commitViewWidget);
    delete m_commitViewWidget;
    m_commitViewWidget = NULL;
  }
  if (NULL != m_codeViewWidget) {
    m_widgets->removeWidget(m_codeViewWidget);
    delete m_codeViewWidget;
    m_codeViewWidget = NULL;
  }
  if (NULL != m_branchViewWidget) {
    m_widgets->removeWidget(m_branchViewWidget);
    delete m_branchViewWidget;
    m_branchViewWidget = NULL;
  }
}
void GMainWindow::resetConfigUrl() {
  QString reposWorkdir = m_projectsLocalHash.value(m_latestUpdatedRepo);
  git_repository* tmpRepos;
  git_config* tmpConfig;
  int error = git_repository_open(&tmpRepos, reposWorkdir.toLocal8Bit().constData());
  if ( error < GIT_SUCCESS) {
    qDebug() << "open repository " << m_latestUpdatedRepo;
    return;
  }
  error = git_repository_config(&tmpConfig, tmpRepos);
  if ( error < GIT_SUCCESS) {
    qDebug() << "open config failed";
  }
  const char* name = "remote.origin.url";
  const char* value;
  error = git_config_get_string(tmpConfig, name, &value);
  if (error == GIT_SUCCESS) {
    qDebug() << name << ": " << value << endl;
  }
  QString url(value);
  //remove the password in the url
  QRegExp rx = QRegExp("(:[^//].+)?@");
  int pos = url.indexOf(rx);
  if (pos >= 0) {
    //qDebug() << rx.cap(1);
    url.replace(rx.cap(1), "");
  }
  //now set the value to the config file
  error = git_config_set_string(tmpConfig, name, url.toLocal8Bit().constData());
  //free the memory
  git_config_free(tmpConfig);
  git_repository_free(tmpRepos);
}
