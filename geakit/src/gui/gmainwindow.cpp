#include <QMenu>
#include <QAction>

#include "ui_gmainwindow.h"

#include "gmainwindow.h"
#include "gsettingsdialog.h"

GMainWindow::GMainWindow(QWidget* parent):QMainWindow(parent), ui(new Ui::GMainWindow)
{
	ui->setupUi(this);
	setupActions();
	setupMenus();
}

void GMainWindow::onActionSettingsTriggered()
{
	GSettingsDialog* dlg = new GSettingsDialog(this);
	if(dlg->exec())
	{
	}
	delete dlg;
}

void GMainWindow::setupMenus()
{
	editMenu = menuBar()->addMenu(tr("&Edit"));
	editMenu->addAction(settingsAction);
}

void GMainWindow::setupActions()
{
	//TODO different define due to different OS
	settingsAction = new QAction(tr("Settings"), this);
	settingsAction->setIcon(QIcon(":icons/action_settings.png"));
	settingsAction->setShortcut(QString("Ctrl+,"));
	settingsAction->setStatusTip(tr("Settings of Geakit Clinet"));
	connect(settingsAction, SIGNAL(triggered()), this, SLOT(onActionSettingsTriggered()));
}
