#include <QTextEdit>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QHBoxLayout>
#include "ghighlighter.h"
#include "gcodevieweditor.h"

GCodeViewEditor::GCodeViewEditor(QWidget* parent) : QWidget(parent) 
{
  m_edit = new QTextEdit(this);
  QFont font;
  font.setFamily("Monospace");
  font.setFixedPitch(true);
  font.setPointSize(10);

  m_edit->setFont(font);

  m_highLighter = new GHighLighter(m_edit->document());

  QHBoxLayout* m_layout = new QHBoxLayout;
  m_layout->addWidget(m_edit);
  setLayout(m_layout);
}
void GCodeViewEditor::loadFile(const QString& fileName) {

  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "can't open the file";
    return;
  }
  m_highLighter->setCurrentMode(GHighLighter::CPP);
  QTextStream inputStream(&file);
  QString str = inputStream.readAll();
  m_edit->setPlainText(str);
  m_edit->setReadOnly(true);
  file.close();
}
GCodeViewEditor::~GCodeViewEditor() {
}
