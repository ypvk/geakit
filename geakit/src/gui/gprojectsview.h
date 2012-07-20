#ifndef GPROJECTSVIEW_H
#define GPROJECTSVIEW_H

#include <QWidget>
#include <QHash>
class QListWidget;
class QPushButton;
class GitCommand;

class GProjectsView : public QWidget {
 Q_OBJECT
  public:
    explicit GProjectsView ( QWidget* parent = 0);
    ~GProjectsView();
    void setProjectsLocalHash(const QHash<QString, QString>& projectsLocalHash);
    QHash<QString, QString> projectsLocalHash();
    void initProjectsItems(const QHash<QString, QString>& projectOnlineHash, const QString& type = "");
    void initProjectsLocal();
  public slots:
    void addProjectToLocal();
    void removeProjectInLocal();

  signals:
    void onWorking(const QString& message);
    void end();
  private:
    QPushButton* m_addButton;
    QPushButton* m_rmButton;

    QListWidget* m_projectsOnline;
    QListWidget* m_projectsLocal;

    QHash<QString, QString> m_projectsLocalHash;
    GitCommand* m_command;
}
#endif
