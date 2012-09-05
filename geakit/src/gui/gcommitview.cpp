#include "gcommitview.h"
#include "gitcommand.h"
#include <QtWebKit/QWebView>
#include <QtGui>
#include <QMessageBox>
#include <QFile>
#include <QDebug>
#include "gpatchview.h"
#include <time.h>

GCommitView::GCommitView(QWidget* parent, git_repository* m_repos) : QWidget(parent){
  this->m_repos = m_repos;
  //set the layout
 // m_frame = new QFrame(this);
  m_webView = new QWebView(this);
  m_splitter = new QSplitter(this);

  m_workdir = git_repository_workdir(m_repos);
  m_command = new GitCommand(this, m_workdir);
  m_command->setRepository(m_repos);

  m_splitter->setOrientation(Qt::Horizontal);
  m_splitter->setStretchFactor(1, 1);

  m_patchGroupBox = new QGroupBox(this);
  m_patchGroupBox->setTitle("PatchView");
  QHBoxLayout* boxLayout = new QHBoxLayout(m_patchGroupBox);
  
  m_patchView = new GPatchView(this, m_repos);
  boxLayout->addWidget(m_patchView);

  m_patchGroupBox->setLayout(boxLayout);
  m_patchGroupBox->hide();//hide before

  QHBoxLayout* mainLayout = new QHBoxLayout(this);
  mainLayout->addWidget(m_splitter);
  setLayout(mainLayout);
//  mainLayout->addWidget(mainLayout);
 // m_frame->setLayout(mainLayout);

  m_splitter->addWidget(m_webView);
  m_splitter->addWidget(m_patchGroupBox);
  //set the webview
  m_webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  connect(m_webView, SIGNAL(linkClicked(QUrl)), this, SLOT(onLinkClicked(QUrl)));
  //build the contents
  this->updateCommitView();
}
void GCommitView::setRepos(git_repository* repos) {
  this->m_repos = repos;
}
void GCommitView::updateCommitView() {
  //setContents();
  if (m_repos == NULL) {
    qDebug() << "error , empty repository!"; 
    return;
  }
  QString html;
  setHtmlHead(html);
  QList<QStringList> Datas = m_command->gitCommitDatas();
  QList<QStringList>::const_iterator it = Datas.constBegin();
  for (; it != Datas.constEnd(); it++) {
    html = html + buildEachElement(*it);
  }
  setHtmlEnd(html);
  setContents(html);
}
QString GCommitView::buildEachElement(const QStringList& dataList) {
  QString html;
  QFile file(":/html_files/commit_unit.html");
  if(!file.open(QIODevice::ReadOnly | QIODevice::ReadOnly)) {
    qDebug() << "Open file falure!";
    return "";
  }
  QTextStream fs(&file);
  html = fs.readAll();
  file.close();
  //build the element
  QHash<QString, QString> stringHash;
  stringHash.insert("message", dataList[4]);
  stringHash.insert("authorName", dataList[2]);
  stringHash.insert("date", dataList[3]);
  stringHash.insert("sha", dataList[0]);
  stringHash.insert("sha-short", dataList[1]);
  htmlParaphrase(html, stringHash);
  return html;
}
//parahrase the html value
void GCommitView::htmlParaphrase(QString& html, const QHash<QString,QString>& stringHash) {
  QList<QString> keys = stringHash.keys();
  for (int i = 0; i < keys.size(); i ++) {
    html.replace(QRegExp("__" + keys[i] + "__"), stringHash.value(keys[i]));
  }
}
void GCommitView::setHtmlHead(QString& html) {
  QFile file(":/html_files/commit_head.html");
  if(!file.open(QIODevice::ReadOnly)) {
    qDebug() << "Open file falure!";
    return;
  }
  QTextStream fs(&file);
  html = fs.readAll();
  file.close();
}
void GCommitView::setHtmlEnd(QString& html) {
//html = html + "</ul></body></html>";
  QFile file(":/html_files/commit_end.html");
  if(!file.open(QIODevice::ReadOnly)) {
    qDebug() << "Open file falure!";
    return;
  }
  QTextStream fs(&file);
  html = html + fs.readAll();
  file.close();
}
void GCommitView::setContents(const QString& html) {
 // QString html = "<a href=\"yuping\">link for test</a>";
  m_webView->setHtml(html);
}
void GCommitView::onLinkClicked(const QUrl& url) {
  QMessageBox::information(this, tr("infp"), url.toString());
 // m_patchView->setCommit(m_commitList[0]);
  m_patchView->setCommitOid(url.toString());
//  m_patchView->execute();
  m_patchView->getDiff();
  m_patchGroupBox->show();
 // delete m_pathView;
}
GCommitView::~GCommitView() {
//  for (int i = 0; i < m_commitList.size(); i ++ ) {
//    git_commit_free(m_commitList[i]);
//  }
}

