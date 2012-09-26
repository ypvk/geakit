#ifndef GPROCESSDIALOG_H
#define GPROCESSDIALOG_H

#include <QDialog>

class QPushButton;
class GitCommand;
class QLabel;
class QTimer;

class GProcessDialog : public QDialog
{
  Q_OBJECT
  public:
    explicit GProcessDialog(QWidget* parent = 0);
    ~GProcessDialog();
    void setTitleName(const QString& title);
    void setContent(const QString& content);
    void setCommand(GitCommand* command);
  public slots:
    void show();
  private slots:
    void onCancelButtonClicked();
    void changeContent();
  signals:
    void canceled();
  private:
    QString m_content;
    QPushButton* m_cancelButton;
    QLabel* m_message;
    GitCommand* m_command;
    int m_times;
    QTimer* m_timer;
};
#endif /**GPROCESSDIALOG_H*/
