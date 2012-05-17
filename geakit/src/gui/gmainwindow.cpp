#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QGroupBox>
#include <QDebug>

#include "ui_gmainwindow.h"

#include "gprojectdialog.h"
#include "gmainwindow.h"
#include "gsettingsdialog.h"
#include "gprojectdialog.h"

#include "data/account.h"

GMainWindow::GMainWindow(QWidget* parent):QMainWindow(parent), ui(new Ui::GMainWindow)
{
  ui->setupUi(this);
  setupActions();
  setupMenus();
  loadSettings();
  buildGui();
  initProjectItems();
}

GMainWindow::~GMainWindow(){
  delete m_account;
  git_config_free(m_config);
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
void GMainWindow::onProjectActionTriggered()
{
  GProjectDialog* dlg = new GProjectDialog(this, 0);
  dlg->exec();
  delete dlg;
}

void GMainWindow::setupMenus()
{
  m_editMenu = menuBar()->addMenu(tr("&Edit"));
  m_editMenu->addAction(m_settingsAction);
  m_editMenu->addAction(m_projectAction);
}

void GMainWindow::setupActions()
{
  m_settingsAction = new QAction(tr("Settings"), this);
  m_settingsAction->setIcon(QIcon(":icons/action_settings.png"));
  m_settingsAction->setShortcut(Qt::CTRL + Qt::Key_Comma);
  m_settingsAction->setStatusTip(tr("Settings of Geakit Clinet"));
  connect(m_settingsAction, SIGNAL(triggered()), this, SLOT(onSettingsActionTriggered()));

  m_projectAction = new QAction(tr("Project"), this);
  m_projectAction->setIcon(QIcon(":icons/action_project.png"));
  m_projectAction->setShortcut(Qt::CTRL + Qt::Key_P);
  m_projectAction->setStatusTip(tr("Project Dialog"));
  connect(m_projectAction, SIGNAL(triggered()), this, SLOT(onProjectActionTriggered()));
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

  m_addButton = new QPushButton(tr("add"), this);
  m_rmButton = new QPushButton(tr("romove"), this);

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
  
  QWidget* centralWidget = new QWidget(this);
  centralWidget->setLayout(mainLayout);
  setCentralWidget(centralWidget);
  
  connect(m_addButton, SIGNAL(clicked()), this, SLOT(addProjectToLocal()));
  connect(m_rmButton, SIGNAL(clicked()), this, SLOT(removeProjectInLocal()));

  connect(m_projectsLocal, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(openProject(QListWidget*)));

}
void GMainWindow::initProjectItems() {
  QListWidgetItem* projectItem = new QListWidgetItem(m_projectsOnline);
  projectItem->setText(tr("project1"));
  QListWidgetItem* projectItem1 = new QListWidgetItem(m_projectsOnline);
  projectItem1->setText(tr("project3"));
}
void GMainWindow::addProjectToLocal() {
  QList<QListWidgetItem* > selectedProjects = m_projectsOnline->selectedItems();
  QList<QListWidgetItem*>::iterator it = selectedProjects.begin();
  while (it != selectedProjects.end()) {
    QListWidgetItem* item = m_projectsOnline->takeItem(m_projectsOnline->row(*it));
    m_projectsLocal->addItem(item);
    it++;
  }
}
void GMainWindow::removeProjectInLocal() {
  QList<QListWidgetItem* > selectedProjects = m_projectsLocal->selectedItems();
  QList<QListWidgetItem* >::iterator it = selectedProjects.begin();
  while ( it != selectedProjects.end()) {
    QListWidgetItem* item = m_projectsLocal->takeItem(m_projectsLocal->row(*it));
    m_projectsOnline->addItem(item);
    it++;
  }
}
