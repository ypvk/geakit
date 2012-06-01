#include "gcommitview.h"
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
//init the commitItem
  //setContents();
  if (m_repos == NULL) {
    qDebug() << "error , empty repository!"; 
    return;
  }
  //get the head reference
  git_reference* m_reference;
  int error;
  error = git_repository_head(&m_reference, m_repos);

  const git_oid* oid = git_reference_oid(m_reference);
    
 // git_oid_fmt(id_str, oid);
  //now get the crevwalk, to realize the log function
  git_revwalk* walk;
  git_commit* wcommit;

  git_revwalk_new(&walk, m_repos);
  git_revwalk_sorting(walk, GIT_SORT_TIME);// | GIT_SORT_REVERSE);
  git_revwalk_push(walk, oid);
  //const git_signature* cauth;
  //const char* cmsg;
  
  //build the commitList value
  git_oid tmpOid;
  QString html = "";
  setHtmlHead(html);
  while ((git_revwalk_next(&tmpOid, walk)) == GIT_SUCCESS) {
    error = git_commit_lookup(&wcommit, m_repos, &tmpOid);
    if (error == -1) {qDebug() << "error get the commit!"; return;}
    //m_commitList << wcommit; 
    html = html + buildEachElement(wcommit, &tmpOid); 
    git_commit_free(wcommit);
  }
  setHtmlEnd(html);
  git_revwalk_free(walk);
  git_reference_free(m_reference);

  setContents(html);
//  git_commit_free(wcommit);
}
QString GCommitView::buildEachElement(git_commit* commit, git_oid* oid) {
  QString html;
  const git_signature* cauth;
  const char* cmsg;
  char sha[41] = {0};
  const char* time; 
  //build each value
  cmsg = git_commit_message(commit);
  cauth = git_commit_author(commit);
  git_oid_fmt(sha, oid);
  git_time gTime = cauth->when;

  QString qSha(sha);
  QString message(cmsg);
  QString author(cauth->name);
  //QString qSha(sha, 10);//use short sha code as the sha to show
  QString shaShort;
  for (int i = 0; i < 10; i++) 
    shaShort.append(sha[i]);
  
  time_t tmpTime = (time_t)gTime.time;
  time = ctime(&tmpTime);
 // qDebug() << gTime.time;
// now get the html;

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
  stringHash.insert("message", message);
  stringHash.insert("authorName", author);
  stringHash.insert("date", QString(time));
  stringHash.insert("sha", qSha);
  stringHash.insert("sha-short", shaShort);
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
  m_patchView->execute();
  m_patchGroupBox->show();
 // delete m_pathView;
}
GCommitView::~GCommitView() {
//  for (int i = 0; i < m_commitList.size(); i ++ ) {
//    git_commit_free(m_commitList[i]);
//  }
}

