#include "ghighlighter.h"
#include <QTextCharFormat>
#include <QDebug>

GHighLighter::GHighLighter(QTextDocument* parent) : QSyntaxHighlighter(parent)
{
  setupHighLightRules(CPP);
  setupHighLightRules(GIT_DIFF);
}
void GHighLighter::setupHighLightRules(Mode mode) {

  m_keywordFormat.setForeground(Qt::darkBlue);
  m_keywordFormat.setFontWeight(QFont::Bold);

  QList<HighLightRule> highLightRules;
  switch (mode) 
  {
    case CPP:
      {
        HighLightRule rule;
        QStringList keywordsPatterns;
        keywordsPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
          << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
          << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
          << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
          << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
          << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
          << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
          << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
          << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
          << "\\bvoid\\b" << "\\bvolatile\\b";
        foreach (const QString &pattern, keywordsPatterns) {
          rule.pattern = QRegExp(pattern);
          rule.format = m_keywordFormat;
          highLightRules.append(rule);
        }
      break;
      }
    case GIT_DIFF:
      {
        HighLightRule rule;
        QTextCharFormat diffLessFormat;
        diffLessFormat.setBackground(Qt::red);
        diffLessFormat.setFontWeight(QFont::Bold);
        QTextCharFormat diffMoreFormat;
        diffMoreFormat.setBackground(Qt::green);
        diffMoreFormat.setFontWeight(QFont::Bold);
        
        rule.pattern = QRegExp("^\\+.*");
//        rule.pattern.setMinimal(true);
        rule.format = diffMoreFormat;
        
        highLightRules.append(rule);

        rule.pattern = QRegExp("^-.*");
//        rule.pattern.setMinimal(true);
        rule.format = diffLessFormat;
        highLightRules.append(rule);
        break;
      }
    default:
      {}

  }
  m_highLightRulesHash.insert(mode, highLightRules);
}
 
void GHighLighter::setCurrentMode(Mode mode) {

  m_currentMode = mode;
}
void GHighLighter::highlightBlock(const QString& text) {
  QList<HighLightRule> highLightRules  = m_highLightRulesHash.value(m_currentMode);

  QList<HighLightRule>::const_iterator rule = highLightRules.constBegin();
  while (rule != highLightRules.constEnd()) {
    QRegExp expression((*rule).pattern);
    int index = expression.indexIn(text);;
    while ( index >= 0) {
     int length = expression.matchedLength();
     setFormat(index, length, (*rule).format);
     index = expression.indexIn(text, index + length);
    }
    rule ++;
  } 
}
