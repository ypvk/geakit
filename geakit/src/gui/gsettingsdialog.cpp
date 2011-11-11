#include "ui_gsettingsdialog.h"

#include "gsettingsdialog.h"

GSettingsDialog::GSettingsDialog(QWidget* parent):QDialog(parent), ui(new Ui::GSettingsDialog)
{
	ui->setupUi(this);
}
