#include <QNetworkAccessManager>

#include "gaccountapi.h"

GAccountAPI::GAccountAPI(QNetworkAccessManager* manager)
{
  m_manager = manager;
}

void GAccountAPI::setNetManager(QNetworkAccessManager* manager)
{
  if(manager)
  {
    m_manager = manager;
        //Do some connect here
  }
}
