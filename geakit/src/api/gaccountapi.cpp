#include <QNetworkAccessManager>

#include "gaccountapi.h"

GAccountAPI::GAccountAPI()
{
	m_username = m_pasword = "";
	m_manager = 0;
}

GAccountAPI::GAccountAPI(QString username, QString password)
{
	m_username = username;
	m_password = password;
	m_manager = 0;
}

void GAccountAPI::requestLogin()
{
	if(m_manager == 0 || m_username.is_empty() || m_password.is_empty())
	{
		emit incompleteInstance();
	}
	else
		m_manager->get(QNetworkRequest(QUrl(test_login_url)));
}

void GAccountAPI::setNetManager(QNetworkAccessManager* mananger)
{
	if(manager)
	{
		m_manager = manager;
		connect(m_manager, SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)), this, SLOT(handleUnAuth(QNetworkReply*, QAuthenticator*)));
	}
}

void GAccountAPI::handleUnAuth(QNetworkReply* reply, QAuthenticator* authenticator)
{
	if(authenticator->user().isEmpty())
	{
		authenticator->setUser(m_username);
		authenticator->setPassword(m_password);
	}
	else
	{
		emit authResult(false);
	}
}
