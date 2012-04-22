#include "ui_gsettingsdialog.h"

#include "gsettingsdialog.h"

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QAuthenticator>

const QString test_login_url = "https://api.github.com/user";

GSettingsDialog::GSettingsDialog(QWidget* parent):QDialog(parent), ui(new Ui::GSettingsDialog)
{
  ui->setupUi(this);
  m_manager = new QNetworkAccessManager(this);

  connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(onLoginButtonClicked()));
  connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(parseFinish(QNetworkReply*)));
  connect(this, SIGNAL(loginResult(bool, QString)), this, SLOT(processLoginResult(bool, QString)));
}

void GSettingsDialog::onLoginButtonClicked()
{
  ui->loginButton->setEnabled(false);
  ui->usernameEdit->setEnabled(false);
  ui->passwordEdit->setEnabled(false);
  ui->loginStatusLabel->setText(tr("Login ..."));

  QByteArray basic = QString("%1:%2").arg(ui->usernameEdit->text()).arg(ui->passwordEdit->text()).toAscii();

  QNetworkRequest request = QNetworkRequest(QUrl(test_login_url));
  request.setRawHeader("Authorization", QByteArray("Basic ") + basic.toBase64());

  m_manager->get(request);
}

void GSettingsDialog::processLoginResult(bool is_success, QString message){
  if(is_success){
    ui->loginStatusLabel->setText(tr("Login Successfully!"));
  }
  else{
    ui->loginStatusLabel->setText(tr("Login Failed!%1").arg(message));
  }
}

void GSettingsDialog::parseFinish(QNetworkReply* reply)
{
  ui->usernameEdit->setEnabled(true);
  ui->passwordEdit->setEnabled(true);
  ui->loginButton->setEnabled(true);

  QNetworkReply::NetworkError err = reply->error();
  switch(err){
    case QNetworkReply::NoError:
      {
        if(reply->url().path() == QUrl(test_login_url).path())
          emit loginResult(true, "");
        //Add more here!
        break;
      }
    default:
      {
        emit loginResult(false, reply->errorString());
        break;
      }
  }
}
