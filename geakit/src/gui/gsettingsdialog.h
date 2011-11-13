#ifndef GEAKIT_GUI_GSETTINGSDIALOG_H
#define GEAKIT_GUI_GSETTINGDSIALOG_H

#include <QDialog>

namespace Ui
{
	class GSettingsDialog;
}

class GSettingsDialog :public QDialog
{
	Q_OBJECT
	public:
		explicit GSettingsDialog(QWidget* parent = 0);
		~GSettingsDialog(){}
	private:
		Ui::GSettingsDialog* ui;

	private slots:
		void onLoginButtonClicked();
};

#endif
