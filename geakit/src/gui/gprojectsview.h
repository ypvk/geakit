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
    QHash<QString, QString> projectsLocalHash();
    void initProjectsItems(const QHash<QString, QString>& projectOnlineHash, const QString& type = "");
    void setProjectsOnlineEnabled(bool isEnable);
    void setProjectsLocalEnabled(bool isEnable);
  private slots:
    void addProjectToLocal();
    void removeProjectInLocal();
    void onItemDoubleClicked(QListWidgetItem* project);
    void onProcessFinished();
  signals:
    void workingStatusChanged(const QString& status, const QString& message);
    void openProject(const QString& reposPath);
    void removeProject(const QString& reposPath);
  private:
    QPushButton* m_addButton;
    QPushButton* m_rmButton;

    QListWidget* m_projectsOnline;
    QListWidget* m_projectsLocal;

    QHash<QString, QString> m_projectsLocalHash;
    GitCommand* m_command;
    GAccount* m_account;
};
#endif
