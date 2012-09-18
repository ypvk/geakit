#ifndef GPROJECTSVIEW_H
#define GPROJECTSVIEW_H

#include <QWidget>
#include <QHash>
class QListWidget;
class QListWidgetItem;
class QPushButton;
class GitCommand;
class GAccount;

class GProjectsView : public QWidget {
 Q_OBJECT
  public:
    explicit GProjectsView ( QWidget* parent = 0, GAccount* account = 0);
    ~GProjectsView();
    void setProjectsLocalHash(const QHash<QString, QString>& projectsLocalHash);
    void setProjectsOnlineHash(const QHash<QString, QString>& projectsOnlineHash);
    QHash<QString, QString> projectsLocalHash();
    QHash<QString, QString> projectsOnlineHash();
    void initProjectsItems(const QHash<QString, QString>& projectOnlineHash, const QString& type = "");
    void setProjectsOnlineEnabled(bool isEnable);
    void setProjectsLocalEnabled(bool isEnable);
  private slots:
    void addProjectToLocal();
    void removeProjectInLocal();
    void onItemDoubleClicked(QListWidgetItem* project);
    void onProcessFinished();
    void onRefreshButtonClicked();
  signals:
    void workingStatusChanged(const QString& status, const QString& message);
    void openProject(const QString& reposPath);
    void removeProject(const QString& reposPath);
    void refreshProjectsOnline();
  private:
    QPushButton* m_addButton;
    QPushButton* m_rmButton;
    QPushButton* m_refreshButton;

    QListWidget* m_projectsOnline;
    QListWidget* m_projectsLocal;

    QHash<QString, QString> m_projectsLocalHash;
    QHash<QString, QString> m_projectsOnlineHash;
    GitCommand* m_command;
    GAccount* m_account;
};
#endif
