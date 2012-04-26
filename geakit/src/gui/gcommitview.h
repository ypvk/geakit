#ifndef GCOMMITVIEW_H
#define GCOMMITVIEW_H

#include <QWidget>
#include <git2.h>
class QLabel;
//class QScrollArea;
class QFrame;
class CommitItem;


class GCommitView : public QWidget
{
  Q_OBJECT
  public:
    explicit GCommitView(QWidget* parent = 0, git_repository* m_repos = 0);
    GCommitView(git_repository* m_repos);
    void setRepos(git_repository* repos);
    ~GCommitView();
  private:
    void init();
  private:
    git_repository* m_repos;
    
    QFrame* m_frame;
  //  QSCrollArea* m_viewArea;
   // QList<QFrame*> m_itemsView;
    QList<CommitItem*> m_commitItems;
};

class CommitItem : public QWidget
{
  public:
    explicit CommitItem(QWidget* parent = 0);
    CommitItem(git_commit* m_commit);
    ~CommitItem();
  private:
    void init();
  private:
    QString m_commitMessgae;
    QString m_commitTime;
    QString m_commitAuthor;
    QString m_commitCommiter;
    QString m_commitOid;
    //git_commit struct
    git_commit* m_commit;
};
#endif
