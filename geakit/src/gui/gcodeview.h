#ifndef GDODEVIEW_H
#define GCODEVIEW_H

#include <QWidget>
#include <git2.h>
#include <QDir>
class QTreeWidget;
class QPushButton;
class QTreeWidgetItem;
class GitCommand;

class GCodeView : public QWidget
{
  Q_OBJECT
  public:
    explicit GCodeView(QWidget* parent = 0, git_repository* repos = 0);
    ~GCodeView();
  signals:
    void reposDataChanged();
  private:
//    void searchFiles();
  private:
    void updateView(QDir& dir);
  public slots:
    void gitAdd();
    void gitRm();
    void gitCommit();
    void onItemDoubleCilcked(QTreeWidgetItem* item, int column);
    void onItemClicked(QTreeWidgetItem* item, int column);
  private:
    git_repository* m_repos;
    QTreeWidget* m_fileList;
    QPushButton* m_gitAddButton;
    QPushButton* m_gitRmButton;
    QPushButton* m_gitCommitButton;
    QString m_workdirRoot;
    QString m_tmpRoot;
    GitCommand* m_command;
    QList<QTreeWidgetItem*> m_selectedItems;
};
#endif
