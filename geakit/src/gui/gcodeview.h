#ifndef GDODEVIEW_H
#define GCODEVIEW_H

#include <QWidget>
#include <git2.h>

class GCodeView : public QWidget
{
  Q_OBJECT
  public:
    explicit GCodeView(QWidget* parent = 0, git_repository* repos = 0);
    ~GCodeView();
  private:
    git_repository* m_repos;
};
#endif
