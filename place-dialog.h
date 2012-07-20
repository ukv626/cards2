#ifndef PLACEDIALOG_H
#define PLACEDIALOG_H

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

class PlaceDialog : public QDialog
{
    Q_OBJECT

public:
  PlaceDialog(QWidget *parent = 0);
  ~PlaceDialog();
  QString getText() const;
    
private slots:
    void OkButtonPushed();

private:
    QSqlTableModel *placeModel;
    QLabel *placeLabel;
    QComboBox *placeComboBox;
    
    QDialogButtonBox *buttonBox;
};

#endif
