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
  setWindowTitle(title);
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
  this->reject();
}
void GProcessDialog::changeContent()
{
  if (m_times < 8) {
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
void GProcessDialog::exec(const QString& type, const QString& target)
{
  if (type == "Sync") {
    this->Sync(target, 0);  
  }
  else if (type == "Push") {
    m_command->gitPush(target);
  }
  else if (type == "Fetch") {
    m_command->gitFetch(target);
  }
  else if (type == "Clone") {
    m_command->gitClone(target);
  }
  connect(m_command, SIGNAL(processSuccess()), this, SLOT(onProcessSuccess()));
  connect(m_command, SIGNAL(processErrored()), this, SLOT(onProcessError()));
  m_timer->start();
  QDialog::exec();
}

void GProcessDialog::Sync(const QString& target, int step)
{
  switch (step) {
    case 0:
      m_command->gitFetch(target);
      break;
    case 1:
      QString head = m_command->gitRefHead();
      QString remoteBranchName = QString("%1/%2").arg(target).arg(head);
      if (m_command->branchExists(remoteBranchName)) {
        m_command->gitMergeBranch(remoteBrnachName);
      }
      else 
        this->Sync(m_target, ++m_syncStep);
      break;
    case 2:
      m_command->gitPush(target);
      break;
    default:
      ;
  }
}
void GProcessDialog::onProcessSuccess()
{
  if (this->m_isSync)
  {
    ++m_syncStep;
    this->Sync(m_target, m_syncStep);
    return;
  }

  this->accept();
}
void GProcessDialog::onProcessError()
{
  QMessageBox::information(0, tr("warning"), tr("error runing ") + this->m_content);
  this->reject();
}
