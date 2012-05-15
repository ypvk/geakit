#include "gcommitdialog.h"
#include <QTextEdit>
#include <QPushButton>
#include <QDebug>
#include <QMessageBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

GCommitDialog::GCommitDialog(QWidget* parent) : QDialog(parent)
{
  m_message = new QTextEdit(this);
  m_okButton = new QPushButton(tr("OK"), this);

  QGroupBox* m_groupBox = new QGroupBox(tr("Commit Message"), this);
  QVBoxLayout* groupLayout = new QVBoxLayout(m_groupBox);
  groupLayout->addWidget(m_message);
  m_groupBox->setLayout(groupLayout);

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(m_groupBox);

  QHBoxLayout* buttonLayout = new QHBoxLayout;
  buttonLayout->addStretch(1);
  buttonLayout->addWidget(m_okButton);
  
  mainLayout->addLayout(buttonLayout);
  setLayout(mainLayout);
  
  connect(m_okButton, SIGNAL(clicked()), this, SLOT(onOkButtonClicked()));
}
void GCommitDialog::onOkButtonClicked() {
  QString content = m_message->toPlainText();
  if ("" == content) {
     QMessageBox::warning(this, tr("warning"), tr("message is empty, you should type in\
the message, or you can just close the dialog to quit\n"));
   return;
  }  
  qDebug() << content;
  accept();
} 
QString GCommitDialog::message() const {
  return m_message->toPlainText();
}
GCommitDialog::~GCommitDialog() {
}
