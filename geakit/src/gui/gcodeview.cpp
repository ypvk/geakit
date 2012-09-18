#include "gcodeview.h"
#include "gitcommand.h"
#include "gcommitdialog.h"
#include "gcodevieweditor.h"

#include <QHeaderView>
#include <QPointer>
#include <QDebug>
#include <QTreeWidget>
#include <QDir>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QLineEdit>
#include <QGroupBox>
#include <QComboBox>


/*************here define a new status that file delete in the index, but still on the disk******/
#define MY_GIT_STATUS_DELETED   12

GCodeView::GCodeView(QWidget* parent, git_repository* repos) : QWidget(parent)
{
  m_repos = repos;
  m_fileList = new QTreeWidget(this);
  m_path = new QLabel(tr("Path:"), this);
  m_currentDir = new QLineEdit(this);
  m_gitAddButton = new QPushButton(tr("Add"), this);
  m_gitRmButton = new QPushButton(tr("Remove"), this);
  m_gitReverseButton = new QPushButton(tr("Reverse"), this);
  m_gitCommitButton = new QPushButton(tr("Commit"), this);
  m_splitter = new QSplitter(this);
  m_editor = new GCodeViewEditor(this);
  m_contentArea = new QGroupBox(tr("content"), this);
  m_branches = new QComboBox(this);

  QHBoxLayout* groupBoxLayout = new QHBoxLayout;
  groupBoxLayout->addWidget(m_editor);
  m_contentArea->setLayout(groupBoxLayout);

  m_splitter->addWidget(m_fileList);
  m_splitter->addWidget(m_contentArea);
  m_splitter->setOrientation(Qt::Vertical);

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  QHBoxLayout* pathLayout = new QHBoxLayout;
  QHBoxLayout* buttonLayout = new QHBoxLayout;
  QHBoxLayout* bottomLayout = new QHBoxLayout;

  pathLayout->addWidget(m_path);
  pathLayout->addWidget(m_currentDir);

  buttonLayout->addWidget(m_branches);
  buttonLayout->addStretch(0.5);
  buttonLayout->addWidget(m_gitAddButton);
  buttonLayout->addWidget(m_gitRmButton);
  buttonLayout->addWidget(m_gitReverseButton);
  buttonLayout->addWidget(m_gitCommitButton);

  bottomLayout->addWidget(m_splitter);
  //bottomLayout->addLayout(buttonLayout);

  mainLayout->addLayout(pathLayout);
  mainLayout->addLayout(buttonLayout);
  mainLayout->addLayout(bottomLayout);

  connect(m_gitAddButton, SIGNAL(clicked()), this, SLOT(gitAdd()));
  connect(m_gitRmButton, SIGNAL(clicked()), this, SLOT(gitRm()));
  connect(m_gitCommitButton, SIGNAL(clicked()), this, SLOT(gitCommit()));
  connect(m_gitReverseButton, SIGNAL(clicked()), this, SLOT(gitReverse()));


  m_fileList->setHeaderLabels(QStringList() << tr("Name") << tr("status"));
  m_fileList->header()->setDefaultSectionSize(400);
  QString workdir(git_repository_workdir(m_repos));
  m_workdirRoot = workdir;
  m_tmpRoot = "";
  QDir dir(workdir);
  updateView(dir);
  connect(m_fileList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleCilcked(QTreeWidgetItem*, int)));
  connect(m_fileList, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*, int)));

  m_command = new GitCommand(this, m_workdirRoot);
  m_command->setRepository(m_repos);
  m_commitOid = m_command->gitHeadCommitOid();

  QStringList branches = m_command->gitBranches();
  m_branches->addItems(branches);
  QString currentBranch = m_command->gitRefHead();
  int index = branches.indexOf(currentBranch);
  m_branches->setCurrentIndex(index);
  connect(m_branches, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeToBranch(QString)));

}
GCodeView::~GCodeView() {
}

