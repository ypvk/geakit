#include <QDebug>
#include <QTimer>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include "gprocessdialog.h"
#include "gitcommand.h"

GProcessDialog::GProcessDialog(QWidget* parent) : QDialog(parent)
{
  m_message = new QLabel(this);
  m_cancelButton = new QPushButton(tr("Cancel"), this);
  m_timer = new QTimer(this);
  m_timer->setInterval(500);
  QHBoxLayout* m_layout = new QHBoxLayout(this);
  m_layout->addWidget(m_message);
  m_layout->addStretch(1);
  m_layout->addWidget(m_cancelButton);
  m_times = 0;

  connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked())); 
  connect(m_timer, SIGNAL(timeout()), this, SLOT(changeContent()));

  setLayout(m_layout);
}

GProcessDialog::~GProcessDialog()
{
}
void GProcessDialog::setTitleName(const QString& title)
{
}
void GProcessDialog::setCommand(GitCommand* command)
{
  m_command = command;
}
void GProcessDialog::setContent(const QString& content)
{
  m_content = content;
  m_message->setText(content);
}
void GProcessDialog::onCancelButtonClicked()
{
  if (m_command) {
    m_command->kill();
  }
  this->accept();
}
void GProcessDialog::changeContent()
{
  if (m_times < 6) {
    QString message = QString("%1.").arg(m_message->text());
    m_message->setText(message);
    m_times ++;
  }
  else {
    m_times = 0;
    m_message->setText(m_content);
  }
}
void GProcessDialog::show()
{
  m_timer->start();
  QDialog::show();
}
