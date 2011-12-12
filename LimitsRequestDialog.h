#ifndef LIMITSREQUESTDIALOG_H
#define LIMITSREQUESTDIALOG_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QComboBox;
class QDialogButtonBox;

class LimitsRequestDialog : public QDialog
{
    Q_OBJECT

public:
  LimitsRequestDialog(QWidget *parent);

  QLineEdit *limNumber;
  QComboBox *technics;
  QLineEdit *technicsNumber;
  QLineEdit *place;
    
private:
  QLabel *limNumberLabel_;
  QLabel *technicsLabel_;
  QLabel *technicsNumberLabel_;
  QLabel *placeLabel_;

  QDialogButtonBox *buttonBox_;
};

#endif
