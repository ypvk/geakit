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

    QAction* m_projectAction;
    QSettings m_settings;

    git_config* m_config;
    GAccount* m_account;
    
    QListWidget* m_projectsOnline;
    QListWidget* m_projectsLocal;

    QPushButton* m_addButton;
    QPushButton* m_rmButton;

  
  private slots:
    void onSettingsActionTriggered();

    void onProjectActionTriggered();
    void addProjectToLocal();
    void removeProjectInLocal();
    void openProject(QListWidget* project);
  private:
    void setupMenus();
    void setupActions();
    void loadSettings();
    void saveSettings();
    void buildGui();
    void initProjectItems();
};

#endif
