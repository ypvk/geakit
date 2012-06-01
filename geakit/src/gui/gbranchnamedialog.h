#ifndef GBRANCHNAMEDIALOG_H
#define GBRANCHNAMEDIALOG_H
#include <QDialog>

class QLineEdit;
class QLabel;
class QPushButton;

class GBranchNameDialog : public QDialog
{
  Q_OBJECT
  public:
    explicit GBranchNameDialog(QWidget* parent = NULL);
    ~GBranchNameDialog();
    QString getBranchName();
  private slots:
    void onOkButtonClicked(); 
//    void onCloseButtonClicked();
  private:
    QPushButton* m_okButton;
    QPushButton* m_closeButton;
    QLabel* m_label;
    QLineEdit* m_lineEdit;
};
#endif
