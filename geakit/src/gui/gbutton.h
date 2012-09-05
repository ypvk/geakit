#ifndef GBUTTON_H
#define GBUTTON_H
#include <QPushButton>

class GButton : public QPushButton
{
  Q_OBJECT
  public:
  explicit GButton(QWidget* parent = 0, const QString& branchName = "");
  ~GButton();
  private slots:
  void onButtonClicked();
  signals:
  void mButtonClicked(const QString& branchName);
  private:
  QString m_branchName;
};
#endif
