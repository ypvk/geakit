#ifndef GBRANCHVIEW_H
#define GBRANCHVIEW_H

#include <QWidget>
#include <git2.h>

class GBranchView : public QWidget
{
  Q_OBJECT
  public:
    explicit GBranchView(QWidget* parent = 0, git_repository* repo = 0);
    ~GBranchView();
  private:
    git_repository* m_repo;
};
#endif
