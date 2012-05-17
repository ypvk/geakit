#ifndef GEAKIT_GUI_GMAINWINDOW_H
#define GEAKIT_GUI_GMAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

#include <git2.h>


class QMenu;
class QAction;
class QCloseEvent;
class QListWidget;
class QPushButton;
class QToolBar;
class QWidget;
class QStackedWidget;
//class GCommitView;
//class GCodeView;
//class GBranchView;
class QListWidgetItem;

class GAccount;

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
    
    QListWidget* m_projectsOnline;
    QListWidget* m_projectsLocal;

    QPushButton* m_addButton;
    QPushButton* m_rmButton;

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

    //each view (real widget)
//    GCodeView* m_codeView;
//    GCommitView* m_commitView;
//    GBranchView* m_branchView;
    
    git_repository* m_currentRepo;
  
  private slots:
    void onSettingsActionTriggered();

    void onProjectsActionTriggered();
    void onCodeViewActionTriggered();
    void onBranchViewActionTriggered();
    void onCommitViewActionTriggered();

    void addProjectToLocal();
    void removeProjectInLocal();
    void openProject(QListWidgetItem* project);
    void updateView();
    void onBranchViewChanged();
  private:
    void setupMenus();
    void setupActions();
    void loadSettings();
    void saveSettings();
    void buildGui();
    void initProjectItems();
    void setupToolBar();
};

#endif
