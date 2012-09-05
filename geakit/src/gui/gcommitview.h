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
class GitCommand;

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
    QString buildEachElement(const QStringList& dataList);
    void setContents(const QString& html);
    void setHtmlHead(QString& html);
    void setHtmlEnd(QString& html);
    void htmlParaphrase(QString& html, const QHash<QString, QString>& stringHash);
  private:
    git_repository* m_repos;
    
    GPatchView* m_patchView;
    QGroupBox* m_patchGroupBox;
    QSplitter* m_splitter;
    QWebView* m_webView;

    QString m_workdir;
    GitCommand* m_command;

};
#endif
