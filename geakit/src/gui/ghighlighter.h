#ifndef GHIGHLIGHTER_H
#define GHIGHLIGHTER_H
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QHash>

class QTextDocument;

class GHighLighter : public QSyntaxHighlighter
{
  Q_OBJECT
  public:
    typedef enum {
                  CPP, C, JAVA, GIT_DIFF
    }Mode;
  public:
    explicit GHighLighter(QTextDocument* parent = 0);
    void setCurrentMode(Mode currentMode);
    ~GHighLighter() {}
  protected:
    void highlightBlock(const QString& text);
  private:
    void setupHighLightRules(Mode mode);
  private:
    typedef struct HighLightRule 
    {
      QRegExp pattern;
      QTextCharFormat format;
    } HighLightRule;
    Mode m_currentMode;//current mode
    QHash<Mode, QList<HighLightRule> > m_highLightRulesHash;

    QTextCharFormat m_keywordFormat;
    //QTextCharFormat classFormat;
};
#endif /*GHIGHTLIGHTER_H*/
