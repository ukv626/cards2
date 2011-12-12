#include <QLabel>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QGridLayout>
#include <QVBoxLayout>

#include "DatesInputDialog.h"

DatesInputDialog::DatesInputDialog(QWidget *parent)
  : QDialog(parent)
{
  date1Edit = new QDateEdit;
  date1Edit->setCalendarPopup(true);
  date1Edit->setDisplayFormat("dd.MM.yyyy");
  date1Label = new QLabel(trUtf8("Дата"));
  date1Label->setBuddy(date1Edit);

  date2Edit = new QDateEdit;
  date2Edit->setCalendarPopup(true);
  date2Edit->setDisplayFormat("dd.MM.yyyy");
  date2Label = new QLabel(trUtf8("Дата"));
  date2Label->setBuddy(date2Edit);
  
  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                   | QDialogButtonBox::Cancel);

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QGridLayout *topLayout = new QGridLayout;
  topLayout->addWidget(date1Label, 1, 0);
  topLayout->addWidget(date1Edit, 1, 1);
  topLayout->addWidget(date2Label, 2, 0);
  topLayout->addWidget(date2Edit, 2, 1);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(topLayout);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  setWindowTitle(tr("DatesInputDialog"));
}
