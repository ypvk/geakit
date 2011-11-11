#ifndef GEAKIT_GMAINWINDOW_H
#define GEAKIT_GMAINWINDOW_H

#include <QMainWindow>

class QMenu;
class QAction;

namespace Ui
{
	class GMainWindow;
}

class GMainWindow :public QMainWindow
{
	Q_OBJECT
	public:
		explicit GMainWindow(QWidget* parent = 0);
		~GMainWindow(){}

	private:
		Ui::GMainWindow* ui;
		QMenu* editMenu;
		QAction* settingsAction;
	
	private slots:
		void onActionSettingsTriggered();

	private:
		void setupMenus();
		void setupActions();
};

#endif
