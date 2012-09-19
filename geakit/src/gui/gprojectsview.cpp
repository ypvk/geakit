#include "gprojectsview.h"
#include "data/account.h"
#include "gitcommand.h"

#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDebug>
#include <QDir>

GProjectsView::GProjectsView (QWidget* parent, GAccount* account) : QWidget(parent), m_account(account)
{
  //init
  m_command = new GitCommand(this);

  m_addButton = new QPushButton(this);
  m_rmButton= new QPushButton(tr("delete"), this);
  m_refreshButton = new QPushButton(tr("refresh"), this);

  m_addButton->setIcon(QIcon(tr(":/icons/right.png")));
  //m_rmButton->setIcon(QIcon(tr(":/icons/left.png")));

  m_addButton->setIconSize(QSize(40, 40));
  //m_rmButton->setIconSize(QSize(40, 40));
  m_addButton->setFixedSize(QSize(37, 37));
  //m_rmButton->setFixedSize(QSize(37, 37));

  m_projectsOnline = new QListWidget(this);
  m_projectsLocal = new QListWidget(this);

  QHBoxLayout* mainLayout = new QHBoxLayout;
  QVBoxLayout* buttonLayout = new QVBoxLayout;
  QVBoxLayout* groupLayoutOnline = new QVBoxLayout;
  QVBoxLayout* groupLayoutLocal = new QVBoxLayout;

  buttonLayout->addWidget(m_addButton);
  //buttonLayout->addWidget(m_rmButton);

  QGroupBox* projectsOnlineBox = new QGroupBox(this);
  QGroupBox* projectsLocalBox = new QGroupBox(this);

  projectsOnlineBox->setTitle(tr("projectsOnline"));
  projectsLocalBox->setTitle(tr("projectsLocal"));

  groupLayoutOnline->addWidget(m_projectsOnline);
  groupLayoutOnline->addWidget(m_refreshButton);
  projectsOnlineBox->setLayout(groupLayoutOnline);

  groupLayoutLocal->addWidget(m_projectsLocal);
  groupLayoutLocal->addWidget(m_rmButton);
  projectsLocalBox->setLayout(groupLayoutLocal);

  mainLayout->addWidget(projectsOnlineBox);
  mainLayout->addLayout(buttonLayout);
  mainLayout->addWidget(projectsLocalBox);

  setLayout(mainLayout);

  //connects
  connect(m_addButton, SIGNAL(clicked()), this, SLOT(addProjectToLocal()));
  connect(m_rmButton, SIGNAL(clicked()), this, SLOT(removeProjectInLocal()));
  connect(m_refreshButton, SIGNAL(clicked()), this, SLOT(onRefreshButtonClicked()));

  connect(m_projectsLocal, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onItemDoubleClicked(QListWidgetItem*)));
  connect(m_command, SIGNAL(finishedProcess()), this, SLOT(onProcessFinished()));
}
void GProjectsView::addProjectToLocal() {
  if (NULL == m_account) {
    QMessageBox::information(this, tr("warning"), tr("Please set the right account first"));
    return;
  }
  emit workingStatusChanged(tr("start"), tr("clone the repository"));
  //start
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
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Project Dir Local"), tr("."));
    if (dirName == "") {
      qDebug() << "empty file name do nothing";
      return;
    }
    QString path = QDir::toNativeSeparators(dirName + "/" + projectName);
    //if the dir exits return false
    QDir test_dir(path);
    if (test_dir.exists()) {
      QMessageBox::warning(this, tr("dir exits"), tr("dir exists, please change it"));
      return;
    }
    //save the value
    if (m_projectsLocalHash.value(projectName) != "") {
      QMessageBox::warning(this, tr("warning"), tr("projects has been here, try another place"));
      return;
    }
    m_projectsLocalHash.insert(projectName, path);

    //now use git clone to clone the repository to local
    m_command->setWorkDir(dirName);
    //here use https://name@github.com/name/repos_name.git as the url;
    QString reposUrl = QString("https://%1@github.com/%1/%2.git").arg(m_account->username()).arg(projectName);
    m_command->gitClone(projectName, reposUrl);
    m_projectsLocal->setEnabled(false);
    QListWidgetItem* item = new QListWidgetItem(m_projectsLocal);
    QString tmpText = QString("%1:\t%2/%1").arg(projectName).arg(dirName);
    item->setText(tmpText);
    m_projectsLocal->addItem(item);
    it++;
  }
  //end
  //emit workingStatusChanged(tr("end"), "");
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
    emit removeProject(tmpList[1].trimmed());
    delete item;
    it++;
  }
}
void GProjectsView::setProjectsLocalHash(const QHash<QString, QString>& projectsLocalHash)
{
  m_projectsLocalHash = projectsLocalHash;
}
void GProjectsView::setProjectsOnlineHash(const QHash<QString, QString>& projectsOnlineHash)
{
  m_projectsOnlineHash = projectsOnlineHash;
}
QHash<QString, QString> GProjectsView:: projectsLocalHash()
{
  return m_projectsLocalHash;
}

QHash<QString, QString> GProjectsView::projectsOnlineHash()
{
  return m_projectsOnlineHash;
}

void GProjectsView::initProjectsItems(const QHash<QString, QString>& projectOnlineHash, const QString& type)
{
  QListWidget* projectsList = type == "online" ? m_projectsOnline : m_projectsLocal;
  projectsList->clear();
  QHash<QString, QString>::const_iterator it = projectOnlineHash.constBegin();
  while (it != projectOnlineHash.constEnd())
  {
    //qDebug() << it.key() << ":" << it.value();
    QListWidgetItem* projectItem = new QListWidgetItem(projectsList);
    QString text = QString("%1:\t%2").arg(it.key()).arg(it.value());
    projectItem->setText(text);
    it ++;
  }
}
void GProjectsView::onItemDoubleClicked(QListWidgetItem* project)
{
  QStringList tmpList = (project->text()).split("\t");
  QString reposPath = tmpList[1].trimmed();
  emit openProject(reposPath);
}
void GProjectsView::setProjectsOnlineEnabled(bool isEnable)
{
  m_projectsOnline->setEnabled(isEnable);
}
void GProjectsView::setProjectsLocalEnabled(bool isEnable)
{
  m_projectsLocal->setEnabled(isEnable);
}
void GProjectsView::onProcessFinished()
{
  m_projectsLocal->setEnabled(true);
  emit workingStatusChanged(tr("end"), "");

}
void GProjectsView::onRefreshButtonClicked()
{
  emit refreshProjectsOnline();
}
GProjectsView::~GProjectsView()
{
}
