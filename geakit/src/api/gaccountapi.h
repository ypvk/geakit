#ifndef GEAKIT_API_ACCOUNT_H
#define GEAKIT_API_ACCOUNT_H

#include <QObject>
#include <QString>

const QString test_login_url = "https://api.github.com/user";

class GAccountAPI: public QObject
{
	Q_OBJECT
	public:
		explicit GAccountAPI();
		explicit GAccountAPI(QString username, QString password);

		QString username(){return m_username;}
		void setUsername(QString username){m_username = username;}
		QString password(){return m_password;}
		void setPassword(QString password){m_password = password;}
		QNetworkAccessManager* netManager(){return m_manager;}
		void setNetManager(QNetworkAccessManager* manager);

		void requestLogin();

	signals:
		void incompleteInstance(); //如果用户名，密码或者networkaccessmanager对象为初始化，在调用相关API的时候会触发此信号
		void authResult(bool success);//认证失败，会触发此信号

	private:
		QString m_username;
		QString m_password;
		QNetworkAccessManager* m_manager;

	private slots:
		void handleUnAuth(QNetworkReply* reply, QAuthenticator* authenticator);
};

#endif
