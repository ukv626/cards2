#ifndef PLACESDIALOG_H
#define PLACESDIALOG_H

#include <QDialog>

class QTableView;
class QSqlTableModel;

class PlacesDialog : public QDialog
{
    Q_OBJECT
public:
  PlacesDialog(QWidget *parent = 0);
  ~PlacesDialog();
  enum {
    Place_Id = 0,
    Place_Text = 1
  };

private slots:
  void insert();
  void remove();

private:
  //QLabel *label;
  //QLineEdit *lineEdit;
  //QComboBox *comboBox;
  QTableView *tableView;
  QSqlTableModel *tableModel;
};

#endif
