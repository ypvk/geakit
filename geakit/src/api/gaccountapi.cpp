#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QDebug>
#include "gaccountapi.h"

const QString url = "https://api.github.com/users/ypvk";//api url
const int timeToRun = 11000;//time to connect or run the api;

GAccountAPI::GAccountAPI(QNetworkAccessManager* manager)
{
  m_manager = manager;
  m_parser = new QJson::Parser;
  m_serializer = new QJson::Serializer;
  m_timeout = new QTimer(this);

  connect(m_timeout, SIGNAL(timeout()), this, SLOT(onTimeout()));
  connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(parseFinished(QNetworkReply*)));
    
}
void GAccountAPI::parseFinished(QNetworkReply* reply) {
  QNetworkReply::NetworkError err = reply->error();
  switch (err) {
    case QNetworkReply::NoError:
      {
        QString json = reply->readAll();
        qDebug() << json;
        bool ok;
        QVariantMap result = m_parser->parse(json.toAscii(), &ok).toMap();
        if (ok) {
          QList<QString> keys = result.keys();
          QList<QString>::const_iterator it = keys.constBegin();
          while (it != keys.constEnd()) {
            qDebug() << *it << ":" << result[*it].toString();
            it ++;
          }
        }
     //   qDebug() << reply->rawHeaderPairs();
        break;
      }
    default:
      qDebug() << "error is " << err;

  }  

  exit(0);
}

void GAccountAPI::startConnect() {
  m_timeout->start(timeToRun); 
  QNetworkRequest request = QNetworkRequest(QUrl(url));
  
  m_manager->get(request);
}
void GAccountAPI::onTimeout() {
  //todo something if the time is out
  m_manager->setNetworkAccessible(QNetworkAccessManager::NotAccessible);
  qDebug() << "can't get api message";
  exit(1);
}
void GAccountAPI::setNetManager(QNetworkAccessManager* manager)
{
  if(manager)
  {
    m_manager = manager;
        //Do some connect here
  }
}
GAccountAPI::~GAccountAPI() {
  delete m_parser;
  delete m_serializer;
}
