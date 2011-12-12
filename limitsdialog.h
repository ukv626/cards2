#ifndef LIMITSDIALOG_H
#define LIMITSDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>

//class QLabel;
//class QLineEdit;
//class QComboBox;
class QTableView;
class QSqlQueryModel;
//class QSortFilterProxyModel;


class LimitsModel : public QSqlQueryModel
{
  Q_OBJECT
    
public:
  LimitsModel(QObject *parent = 0);
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const;
};


class LimitsDialog : public QDialog
{
    Q_OBJECT

public:
  LimitsDialog(QWidget *parent = 0);
  ~LimitsDialog();
  enum {
    Limit_Date = 0,
    Limit_Text = 1,
    Limit_Customer = 2,
    Limit_Count = 3,
    Limit_Sum = 4
  };

private slots:
    //void filterStringChanged();
    void printPreview();
    void print(QPrinter * printer);
    //void showPrintDialog();

private:
  //QLabel *label;
  //QLineEdit *lineEdit;
  //QComboBox *comboBox;
  QTableView *tableView;
  LimitsModel *queryModel;
  //QSortFilterProxyModel *proxyModel;

  QString html;
};

#endif
