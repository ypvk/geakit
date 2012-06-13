#include "gremotenamedialog.h"
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QMessageBox>

GRemoteNameDialog::GRemoteNameDialog(QWidget* parent) : QDialog(parent)
{
  m_remoteNameLabel = new QLabel(tr("remoteName"), this);
  m_remoteUrlLabel = new QLabel(tr("remoteUrl"), this);

  m_remoteNameEdit = new QLineEdit(this);
  m_remoteUrlEdit = new QLineEdit(this);

  m_okButton = new QPushButton(tr("OK"), this);

  connect(m_okButton, SIGNAL(clicked()), this, SLOT(onOkButtonClicked()));

  QGridLayout* editLayout = new QGridLayout;
  editLayout->addWidget(m_remoteNameLabel, 0, 0);
  editLayout->addWidget(m_remoteNameEdit, 0, 1);
  editLayout->addWidget(m_remoteUrlLabel, 1, 0);
  editLayout->addWidget(m_remoteUrlEdit, 1, 1);

  QHBoxLayout* buttonLayout = new QHBoxLayout;
  buttonLayout->addStretch(1); 
  buttonLayout->addWidget(m_okButton);

  QVBoxLayout* mainLayout = new QVBoxLayout;
  mainLayout->addLayout(editLayout);
  mainLayout->addLayout(buttonLayout);

  setLayout(mainLayout);
  setWindowTitle(tr("create new remote"));
}

QString GRemoteNameDialog::remoteName() const {
  return m_remoteNameEdit->text();
}
QString GRemoteNameDialog::remoteUrl() const {
  return m_remoteUrlEdit->text();
}
void GRemoteNameDialog::onOkButtonClicked() {
  if ("" == m_remoteNameEdit->text() || "" == m_remoteUrlEdit->text()) {
    QMessageBox::warning(this, tr("warning"), tr("Input can't be empty"));
    qDebug() << "empty input";
    return;
  }
  accept();
}
