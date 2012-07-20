#ifndef GEAKIT_GUI_GMAINWINDOW_H
#define GEAKIT_GUI_GMAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
//#include <QProcess>

#include <api/grepositoryapi.h>

#include <git2.h>


class QMenu;
class QAction;
class QCloseEvent;
class QListWidget;
class QPushButton;
class QToolBar;
class QWidget;
class QStackedWidget;
class QNetworkAccessManager;
//class GCommitView;
//class GCodeView;
//class GBranchView;
class QListWidgetItem;

class GAccount;
class GitCommand;

namespace Ui
{
  class GMainWindow;
}

class GMainWindow :public QMainWindow
{
  Q_OBJECT
  public:
    explicit GMainWindow(QWidget* parent = 0);
    ~GMainWindow();
 
  protected:
    void closeEvent(QCloseEvent *event); 

  private:
    Ui::GMainWindow* ui;
    QMenu* m_editMenu;
    QAction* m_settingsAction;

    QSettings m_settings;

    git_config* m_config;
    GAccount* m_account;
    
//    QListWidget* m_projectsOnline;
//    QListWidget* m_projectsLocal;

//    QPushButton* m_addButton;
//    QPushButton* m_rmButton;

    QToolBar* m_toolBar;
    QAction* m_projectsAction;
    QAction* m_codeViewAction;
    QAction* m_branchViewAction;
    QAction* m_commitViewAction;
    
    QStackedWidget* m_widgets;
    QWidget* m_projectsWidget;
    QWidget* m_codeViewWidget;
    QWidget* m_branchViewWidget;
    QWidget* m_commitViewWidget; 
    
    QNetworkAccessManager* m_manager;
    GRepositoryAPI* m_reposAPI;
    GitCommand* m_command;
    QHash<QString, QString> m_projectsLocalHash;//project local ,key is name, value is thepath
    //each view (real widget)
//    GCodeView* m_codeView;
//    GCommitView* m_commitView;
//    GBranchView* m_branchView;
    
    git_repository* m_currentRepo;
//    QString m_latestUpdatedRepo;
  
  private slots:
    void onSettingsActionTriggered();

    void onProjectsActionTriggered();
    void onCodeViewActionTriggered();
    void onBranchViewActionTriggered();
    void onCommitViewActionTriggered();

    void addProjectToLocal();
    void removeProjectInLocal();
    //void openProject(QListWidgetItem* project);
    void openProject(const QString& reposWorkdir);
    void updateView();
    void onBranchViewChanged();
    void onAccessComplete(GRepositoryAPI::ResultCode resultCode);
    void onWorkingStatusChanged(const QString& status, const QString& message);
    void onProcessFinished(/*int exitCode, QProcess::ExitStatus exitStatus*/);
  private:
    void setupMenus();
    void setupActions();
    void loadSettings();
    void saveSettings();
    void buildGui();
    void initProjectItems();
    void setupToolBar();
    void freeWidgets();
    void resetConfigUrl();
};

#endif
