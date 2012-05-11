#include "gbutton.h"

GButton::GButton(QWidget* parent, int id) : QPushButton(parent), m_id(id)
{
}

GButton::~GButton() {
}
void GButton::showId() {
  emit buttonId(m_id);
}
