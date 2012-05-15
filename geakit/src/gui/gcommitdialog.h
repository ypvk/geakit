#ifndef GCOMMITDIALOG_H
#define GCOMMITDIALOG_H

#include <QDialog>
class QTextEdit;
class QPushButton;

class GCommitDialog : public QDialog
{
  Q_OBJECT
  public:
    explicit GCommitDialog(QWidget* parent = 0);
    QString message() const;
    ~GCommitDialog();
  private slots:
    void onOkButtonClicked();
  private:
    QTextEdit* m_message;
    QPushButton* m_okButton;
};
#endif
