#ifndef GPATCHVIEW_H
#define GPATCHVIEW_H

#include <QWidget>
#include <git2.h>
#include <QProcess>

//class QProcess;
class QTextEdit;
class GitCommand;
class GHighLighter;

class GPatchView : public QWidget
{
  Q_OBJECT
  public:
    explicit GPatchView (QWidget* parent = 0,git_repository* repo = 0);
    ~GPatchView();
    void setCommitOid(const QString& commitOid);
    void getDiff();
//  public slots:
//    void redFromStdOut();
//    void redFromStdErr();
//    void processError(QProcess::ProcessError error);
//    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
  private:
   // git_commit* m_commit;
    QString m_commitOid;
    git_repository* m_repo;
    GitCommand* m_command;
//    QProcess* m_process;
//    QString m_content;
    QTextEdit* m_text;
    GHighLighter* m_highLighter;
};
    
#endif
