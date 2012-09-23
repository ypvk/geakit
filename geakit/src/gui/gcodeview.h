#ifndef GDODEVIEW_H
#define GCODEVIEW_H

#include <QWidget>
#include <git2.h>
#include <QDir>
#include <QSettings>

class QTreeWidget;
class QPushButton;
class QTreeWidgetItem;
class QLabel;
class QSplitter;
class QLineEdit;
class QGroupBox;
class QComboBox;
class GitCommand;
class GCommitDialog;
class GCodeViewEditor;
class GAccount;

class GCodeView : public QWidget
{
  Q_OBJECT
  public:
    explicit GCodeView(QWidget* parent = 0, git_repository* repos = 0);
    void setAccount(GAccount* account);
    ~GCodeView();
  signals:
    void reposDataChanged();
    void newCommit();
    void branchChanged();
  private:
//    void searchFiles();
    void gitAddDirectory(const QString& dirName);
  private:
    void updateView(QDir& dir);
    void freeTreeWidget(QTreeWidget* treeWidget);
  public slots:
    void gitAdd();
    void gitRm();
    void gitCommit();
    void gitReverse();
    void onItemDoubleCilcked(QTreeWidgetItem* item, int column);
    void onItemClicked(QTreeWidgetItem* item, int column);
    void changeToBranch(const QString& branchName);
    void onBranchChanged();
  private:
    git_repository* m_repos;//repository
    QTreeWidget* m_fileList;
    QPushButton* m_gitAddButton;
    QPushButton* m_gitRmButton;
    QPushButton* m_gitReverseButton;//revers
    QPushButton* m_gitCommitButton;
    QString m_workdirRoot;//repos root
    QString m_tmpRoot;//current dir
    GitCommand* m_command;//command to do the git command
    QList<QTreeWidgetItem*> m_selectedItems;//selected items
    QStringList m_filesToDelete; //file delete after commit
    QSettings m_setting;//settings to save the modified tree
    QLabel* m_path;
    QLineEdit* m_currentDir;//current dir
    QSplitter* m_splitter;
    QGroupBox* m_contentArea;
    QComboBox* m_branches;
    GCodeViewEditor* m_editor;
    GAccount* m_account;
};
#endif
