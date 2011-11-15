#ifndef GEAKIT_API_ACCOUNT_H
#define GEAKIT_API_ACCOUNT_H

#include <QObject>

class QNetworkAccessManager;

class GAccountAPI: public QObject
{
	Q_OBJECT
	public:
		explicit GAccountAPI(QNetworkAccessManager* manager = 0);
        ~GAccountAPI(){}

		QNetworkAccessManager* netManager(){return m_manager;}
		void setNetManager(QNetworkAccessManager* manager);

	signals:
		void incompleteInstance(); //如果networkaccessmanager等私有变量没有初始化， 再调用相关API的时候会触发此信号

	private:
		QNetworkAccessManager* m_manager;
};

#endif
