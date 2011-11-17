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
  m_toAuth = false;

  connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(onLoginButtonClicked()));
  connect(ui->passwordEdit, SIGNAL(textEdited(QString)), this, SLOT(onAccountSetted(QString)));
  connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(parseFinish(QNetworkReply*)));
  connect(m_manager, SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)), this, SLOT(handleUnAuth(QNetworkReply*, QAuthenticator*)));
  connect(this, SIGNAL(loginResult(bool, QString)), this, SLOT(processLoginResult(bool, QString)));
}

void GSettingsDialog::onLoginButtonClicked()
{
  ui->loginButton->setEnabled(false);
  ui->usernameEdit->setEnabled(false);
  ui->passwordEdit->setEnabled(false);
  ui->loginStatusLabel->setText(tr("Login ..."));
  m_toAuth = true;
  m_manager->get(QNetworkRequest(QUrl(test_login_url)));
}

void GSettingsDialog::onAccountSetted(QString /*text*/)
{
  ui->loginButton->setText(tr("Test Login"));
  ui->loginButton->setEnabled(true);
}

void GSettingsDialog::handleUnAuth(QNetworkReply* reply, QAuthenticator* authenticator)
{
  if(m_toAuth)
  {
    authenticator->setUser(ui->usernameEdit->text());
    authenticator->setPassword(ui->passwordEdit->text());
    m_toAuth = false;
  }
  else
  {
    emit loginResult(false, tr("Incorrect password"));
  }
}

void GSettingsDialog::processLoginResult(bool is_success, QString message){
  if(is_success){
    ui->loginButton->setText(tr("Logged in"));
    ui->loginStatusLabel->setText(tr("Login Successfully!"));
  }
  else{
    ui->usernameEdit->setEnabled(true);
    ui->passwordEdit->setEnabled(true);
    ui->loginStatusLabel->setText(tr("Login Failed!%1").arg(message));
  }
}

void GSettingsDialog::parseFinish(QNetworkReply* reply)
{
  QNetworkReply::NetworkError err = reply->error();
  switch(err){
    case QNetworkReply::NoError:
      {
        if(reply->url().path() == QUrl(test_login_url).path())
          emit loginResult(true, "");
        //Add more here!
        break;
      }
    case QNetworkReply::TimeoutError:
      {
        emit loginResult(false, reply->errorString());
        break;
      }
    case QNetworkReply::AuthenticationRequiredError:
      {
        break;
      }
    default:
      {
        emit loginResult(false, reply->errorString());
        break;
      }
  }
}
