#ifndef GEAKIT_GUI_GSETTINGSDIALOG_H
#define GEAKIT_GUI_GSETTINGDSIALOG_H

#include <QDialog>
#include <QString>

class QNetworkAccessManager;
class QNetworkReply;
class QAuthenticator;

namespace Ui
{
  class GSettingsDialog;
}

class GSettingsDialog :public QDialog
{
  Q_OBJECT

  public:
    explicit GSettingsDialog(QWidget* parent = 0);
    ~GSettingsDialog(){}

  signals:
    void loginResult(bool is_success, QString message);

  private:
    Ui::GSettingsDialog* ui;
    QNetworkAccessManager* m_manager;

  private slots:
    void onLoginButtonClicked();
    void processLoginResult(bool is_success, QString message);
    void parseFinish(QNetworkReply* reply); //根据reply 分配给不同的函数处理
};

#endif
