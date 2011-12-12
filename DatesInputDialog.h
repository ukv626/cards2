#ifndef DATESINPUTDIALOG_H
#define DATESINPUTDIALOG_H

#include <QDialog>

class QLabel;
class QDateEdit;
class QDialogButtonBox;

class DatesInputDialog : public QDialog
{
    Q_OBJECT

public:
  DatesInputDialog(QWidget *parent);
  
  QDateEdit *date1Edit;
  QDateEdit *date2Edit;
    
private:
  QLabel *date1Label;
  QLabel *date2Label;

  QDialogButtonBox *buttonBox;
};

#endif
