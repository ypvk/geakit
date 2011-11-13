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

void GMainWindow::onSettingsActionTriggered()
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
	settingsAction = new QAction(tr("Settings"), this);
	settingsAction->setIcon(QIcon(":icons/action_settings.png"));
	settingsAction->setShortcut(Qt::CTRL + Qt::Key_Comma);
	settingsAction->setStatusTip(tr("Settings of Geakit Clinet"));
	connect(settingsAction, SIGNAL(triggered()), this, SLOT(onSettingsActionTriggered()));
}
