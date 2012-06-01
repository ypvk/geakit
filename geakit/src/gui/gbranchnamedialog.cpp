#include "gbranchnamedialog.h"
#include <QPushButton>
#include <QLineEdit>
#include <QMessageBox>
#include <QLabel>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>

GBranchNameDialog::GBranchNameDialog(QWidget* parent): QDialog(parent) 
{
  m_label = new QLabel(tr("Branch Name:"));
  m_okButton = new QPushButton(tr("Ok"), this);
  m_closeButton = new QPushButton(tr("Close"), this);
  m_lineEdit = new QLineEdit(this);

  QHBoxLayout* contentLayout = new QHBoxLayout;
  QHBoxLayout* buttonLayout = new QHBoxLayout;
  QVBoxLayout* mainLayout = new QVBoxLayout;

  contentLayout->addWidget(m_label);
  contentLayout->addWidget(m_lineEdit);
  
  buttonLayout->addStretch(1);
  buttonLayout->addWidget(m_okButton);
  buttonLayout->addWidget(m_closeButton);

  mainLayout->addLayout(contentLayout);
  mainLayout->addLayout(buttonLayout);

  setLayout(mainLayout);
  setWindowTitle(tr("Branch Name"));

  connect(m_okButton, SIGNAL(clicked()), this, SLOT(onOkButtonClicked()));
  connect(m_closeButton, SIGNAL(clicked()), this, SLOT(reject()));
}
void GBranchNameDialog::onOkButtonClicked() {
  QString name = m_lineEdit->text();
  if (name == "") {
    QMessageBox::warning(this, tr("warning"), tr("Empty name"));
    return;
  }
  accept();
}

QString GBranchNameDialog::getBranchName() {
  return m_lineEdit->text();
}
GBranchNameDialog::~GBranchNameDialog() {
}


