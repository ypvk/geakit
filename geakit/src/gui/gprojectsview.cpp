#include "gpojectsview.h"
#include "gitcommand"

#include <QPushButton>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDebug>

GProjectsView::GProjectsView (QWidget* parent) : QWidget(parent)
{
  //init
  m_command = new GitCommand(this);

  m_addButton = new QPushButton(this);
  m_rmButton= new QPushButton(this);

  m_addButton->setIcon(QIcon(tr(":/icons/right.png")));
  m_rmButton->setIcon(QIcon(tr(":/icons/left.png")));

  m_addButton->setIconSize(QSize(40, 40));
  m_rmButton->setIconSize(QSize(40, 40));
  m_addButton->setFixedSize(QSize(37, 37));
  m_rmButton->setFixedSize(QSize(37, 37));

  m_projectsOnline = new QListWidget(this);
  m_projectsLocal = new QListWidget(this);

  QHBoxLayout* mainLayout = new QHBoxLayout;
  QVBoxLayout* buttonLayout = new QVBoxLayout;
  QHBoxLayout* groupLayoutOnline = new QHBoxLayout;
  QHBoxLayout* groupLayoutLocal = new QHBoxLayout;

  buttonLayout->addWidget(m_addButton);
  buttonLayout->addWidget(m_rmButton);

  QGroupBox* projectsOnlineBox = new QGroupBox(this);
  QGroupBox* projectsLocalBox = new QGroupBox(this);

  projectsOnlineBox->setTitle(tr("projectsOnline"));
  projectsLocalBox->setTitle(tr("projectsLocal"));

  groupLayoutOnline->addWidget(m_projectsOnline);
  projectsOnlineBox->setLayout(groupLayoutOnline);

  groupLayoutLocal->addWidget(m_projectsLocal);
  groupLayoutLocal->setLayout(groupLayoutLocal);

  mainLayout->addWidget(projectsOnlineBox);
  mainLayout->addLayout(buttonLayout);
  mainLayout->addWidget(projectsLocalBox);

  setLayout(mainLayout);

  //connects
  connect(m_addButton, SIGNAL(clicked()), this, SLOT(addProjectToLocal()));
  connect(m_rmButton, SIGNAL(clicked()), this, SLOT(removeProjectInLocal()));

  connect(m_projectsLocal, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(openProject(QListWidgetItem*)));
}
void GProjectsView::addProjectToLocal() {
  emit onWorking(tr("clone the repository"));
  QList<QListWidgetItem* > selectedProjects = m_projectsOnline->selectedItems();
  QList<QListWidgetItem*>::iterator it = selectedProjects.begin();
  while (it != selectedProjects.end()) {
    //QListWidgetItem* item = m_projectsOnline->takeItem(m_projectsOnline->row(*it));
    QString text = (*it)->text();
    qDebug() << text;
    QRegExp rx = QRegExp("([^:]+)?:");
    int pos = text.indexOf(rx);
    if (pos < 0) return;
    QString projectName = rx.cap(1);
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Project Dir Local"), tr("/home/yuping/yp/git"));
    QString path = dirName + "/" + projectName;
    //save the value
    if (m_projectsLocalHash.value(projectName) != "") {
      QMessageBox::warning(this, tr("warning"), tr("projects has been here, try another place"));
      return;
    }
    m_projectsLocalHash.insert(projectName, path);

    //now use git clone to clone the repository to local
    m_command->setWorkDir(dirName);
    //here use https://name@github.com/name/repos_name.git as the url;
    QString reposUrl = QString("https://%1:%3@github.com/%1/%2.git").arg(m_account->username()).arg(projectName).arg(m_account->password());
    //reposUrl = reposUrl + projectName + ".git";
    qDebug() << reposUrl;
    QString cmd = QString("git clone %1").arg(reposUrl);
    qDebug() << cmd;
    //add tips
    this->statusBar()->showMessage(tr("clone the repos"));
    //m_command->setCmd(cmd);
    //do it asycronize
    m_command->setWaitTime(0);
    m_command->execute(cmd);
    m_projectsLocal->setEnabled(false);
    //add the latestUpdateRepos String to reset the remote url in the config file(remove the password)
    m_latestUpdatedRepo = projectName;
    //qDebug() << m_command->output();
   // this->statusBar()->showMessage("");
    QListWidgetItem* item = new QListWidgetItem(m_projectsLocal);
    QString tmpText = QString("%1:\t%2/%1").arg(projectName).arg(dirName);
    item->setText(tmpText);
    m_projectsLocal->addItem(item);
    it++;
  }
  emit end();
}
void GProjectsView::removeProjectInLocal() {
  QList<QListWidgetItem* > selectedProjects = m_projectsLocal->selectedItems();
  QList<QListWidgetItem* >::iterator it = selectedProjects.begin();
  while ( it != selectedProjects.end()) {
    int reply = QMessageBox::question(this, tr("warning"), tr("Do you really want to delete the project?"), QMessageBox::Ok, QMessageBox::No);
    if ( QMessageBox::No == reply) {
      qDebug() << "Don't remove it";
      return;
    }
    QListWidgetItem* item = m_projectsLocal->takeItem(m_projectsLocal->row(*it));
    //m_projectsOnline->addItem(item);
    QString projectString = item->text();
    QStringList tmpList = projectString.split(":");
    qDebug() << tmpList[0];
    m_projectsLocalHash.remove(tmpList[0]);
    bool result = m_command->removeGitDir(tmpList[1].trimmed()); 
    qDebug() << "remove result:" << result;
    /*******also can remove the projects on disk***************/
    delete item;
    it++; 
  }
}
void GProjectsView::setProjectLocalHash(const QHash<QString, QString>& projectLocalHash)
{
  m_projectsLocalHash = projectsLocalHash;
}
QHash projectLocalHash() 
{
  return m_projectsLocalHash;
}

void GProjectsView::initProjectsItems(const QHash<QString, QString>& projectOnlineHash, const QString& type)
{
  QListWidget* projectsList = type == "online" ? m_projectsOnline : m_projectsLocal;
  QHash<QString, QString>::const_iterator it = projectOnlineHash.constBegin();
  while (it != projectOnlineHash.constEnd())
  {
    qDebug() << it.key() << ":" << it.value();
    QListWidgetItem* projectItem = new QListWidgetItem(projectsList);
    QString text = QString("%1:\t%2").arg(it.key()).arg(it.value());
    projectItem->setText(text);
    it ++;
  }
}
