#include "ui_gsettingsdialog.h"

#include "gsettingsdialog.h"

#include <QString>
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
    connect(m_manager, SIGNAL(finished(QNetworkReply* reply)), this, SLOT(dispathc(QNetworkReply* reply)));
    connect(m_manager, SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)), this, SLOT(handleUnAuth(QNetworkReply*, QAuthenticator*)));
}

void GSettingsDialog::onLoginButtonClicked()
{
    QUrl url(test_login_url);
    QString userinfo =QString("%1:%2").arg(ui->usernameEdit->text()).arg(ui->passwordEdit->text());
    url.setUserInfo(userinfo);
    m_manager->get(QNetworkRequest(url));
}

void GSettingsDialog::handleUnAuth(QNetworkReply* reply, QAuthenticator* authenticator)
{
  if(authenticator->user().isEmpty() || authenticator->password().isEmpty())
  {
    authenticator->setUser(ui->usernameEdit->text());
    authenticator->setPassword(ui->passwordEdit->text());
  }
  else
  {
        ui->loginStatusLabel->setText(tr("Login Failed!"));
        ui->loginButton->setEnabled(true);
  }
}

void GSettingsDialog::dispatch(QNetworkReply* reply)
{
    QUrl url = reply->request().url();
    if (url.path() == QUrl(test_login_url).path())
    {
        ui->loginStatusLabel->setText(tr("Login Successfully!"));
        ui->loginButton->setEnabled(false);
    }
}
