#ifndef MOVEDIALOG_H
#define MOVEDIALOG_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QDoubleSpinBox;
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
  MoveDialog(qint32 id, qint32 detailId = 0, double qty = 0, double nr = 0, QWidget *parent = 0);
  ~MoveDialog();
  double getAmount();
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
    /* QLabel *limLabel; */

    QDateTimeEdit *dateTimeEdit;
    QComboBox *placeComboBox;
    QCheckBox *typeCheckBox;
    QLineEdit *documentEdit;
    QDoubleSpinBox *nSpinBox;
    QDoubleSpinBox *ostSpinBox;
    /* QSpinBox *limSpinBox; */
    //QComboBox *comboType;
    
    QDialogButtonBox *buttonBox;

    qint32 id_;
    qint32 detailId_;
    double qty_;
    double nr_;
};

#endif
