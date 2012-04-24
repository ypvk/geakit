#ifndef GEAKIT_GUI_GSETTINGSDIALOG_H
#define GEAKIT_GUI_GSETTINGDSIALOG_H

#include <QDialog>
#include <QString>

class QNetworkAccessManager;
class QNetworkReply;

class GAccount;

namespace Ui
{
  class GSettingsDialog;
}

class GSettingsDialog :public QDialog
{
  Q_OBJECT

  public:
    explicit GSettingsDialog(GAccount* account, QWidget* parent = 0);
    ~GSettingsDialog();

    GAccount* account();

  signals:
    void loginResult(bool is_success, QString message);

  private:
    Ui::GSettingsDialog* ui;
    QNetworkAccessManager* m_manager;
    GAccount* m_account;

  private slots:
    void onLoginButtonClicked();
    void processLoginResult(bool is_success, QString message);
    void parseFinish(QNetworkReply* reply); //根据reply 分配给不同的函数处理
};

#endif
