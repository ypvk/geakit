#ifndef GREMOTENAMEDIALOG_H
#define GREMOTENAMEDIALOG_H
#include <QDialog>
class QLabel;
class QLineEdit;
class QPushBtton;

class GRemoteNameDialog : public QDialog
{
  Q_OBJECT
  public:
    explicit GRemoteNameDialog(QWidget* parent = 0);
    ~GRemoteNameDialog(){}
    QString remoteName() const;
    QString remoteUrl() const;
  private slots:
    void onOkButtonClicked();
  private:
    QLabel* m_remoteNameLabel;
    QLabel* m_remoteUrlLabel;
    QLineEdit* m_remoteNameEdit;
    QLineEdit* m_remoteUrlEdit;
    QPushButton* m_okButton;
};

#endif /*GREMOTENAMEDIALOG_H*/
