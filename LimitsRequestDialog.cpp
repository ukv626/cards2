#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QGridLayout>
#include <QVBoxLayout>

#include "LimitsRequestDialog.h"

LimitsRequestDialog::LimitsRequestDialog(QWidget *parent)
  : QDialog(parent)
{
  limNumber = new QLineEdit;
  limNumberLabel_ = new QLabel(trUtf8("Лимитная карта"));
  limNumberLabel_->setBuddy(limNumber);
  
  technics = new QComboBox;
  technics->addItems(QStringList() << trUtf8("CAT140H")
		     << trUtf8("CAT345DL")
		     << trUtf8("D275A") 
		     << trUtf8("D355A")
		     << trUtf8("D355C")
		     << trUtf8("D9N")
		     << trUtf8("D9R")
		     << trUtf8("EX400")
		     << trUtf8("PC400-7")
		     << trUtf8("ZX330")
		     << trUtf8("Шантуй"));
			 
  technics->setEditable(true);

  technicsLabel_ = new QLabel(trUtf8("Марка Техники"));
  technicsLabel_->setBuddy(technics);

  technicsNumber = new QLineEdit;
  technicsNumberLabel_ = new QLabel(trUtf8("№ Техники"));
  technicsNumberLabel_->setBuddy(technicsNumber);

  place = new QLineEdit;
  placeLabel_ = new QLabel(trUtf8("Дислокация"));
  placeLabel_->setBuddy(place);
  
  buttonBox_ = new QDialogButtonBox(QDialogButtonBox::Ok
                                   | QDialogButtonBox::Cancel);

  connect(buttonBox_, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox_, SIGNAL(rejected()), this, SLOT(reject()));

  QGridLayout *topLayout = new QGridLayout;
  topLayout->addWidget(limNumberLabel_, 1, 0);
  topLayout->addWidget(limNumber, 1, 1);
  topLayout->addWidget(technicsLabel_, 2, 0);
  topLayout->addWidget(technics, 2, 1);
  topLayout->addWidget(technicsNumberLabel_, 3, 0);
  topLayout->addWidget(technicsNumber, 3, 1);
  topLayout->addWidget(placeLabel_, 4, 0);
  topLayout->addWidget(place, 4, 1);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(topLayout);
  mainLayout->addWidget(buttonBox_);
  setLayout(mainLayout);

  setWindowTitle(trUtf8("Дополнительная информация"));
}