void GCodeView::gitAdd() {
  QStringList fileNames;
  QList<QTreeWidgetItem*>::iterator it = m_selectedItems.begin();
  while(it != m_selectedItems.end()) {
    if ((*it)->data(0, Qt::WhatsThisRole).toString() == "dir") {
      qDebug() << "is dir";
      gitAddDirectory((*it)->text(0));
    }
    else {
      QString path = m_tmpRoot == "" ? (*it)->text(0) : (m_tmpRoot + "/" + (*it)->text(0));
      fileNames << path;
    }
    ++ it;
  }
  if (fileNames.size() > 0) {
    m_command->gitAdd(fileNames);
  }
  QString path = m_workdirRoot + m_tmpRoot;
  QDir dir(path);
  updateView(dir);
}
void GCodeView::gitAddDirectory(const QString& dirName) {
  //build the dir if is file than add to index
  QString dirString = m_tmpRoot == "" ? (m_workdirRoot + dirName) : (m_workdirRoot + m_tmpRoot + "/" + dirName);

  dirString = QDir::toNativeSeparators(dirString);
  qDebug() << dirString;
  QDir dir(dirString);
  if (!dir.exists()) return;
  dir.setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
  QStringList dirList = dir.entryList();
  QStringList::const_iterator it = dirList.constBegin();
  while (it != dirList.constEnd()) {
    gitAddDirectory(dirName + "/" + (*it));
    it ++;
  }
  dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
  QStringList fileAddList;
  QStringList fileList = dir.entryList();
  it = fileList.constBegin();
  while ( it != fileList.constEnd()) {
    //first get the path
    QString path = m_tmpRoot == "" ? (dirName + "/" + (*it)) : (m_tmpRoot + "/" + dirName + "/" + (*it));
    fileAddList << path;
    qDebug() << path;
    it ++;
  }
  m_command->setRepository(m_repos);
  m_command->gitAdd(fileAddList);
}
void GCodeView::gitRm() {
  /************delete first the thing in the entry, after commit delete the entry int the working dir***********/
  git_index* m_index;
  int error;
  error = git_repository_index(&m_index, m_repos);
  QList<QTreeWidgetItem*>::iterator it = m_selectedItems.begin();
  while(it != m_selectedItems.end()) {
    //first get the path
    char* path;
    if (m_tmpRoot == "") {
      QString filePath = (*it++)->text(0);
      qDebug() << filePath;
      path = new char[filePath.size() + 1];
      strcpy(path, filePath.toLocal8Bit().constData());
    }
    else {
      QString filePath = QDir::toNativeSeparators(m_tmpRoot + "/" + (*it++)->text(0));
      path = new char[filePath.size() + 1];
      strcpy(path, filePath.toLocal8Bit().constData());

  }
    qDebug() << "selected path is " << path;
    int error;
    int position;
    position = git_index_find(m_index, path);
    qDebug() << "position is: " << position;
    if (position >= 0) {
      error = git_index_remove(m_index, position);
      if (error < GIT_SUCCESS)qDebug() << "add failue";
    }
    error = git_index_write(m_index);
    delete[] path;
  }
  git_index_free(m_index);
  QString path = QDir::toNativeSeparators(m_workdirRoot + m_tmpRoot);
  QDir dir(path);
  updateView(dir);
}
void GCodeView::gitCommit() {
  //get commit message
  QString commitMessage;
  GCommitDialog* dlg = new GCommitDialog;
  if (dlg->exec()) {
    if ("" == dlg->message()) {
      qDebug() << "empty message";
      delete dlg;
      return;
    }
    commitMessage = dlg->message();
    delete dlg;
  }
  else {
    qDebug() << "unaccepted quit the commit!";
    delete dlg;
    return;
  }
  qDebug() << QString("messags is %1").arg(commitMessage);
  //first get the new tree
  /************index => tree*****************/
  int error;
  git_oid oid;
  git_index* m_index;
  git_tree* m_tree;
  git_commit* m_commit;

  //get the tree oid and then commit
  error = git_repository_index(&m_index, m_repos);
  error = git_tree_create_fromindex(&oid, m_index);
  error = git_tree_lookup(&m_tree, m_repos, &oid);
  error = git_oid_fromstr(&oid, m_commitOid.toLocal8Bit().constData());
  error = git_commit_lookup(&m_commit, m_repos, &oid);
  //get the author signature
  git_config* m_config;
  const char* userName;
  const char* userEmail;
  error = git_config_open_global(&m_config);
  error = git_config_get_string(m_config, "user.name", &userName);
  error = git_config_get_string(m_config, "user.email", &userEmail);
  git_signature* author_signature;

  error = git_signature_now(&author_signature, userName, userEmail);
  //create commit
  const git_commit* parents[] = {m_commit};
  error = git_commit_create(
      &oid,
      m_repos,
      "HEAD", //update the HEAD
      author_signature,
      author_signature,
      NULL,
      commitMessage.toLocal8Bit().constData(),
      m_tree,
      1,
      parents
      );
  char oidStr[41] = {0};
  git_oid_fmt(oidStr, &oid);
  qDebug() << "commit oid is: " << oidStr;
  /************update the commit oid*****************/
  m_commitOid = QString(oidStr);
  /******************end****************************/

  git_index_free(m_index);
  git_index_write(m_index);
  git_config_free(m_config);
  git_signature_free(author_signature);
  git_commit_free(m_commit);
  git_index_free(m_index);
  git_tree_free(m_tree);
  //update the view(status)
  /** now delete the file on the disk if the file is deleted in the indexed**/
  QDir dirDelete(m_workdirRoot);
  if (0 != m_filesToDelete.size() ) {
    QStringList::const_iterator it = m_filesToDelete.constBegin();
    while (it != m_filesToDelete.constEnd()) {
      dirDelete.remove(*it++);
    }
 }
  QDir dir(QDir::toNativeSeparators(m_workdirRoot + m_tmpRoot));
  updateView(dir);
  /*********emit newCommit() signal*****************/
  emit newCommit();
}
void GCodeView::onItemDoubleCilcked(QTreeWidgetItem* item, int column) {
  QString name = item->text(2);
 // qDebug() << item->text(3);
  //double clicked the dir
  if (name == "dir") {
    QString dirName = item->text(0);
    if (dirName == "..") {
      int size = m_tmpRoot.split("/").size();
      if (size > 1) {
        m_tmpRoot = m_tmpRoot.section('/', 0, -2);
      }
      //QStringList tmpList = m_tmpRoot.split("/");
      //if (tmpList.size() > 1) {
        //m_tmpRoot.replace("/" + tmpList[tmpList.size() - 1], "");//remove the end
      //}
      else
        m_tmpRoot = "";
    }
    else {
      m_tmpRoot = m_tmpRoot == "" ? dirName : m_tmpRoot + "/" + dirName;
    }
    QDir dir(QDir::toNativeSeparators(m_workdirRoot + m_tmpRoot));
    updateView(dir);
  }
  else {
    QString fileName = item->text(0);
//    GCodeViewEditor* m_editor = new GCodeViewEditor();
    QString path = m_tmpRoot == "" ? m_workdirRoot + fileName : m_workdirRoot + m_tmpRoot + "/" + fileName;
    path = QDir::toNativeSeparators(path);
    m_editor->loadFile(path);
    m_contentArea->setTitle(fileName);
    //m_editor->show();
  }
}
void GCodeView::updateView(QDir& dir) {
  //m_fileList->clear();
  freeTreeWidget(m_fileList);
  m_selectedItems.clear();
  m_filesToDelete.clear();
  dir.setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
  QIcon dirIcon(tr(":/icons/dir.png"));
  QIcon fileIcon(tr(":/icons/txt.png"));
  if (m_tmpRoot != "") {
    QTreeWidgetItem* dirItem = new QTreeWidgetItem(m_fileList, QStringList() << "..");
    dirItem->setIcon(0, dirIcon);
    dirItem->setText(2, tr("dir"));
    dirItem->setData(0, Qt::WhatsThisRole, QString("dir"));
  }
  m_currentDir->setText(QDir::toNativeSeparators("/" + m_tmpRoot));
  QStringList dirList = dir.entryList();
  if (0 != dirList.size());
  {
    for (int i = 0; i < dirList.size(); i++)
    {
      QTreeWidgetItem* dirItem = new QTreeWidgetItem(m_fileList, QStringList() << dirList[i]);
      dirItem->setIcon(0, dirIcon);
      dirItem->setText(2, tr("dir"));
      dirItem->setCheckState(0, Qt::Unchecked);
      dirItem->setData(0, Qt::WhatsThisRole, QString("dir"));
    }
  }
  dir.setFilter(QDir::Files);
  QStringList fileList = dir.entryList();
  if (0 != fileList.size()) {
    for (int i = 0; i < fileList.size(); i ++)
    {
      QTreeWidgetItem* fileItem = new QTreeWidgetItem(m_fileList, QStringList() << fileList[i]);
      fileItem->setIcon(0, fileIcon);
      fileItem->setCheckState(0, Qt::Unchecked);
      m_fileList->addTopLevelItem(fileItem);
      //get the file status in git repository
      char* path;
     // qDebug() << fileList[i].toAscii().data();
     // path = m_tmpRoot == "" ? (fileList[i]).toAscii().data() : (m_tmpRoot + "/" + fileList[i]).toAscii().data();
      if (m_tmpRoot == "") {
        QByteArray strTmp = fileList[i].toLocal8Bit();
        path = new char[strTmp.size() + 1];
        path[strTmp.size()] = '\0';
        strcpy(path, strTmp.data());
      }
      else {
        QByteArray strTmp = (m_tmpRoot + "/" + fileList[i]).toLocal8Bit();
        path = new char[strTmp.size() + 1];
        path[strTmp.size()] = '\0';
        strcpy(path, strTmp.data());
      }
      int error;
      unsigned int status_flags;
     // qDebug() << "path is " << path;
      error = git_status_file(&status_flags, m_repos, path);
     qDebug() << path << "flags" << status_flags;
      switch(status_flags)
      {
        case GIT_STATUS_WT_NEW :
          fileItem->setText(1, tr("untacked"));
          break;
        case GIT_STATUS_WT_MODIFIED :
          fileItem->setText(1, tr("Modifiled, not update"));
          break;
        case GIT_STATUS_CURRENT :
          fileItem->setText(1, tr("Current"));
          break;
        case GIT_STATUS_INDEX_NEW :
          fileItem->setText(1, tr("new file, not commit"));
          break;
        case GIT_STATUS_INDEX_MODIFIED :
          fileItem->setText(1, tr("modifiled, not commit"));
          break;
        case MY_GIT_STATUS_DELETED :
          fileItem->setText(1, tr("deleted, not commit"));
          m_filesToDelete << path;
          break;
      }
      delete[] path;
    }
  }
}
void GCodeView::onItemClicked(QTreeWidgetItem* item, int column)
{
  if (column == 0) {
   if (Qt::Checked == item->checkState(0)) {
     m_selectedItems << item;
   }
   else {
     m_selectedItems.removeOne(item);
   }
  }
  /*
  QList<QTreeWidgetItem*>::const_iterator it = m_selectedItems.constBegin();
  while(it != m_selectedItems.constEnd()) {
    qDebug() << "item is:" << (*it++)->text(0);
  }
  */
}
void GCodeView::freeTreeWidget(QTreeWidget* treeWidget) {
  int size = treeWidget->topLevelItemCount();
  for (int i = 0; i < size; i++) {
    QTreeWidgetItem* item = treeWidget->takeTopLevelItem(0);
    delete item;
  }
}
void GCodeView::gitReverse() {
  m_command->gitReverse();
  QString path = QDir::toNativeSeparators(m_workdirRoot + m_tmpRoot);
  QDir dir(path);
  updateView(dir);
}
void GCodeView::changeToBranch(const QString& branchName)
{
  qDebug() <<"change to branch: " << branchName;
  bool result = m_command->gitChangeBranch(branchName);
  if (!result) {
    qDebug() << "error change branch";
    return;
  }
  QDir dir(m_workdirRoot);
  m_tmpRoot.clear();
  updateView(dir);
  emit branchChanged();
  return;
}
void GCodeView::onBranchChanged()
{
  QDir dir(m_workdirRoot);
  m_tmpRoot.clear();
  updateView(dir);
}
