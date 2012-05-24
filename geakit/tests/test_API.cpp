#include <api/gaccountapi.h>
#include <QNetworkAccessManager>
#include <QApplication>

int main (int argc, char* argv[]) {
  QApplication app(argc, argv);
  QNetworkAccessManager m_network;
  GAccountAPI api(&m_network);
  api.startConnect();
  return app.exec();
} 
