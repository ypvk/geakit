#ifndef GCODEVIEWEDITOR_H
#define GCODEVIEWEDITOR_H

#include <QWidget>

class QTextEdit;
class GHighLighter;

class GCodeViewEditor : public QWidget
{
  Q_OBJECT
  public:
    explicit GCodeViewEditor(QWidget* parent = 0);
    void loadFile(const QString& fileName);
    ~GCodeViewEditor();
  private:
    QTextEdit* m_edit;
    GHighLighter* m_highLighter;
}; 

#endif/*GCODEVIEWEDITOR_H*/
