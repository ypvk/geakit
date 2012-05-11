#ifndef GPROJECTDIALOG_H
#define GPROJECTDIALOG_H

#include <QDialog>
#include <git2.h>
class QTabWidget;
class QPushButton;

class GProjectDialog : public QDialog
{
  Q_OBJECT
  public:
    explicit GProjectDialog(QWidget* parent = 0, git_repository* m_repos = 0);
    ~GProjectDialog();
  public slots:
    void onBranchViewUpdate();
  private:
    git_repository* m_repos;
    QTabWidget* m_projectTab;
    QPushButton* m_settingButton;
    QPushButton* m_mergeButton;
};
#endif
