#include "ui_gsettingsdialog.h"

#include "gsettingsdialog.h"

GSettingsDialog::GSettingsDialog(QWidget* parent):QDialog(parent), ui(new Ui::GSettingsDialog)
{
	ui->setupUi(this);
	connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(onLoginButtonClicked()));
}

void GSettingsDialog::onLoginButtonClicked()
{
	QString username, password;
	username = ui->usernameEdit.text();
	password = ui->passwordEdit.text();
}
