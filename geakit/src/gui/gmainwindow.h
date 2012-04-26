#ifndef GEAKIT_GUI_GMAINWINDOW_H
#define GEAKIT_GUI_GMAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

#include <git2.h>


class QMenu;
class QAction;
class QCloseEvent;

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
  
  private slots:
    void onSettingsActionTriggered();

    void onProjectActionTriggered();
  private:
    void setupMenus();
    void setupActions();
    void loadSettings();
    void saveSettings();
};

#endif
