#include "gcommitview.h"
#include <QtGui>

GCommitView::GCommitView(QWidget* parent, git_repository* m_repos) : QWidget(parent){
  this->init();
  }
GCommitView::GCommitView(git_repository* m_repos) {
  this->m_repos = m_repos;
  m_viewArea = new QScrollArea;
  this->init();
}
void GCommitView::setRepos(git_repository* repos) {
  this->m_repos = repos;
}
void init() {


