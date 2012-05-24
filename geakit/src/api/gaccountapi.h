#ifndef GEAKIT_API_ACCOUNT_H
#define GEAKIT_API_ACCOUNT_H

#include <QObject>
#include <parser.h>
#include <serializer.h>

class QNetworkAccessManager;
class QNetworkReply;
//class QJson::Parser;
//class QJson::Serializer;
class QTimer;

class GAccountAPI: public QObject
{
  Q_OBJECT
  public:
    explicit GAccountAPI(QNetworkAccessManager* manager = 0);
    ~GAccountAPI();

    QNetworkAccessManager* netManager(){return m_manager;}
    void setNetManager(QNetworkAccessManager* manager);
    void startConnect();

  signals:
    void incompleteInstance(); //如果networkaccessmanager等私有变量没有初始化， 再调用相关API的时候会触发此信号
  private slots:
    void parseFinished(QNetworkReply* reply);
    void onTimeout();

  private:
    QNetworkAccessManager* m_manager;
    QJson::Parser* m_parser;
    QJson::Serializer* m_serializer;
    QTimer* m_timeout;//add timer to count the time
};

#endif
