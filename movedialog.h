#ifndef MOVEDIALOG_H
#define MOVEDIALOG_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QSpinBox;
class QDateTimeEdit;
class QComboBox;
class QCheckBox;
class QDataWidgetMapper;
class QSqlRelationalTableModel;
class QSqlTableModel;
class QDialogButtonBox;

class MoveDialog : public QDialog
{
    Q_OBJECT

public:
  MoveDialog(int id, int detailId = 0, double qty = 0, int nr = 0, QWidget *parent = 0);
  ~MoveDialog();
  int getAmount();
    enum {
      Move_Id = 0,
      Move_DetailId = 1,
      Move_Date = 2,
      Move_PlaceId = 3,
      Move_Document = 4,
      Move_N = 5,
      Move_Type = 6,
      Move_Qty = 7,
      Move_Ost = 8
      //Move_Lim = 9
    };
    
private slots:
    void typeChanged();
    void OkButtonPushed();

private:
    QSqlRelationalTableModel *tableModel;
    QSqlTableModel *placeModel;
    QDataWidgetMapper *mapper;
    QLabel *dateLabel;
    QLabel *placeLabel;
    QLabel *documentLabel;
    QLabel *nLabel;
    QLabel *ostLabel;
    QLabel *typeLabel;
    QLabel *limLabel;

    QDateTimeEdit *dateTimeEdit;
    QComboBox *placeComboBox;
    QCheckBox *typeCheckBox;
    QLineEdit *documentEdit;
    QSpinBox *nSpinBox;
    QSpinBox *ostSpinBox;
    QSpinBox *limSpinBox;
    //QComboBox *comboType;
    
    QDialogButtonBox *buttonBox;

    int id_;
    int detailId_;
    double qty_;
    int nr_;
};

#endif
