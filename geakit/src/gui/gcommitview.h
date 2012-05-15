#ifndef GCOMMITVIEW_H
#define GCOMMITVIEW_H

#include <QWidget>
#include <git2.h>
#include <QUrl>
class QLabel;
//class QScrollArea;
//class QFrame;
//class CommitItem;
class QWebView;
class QSplitter;
class GPatchView;
class QGroupBox;

class GCommitView : public QWidget
{
  Q_OBJECT
  public:
    explicit GCommitView(QWidget* parent = 0, git_repository* m_repos = 0);
    void setRepos(git_repository* repos);
    ~GCommitView();
  public slots:
    void onLinkClicked(const QUrl& url);
    void updateCommitView();
  private:
   // void init();
    QString buildEachElement(git_commit* commit, git_oid* oid);
    void setContents(const QString& html);
    void setHtmlHead(QString& html);
    void setHtmlEnd(QString& html);
    void htmlParaphrase(QString& html, const QHash<QString, QString>& stringHash);
  private:
    git_repository* m_repos;
    
    GPatchView* m_patchView;
    QGroupBox* m_patchGroupBox;
    QSplitter* m_splitter;
  //  QFrame* m_frame;
    QWebView* m_webView;
  //  QSCrollArea* m_viewArea;
   // QList<QFrame*> m_itemsView;
   // QList<CommitItem*> m_commitItems;
   // QList<git_commit*> m_commitList;
};
/*
class CommitItem : public QWidget
{
  public:
    explicit CommitItem(QWidget* parent = 0, git_commit* commit = 0);
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
};*/
#endif
