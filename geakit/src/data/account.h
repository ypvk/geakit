#ifndef GEAKIT_DATA_ACCOUNT_H
#define GEAKIT_DATA_ACCOUNT_H

#include <QString>

class GAccount
{
  public:
    explicit GAccount(QString username, QString password, QString fullname, QString email);
    explicit GAccount(){m_username = m_password = m_fullname = m_email = "";}
    ~GAccount(){}

    void setUsername(QString username){m_username = username;}
    QString username(){return m_username;}
    void setPassword(QString password){m_password = password;}
    QString password(){return m_password;}
    void setFullname(QString fullname){m_fullname = fullname;}
    QString fullname(){return m_fullname;}
    void setEmail(QString email){m_email = email;}
    QString email(){return m_email;}

  private:
    QString m_username;
    QString m_password;
    QString m_fullname;
    QString m_email;
};

#endif
