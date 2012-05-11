#ifndef GBRANCHVIEW_H
#define GBRANCHVIEW_H

#include <QWidget>
#include <git2.h>
#include <QProcess>
class QScrollArea;
class QGroupBox;
class GitCommand;


class GBranchView : public QWidget
{
  Q_OBJECT
  public:
    explicit GBranchView(QWidget* parent = 0, git_repository* repo = 0);
    ~GBranchView();
signals:
    void renewObject();
  private slots:
    void onChangeButtonClicked(int id);
    void onMergeButtonClicked(int id);
    void onRemoteButtonClicked(int id);
  private:
  private:
    git_repository* m_repo;
    QGroupBox* m_localBranchArea;
    QList<QGroupBox*> m_remotAreaList; 
    QScrollArea* m_mainArea;
    GitCommand* m_command;
 //   QProcess* m_process;
    QStringList branchList;
};
#endif
