#include "gbutton.h"

GButton::GButton(QWidget* parent, const QString& branchName) : QPushButton(parent), m_branchName(branchName)
{
  connect(this, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
}
void GButton::onButtonClicked()
{
  emit mButtonClicked(m_branchName);
}

GButton::~GButton() {
}
