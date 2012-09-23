#include "gpatchview.h"
#include <QTextEdit>
#include <QDebug>
#include <QHBoxLayout>
#include "ghighlighter.h"
#include "gitcommand.h"

GPatchView::GPatchView(QWidget* parent,git_repository* repo) : QWidget(parent) {
 // m_commit = commit;
  m_repo = repo;
//  m_process = new QProcess(this);
  m_text = new QTextEdit(this);
  m_text->setReadOnly(true);
  QHBoxLayout* viewLayout = new QHBoxLayout(this);
  viewLayout->addWidget(m_text);
  setLayout(viewLayout);

  m_command = new GitCommand(this);
  m_highLighter = new GHighLighter(m_text->document());
  m_highLighter->setCurrentMode(GHighLighter::GIT_DIFF);
}
void GPatchView::setCommitOid(const QString& commitOid) {
  m_commitOid = commitOid;
}
void GPatchView::getDiff() {

  QString cmd = QString("git log -1 -p %1").arg(m_commitOid);
  QString workDir(git_repository_workdir(m_repo));
  m_command->setWorkDir(workDir);
  m_command->execute(cmd);
  QString content = m_command->output();

  QRegExp rx("(diff --git.*)");
  if (content.indexOf(rx) >= 0) content = rx.cap(1);
  else content = "";
 // qDebug() << m_content;
  m_text->setPlainText(content);
}
GPatchView::~GPatchView() {
}
