#include <QMenu>
#include <QAction>

#include "ui_gmainwindow.h"

#include "gmainwindow.h"
#include "gsettingsdialog.h"

#include "data/account.h"

GMainWindow::GMainWindow(QWidget* parent):QMainWindow(parent), ui(new Ui::GMainWindow)
{
  ui->setupUi(this);
  setupActions();
  setupMenus();
  loadSettings();
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

void GMainWindow::setupMenus()
{
  m_editMenu = menuBar()->addMenu(tr("&Edit"));
  m_editMenu->addAction(m_settingsAction);
}

void GMainWindow::setupActions()
{
  m_settingsAction = new QAction(tr("Settings"), this);
  m_settingsAction->setIcon(QIcon(":icons/action_settings.png"));
  m_settingsAction->setShortcut(Qt::CTRL + Qt::Key_Comma);
  m_settingsAction->setStatusTip(tr("Settings of Geakit Clinet"));
  connect(m_settingsAction, SIGNAL(triggered()), this, SLOT(onSettingsActionTriggered()));
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
