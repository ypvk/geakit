#ifndef GITCOMMAND_H
#define GITCOMMAND_H

#include <QWidget>
#include <QProcess>

class GitCommand : public QWidget
{
  Q_OBJECT
  public:
    explicit GitCommand(QWidget* parent = 0, const QString& workDir = "");
    ~GitCommand();
    void execute(const QString& cmd);
   const QString& output() const;
  private slots:
    void redFromStdOut();
    void redFromStdErr();
    void processError(QProcess::ProcessError error);
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
  private:
    QString m_output;
    QString m_workDir;
    QProcess* m_process;
};
#endif
