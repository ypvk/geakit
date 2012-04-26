#ifndef GPROJECTFILES_H
#define GPROJECTFILES_H
#include <QWidget>

class GProjectFiles : public QWidget
{
  public:
    GProjectFiles(QWidget* parent = 0);
    ~GProjectFiles();
  private:
    QTreeWidget* fileList;
    QTextArea* fileContent;
}
