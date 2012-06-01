#ifndef GREPOSITORYAPI_H
#define GREPOSITORYAPI_H
#include <QObject>
#include <QHash>
#include <parser.h>
#include <serializer.h>

class QNetworkAccessManager;
class QNetworkReply;
class QTimer;

class GRepositoryAPI : public QObject
{
  Q_OBJECT
  public:
    typedef enum {SUCCESSFUL, ERROR, TIMEOUT} ResultCode;
  public:
    explicit GRepositoryAPI(QNetworkAccessManager* manager = 0);
    ~GRepositoryAPI();
    void startConnect();
    void setUsername(const QString& username);
    int getReposNum() const;
    QHash<QString, QString> getRepos() const;
    
  private slots:
    void parseFinished(QNetworkReply* reply);
    void onTimeout(); 

  signals:
    void complete(GRepositoryAPI::ResultCode result);
  private:
    QNetworkAccessManager* m_manager;
    QJson::Parser* m_parser;
    QJson::Serializer* m_serializer;
    QTimer* m_timeout;
    QString m_username;
    int m_reposNum;
    QHash<QString, QString> m_repos;
};
#endif/*GREPOSITORY_H*/
