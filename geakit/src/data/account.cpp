#include "account.h"

GAccount::GAccount(QString username, QString password, QString fullname, QString email){
  m_username = username;
  m_password = password;
  m_fullname = fullname;
  m_email = email;
}

void GAccount::copy(GAccount* other){
  setUsername(other->username());
  setPassword(other->password());
  setFullname(other->fullname());
  setEmail(other->email());
}
