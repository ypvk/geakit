#include "gcodeview.h"
#include "gitcommand.h"

#include <QHeaderView>
#include <QDebug>
#include <QTreeWidget>
#include <QDir>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <string>
#include <iostream>
GCodeView::GCodeView(QWidget* parent, git_repository* repos) : QWidget(parent) 
{
  m_repos = repos;
  m_fileList = new QTreeWidget(this);
  m_gitAddButton = new QPushButton(tr("Add"), this);
  m_gitRmButton = new QPushButton(tr("Remove"), this);
  m_gitCommitButton = new QPushButton(tr("Commit"), this);

  QHBoxLayout* mainLayout = new QHBoxLayout(this);
  QVBoxLayout* buttonLayout = new QVBoxLayout;

  buttonLayout->addWidget(m_gitAddButton);
  buttonLayout->addWidget(m_gitRmButton);
  buttonLayout->addWidget(m_gitCommitButton);
  buttonLayout->addStretch(0.5);
  mainLayout->addWidget(m_fileList);
  mainLayout->addLayout(buttonLayout);
  connect(m_gitAddButton, SIGNAL(clicked()), this, SLOT(gitAdd()));
  connect(m_gitRmButton, SIGNAL(clicked()), this, SLOT(gitRm()));
  connect(m_gitCommitButton, SIGNAL(clicked()), this, SLOT(gitCommit()));

 // setLayout(mainLayout);

  m_fileList->setHeaderLabels(QStringList() << tr("Name") << tr("age") << tr("status"));
  m_fileList->header()->setResizeMode(QHeaderView::ResizeToContents);
  QString workdir(git_repository_workdir(m_repos));
  m_workdirRoot = workdir;
  m_tmpRoot = "";
  QDir dir(workdir);
  updateView(dir);
  connect(m_fileList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleCilcked(QTreeWidgetItem*, int)));  
  connect(m_fileList, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*, int)));

  m_command = new GitCommand(this, m_workdirRoot);

  git_index* index;
  git_index_entry* entry;

  git_repository_index(&index, m_repos);

  int error;
  //TODO something if error
  error =  git_index_read(index);
  int ecount = git_index_entrycount(index);

  for (int i = 0; i < ecount; i++) {
    entry = git_index_get(index, i);
  //  QListWidgetItem* file = new QListWidgetItem(m_fileList);
   // file->setText(QString(entry->path));
    unsigned int status_flags;
    error = git_status_file(&status_flags, m_repos, entry->path);
    qDebug() << status_flags;
    qDebug() << entry->path;
   // qDebug() << entry->mtime.seconds;
   // qDebug() << entry->file_size;
  }
  git_index_free(index);

  /******************test the odb and the tree*************************/
  git_tree* head_tree;
  //get the head commit
  git_reference* head; 
  error = git_repository_head(&head, m_repos);
  const git_oid* refoid = git_reference_oid(head);
  git_commit* m_commit;
  error = git_commit_lookup(&m_commit, m_repos, refoid);
  error = git_commit_tree(&head_tree, m_commit);
  int num = git_tree_entrycount(head_tree);
  for (int i = 0; i < num; i++) {
    const git_tree_entry* entry = git_tree_entry_byindex(head_tree, i);
    qDebug() << git_tree_entry_name(entry);
    git_otype otype = git_tree_entry_type(entry);
    if (otype == GIT_OBJ_TREE)
      qDebug() << " is subtree";
    qDebug() << "entry type is : " << git_object_type2string(git_tree_entry_type(entry));
  }
  git_commit_free(m_commit);
  git_tree_free(head_tree);
  git_reference_free(head);
  /*
  git_odb* odb;
  git_odb_object* obj;
  git_otype otype;

  char* data;
  const char* str_type;

  git_reference* head;//head reference or said tree
  error = git_repository_head(&head, m_repos);
  const git_oid* headOid = git_reference_oid(head);
  git_tree* m_tree;
  error = git_commit_tree(&m_tree, head);


  error = git_repository_odb(&odb, m_repos);

  error = git_odb_read(&obj, odb, headOid);
  data = (char*)git_odb_object_data(obj);
  otype = git_odb_object_type(obj);

  str_type = git_object_type2string(otype);

  qDebug() << "object length  and type is " << (int)git_odb_object_size(obj) << " " << str_type;
  git_reference_free(head);
  git_odb_object_free(obj);
  git_odb_free(odb);
  */
}  
  
GCodeView::~GCodeView() {
}

