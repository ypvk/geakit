#ifndef GBUTTON_H
#define GBUTTON_H
#include <QPushButton>

class GButton : public QPushButton
{
  Q_OBJECT
  public:
  explicit GButton(QWidget* parent = 0, int id = 0);
  ~GButton();
signals:
  void buttonId(int id);
public slots:
  void showId();
  private:
  int m_id;
};
#endif
