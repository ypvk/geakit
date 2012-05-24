#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QDebug>
#include "gaccountapi.h"

const QString url = "https://api.github.com/user";//api url
const int timeToRun = 1000;//time to connect or run the api;

GAccountAPI::GAccountAPI(QNetworkAccessManager* manager)
{
  m_manager = manager;
  m_parser = new QJson::Parser;
  m_serializer = new QJson::Serializer;
  m_timeout = new QTimer(this);

  connect(m_timeout, SIGNAL(timeout()), this, SLOT(onTimeout()));
  connect(m_manager, SIGNAL(finished(QNetWorkReply*)), this, SLOT(parseFinished(QNetWorkReply*)));
    
}
void GAccountAPI::parseFinished(QNetworkReply* reply) {
    

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
