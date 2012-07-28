#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QDebug>
#include "grepositoryapi.h"

static const QString url = "https://api.github.com/users/%1/repos";
static const int timeToRun = 10000;//10s to connect and run

GRepositoryAPI::GRepositoryAPI(QNetworkAccessManager* manager) : m_manager(manager) {
  m_parser = new QJson::Parser;
  m_serializer = new QJson::Serializer;
  m_timeout = new QTimer(this);

  connect(m_timeout, SIGNAL(timeout()), this, SLOT(onTimeout()));
  connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(parseFinished(QNetworkReply*)));
}

void GRepositoryAPI::parseFinished(QNetworkReply* reply) {
  m_timeout->stop();
  QNetworkReply::NetworkError err = reply->error();
  switch (err) {
    case QNetworkReply::NoError :
      {
        m_repos.clear();
        QString json = reply->readAll();
        //qDebug() << json;
        bool ok;
        QVariantList reposList = m_parser->parse(json.toAscii(), &ok).toList();
        if (ok) {
          //QVariantList reposList = result.toList();
          m_reposNum = reposList.size();
          foreach(QVariant repos, reposList) {
            QVariantMap reposMap = repos.toMap();
            m_repos.insert(reposMap["name"].toString(), reposMap["url"].toString());
            /*
            QList<QString> keys = reposMap.keys();
            QList<QString>::const_iterator it = keys.constBegin();
            while( it != keys.constEnd()) {
              qDebug() << (*it) << ":" << reposMap[*it].toString();
              it++;
            }
            */
          }
        }
        emit complete(GRepositoryAPI::SUCCESSFUL);
        break;
      }
    default:
      emit complete(GRepositoryAPI::ERROR);
      qDebug() << "error code is : " << err;
  }
//  exit(0);
}
void GRepositoryAPI::startConnect() {
  m_timeout->start(timeToRun);
  QNetworkRequest request = QNetworkRequest(QUrl(QString(url).arg(m_username)));

  m_manager->get(request);
}
void GRepositoryAPI::onTimeout() {
  m_manager->setNetworkAccessible(QNetworkAccessManager::NotAccessible);
  qDebug() << "can't get api message";
  emit complete(GRepositoryAPI::TIMEOUT);
  m_timeout->stop();
  //exit(1);
}
GRepositoryAPI::~GRepositoryAPI() {
  delete m_parser;
  delete m_serializer;
}
void GRepositoryAPI::setUsername(const QString& username) {
  m_username = username;
}
int GRepositoryAPI::getReposNum() const {
  return m_reposNum;
}
QHash<QString, QString> GRepositoryAPI::getRepos() const{
  return m_repos;
}
