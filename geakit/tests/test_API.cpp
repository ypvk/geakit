#include <api/gaccountapi.h>
#include <api/grepositoryapi.h>
#include <QNetworkAccessManager>
#include <QApplication>

int main (int argc, char* argv[]) {
  QApplication app(argc, argv);
  QNetworkAccessManager m_network;
//  GAccountAPI api(&m_network);
  GRepositoryAPI api2(&m_network);
  //api.startConnect();
  
  api2.startConnect();
  return app.exec();
} 