void GCodeView::gitAdd() {
  const QString cmdHead = "git add ";
  QList<QTreeWidgetItem*>::iterator it = m_selectedItems.begin();
  while(it != m_selectedItems.end()) {
    qDebug() << cmdHead + m_tmpRoot + "/" + (*it)->text(0);
    m_command->execute(cmdHead + m_tmpRoot + "/" + (*it)->text(0));
    ++it;
  }
  //emit the repository changed and update the data
  emit reposDataChanged();
  QDir dir(m_workdirRoot + m_tmpRoot);
  updateView(dir);
}
void GCodeView::gitRm() {
}
void GCodeView::gitCommit() {
}
void GCodeView::onItemDoubleCilcked(QTreeWidgetItem* item, int column) {
  QString name = item->text(3);
 // qDebug() << item->text(3);
  //double clicked the dir
  if (name == "dir") {
    QString dirName = item->text(0);
    if (dirName == "..") {
      QStringList tmpList = m_tmpRoot.split("/");
      if (tmpList.size() > 1) {
        m_tmpRoot.replace("/" + tmpList[tmpList.size() - 1], "");//remove the end 
      }
      else
        m_tmpRoot = "";
    }
    else {
      m_tmpRoot = m_tmpRoot == "" ? dirName : m_tmpRoot + "/" + dirName;
    }
    QDir dir(m_workdirRoot + m_tmpRoot);
    updateView(dir);
  }
}
void GCodeView::updateView(QDir& dir) {
  m_fileList->clear();
  m_selectedItems.clear();
  if (m_tmpRoot == "") {
    dir.setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
  }
  else {
    dir.setFilter(QDir::NoDot | QDir::AllDirs);
  } 
  QStringList dirList = dir.entryList();
  QIcon dirIcon(tr(":/icons/dir.png"));
  QIcon fileIcon(tr(":/icons/txt.png"));
  if (0 != dirList.size());
  {
    for (int i = 0; i < dirList.size(); i++)
    {
      QTreeWidgetItem* dirItem = new QTreeWidgetItem(m_fileList, QStringList() << dirList[i]);
      dirItem->setIcon(0, dirIcon);
      dirItem->setText(3, tr("dir"));
      dirItem->setCheckState(0, Qt::Unchecked);
      m_fileList->addTopLevelItem(dirItem);
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
          fileItem->setText(2, tr("untacked"));
          break;
        case GIT_STATUS_WT_MODIFIED :
          fileItem->setText(2, tr("Modifiled"));
          break;
        case GIT_STATUS_CURRENT :
          fileItem->setText(2, tr("Current"));
          break;
        case GIT_STATUS_INDEX_NEW :
          fileItem->setText(2, tr("index new"));
          break;
        case GIT_STATUS_INDEX_MODIFIED :
          fileItem->setText(2, tr("index modifiled"));
          break;
      }
      /*******************here we can't get tree and the item reluze later****************
      //get the index and the property
      git_index* fileIndex;
      qDebug() << "index is :" << m_workdirRoot + m_tmpRoot;
      error = git_index_open(&fileIndex, m_tmpRoot.toLocal8Bit().data());
      git_index_read(fileIndex);
      if (error < GIT_SUCCESS) {
        qDebug() << "error open the index";
      }
      qDebug() << "index count is : " << git_index_entrycount(fileIndex);
      git_reference* head;
      error = git_repository_head(&head, m_repos);
      if (error < GIT_SUCCESS) {
        qDebug() << "error";
      }
      const git_oid* headOid = git_reference_oid(head);
      //now get the tree
      git_tree* m_tree;
      git_commit* headCommit;
      error = git_commit_lookup(&headCommit, m_repos, headOid);
      if (error < GIT_SUCCESS) {
        qDebug() << "error 22";
      }
      error = git_commit_tree(&m_tree, headCommit);
      if (error < GIT_SUCCESS) {
        qDebug() << "error 33";
      }
      int num = git_tree_entrycount(m_tree);
      for (int i = 0; i < num; i++) {
       const git_tree_entry* git_entry = git_tree_entry_byindex(m_tree, i);
       qDebug() << git_tree_entry_name(git_entry);
       qDebug() << git_tree_entry_type(git_entry);
      }
      git_commit_free(headCommit);
      git_tree_free(m_tree);
      git_reference_free(head);
//      qDebug() << "index count";
//      qDebug() << git_index_entrycount(fileIndex);
//      git_index_entry* indexEntry = git_index_get(fileIndex, 0);
      //qDebug() << indexEntry->path;
      git_index_free(fileIndex);
      delete[] path;
      *******************************************************/
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
