#include <QLabel>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QApplication>
#include <QInputDialog>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QDebug>

#include <QtSql>

#include "place-dialog.h"

PlaceDialog::PlaceDialog(QWidget *parent)
  : QDialog(parent)
{
  placeComboBox = new QComboBox;
  placeLabel = new QLabel(trUtf8("Назначение"));
  placeLabel->setBuddy(placeComboBox);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                   | QDialogButtonBox::Cancel);

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(OkButtonPushed()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QGridLayout *topLayout = new QGridLayout;
  topLayout->addWidget(placeLabel, 0, 0);
  topLayout->addWidget(placeComboBox, 0, 1);

  placeModel = new QSqlTableModel(this);
  placeModel->setTable("tb_places");
  placeModel->sort(1, Qt::AscendingOrder);
  placeModel->select();
  //relModel->setFilter(QString("mflag=1"));

    //placeModel = new QSqlTableModel(this);
    //placeModel->setTable("tb_places");
    //placeModel->setFilter(QString("mflag=%1").arg(typeCheckBox->isChecked()));
    //placeModel->select();

  placeComboBox->setModel(placeModel);
  placeComboBox->setModelColumn(placeModel->fieldIndex("text"));
  placeComboBox->setCurrentIndex(-1);
  
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(topLayout);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  setWindowTitle(trUtf8("Поставщик"));
}


PlaceDialog::~PlaceDialog()
{
  //delete queryModel;
  //delete proxyModel;
}


void PlaceDialog::OkButtonPushed()
{
  accept();
}

QString PlaceDialog::getText() const {
  return placeComboBox->currentText();
}
