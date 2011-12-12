#ifndef DETAILSWINDOW_H
#define DETAILSWINDOW_H

#include <QWidget>
#include <QStyledItemDelegate>
#include <QSqlRelationalTableModel>

class QLabel;
class QLineEdit;
class QComboBox;
class QTableView;
class QSortFilterProxyModel;
class QSqlRelationalTableModel;


class DetailsModel : public QSqlRelationalTableModel {
     Q_OBJECT
 public:
     DetailsModel(QObject *parent = 0);
     
     //Qt::ItemFlags flags(const QModelIndex &index) const;
     
     QVariant data(const QModelIndex &index,
                   int role = Qt::DisplayRole) const;
     
     //bool setData(const QModelIndex &index, const QVariant &value, int role);
     bool select();
 };


/*
// -- CurrencyFormatDelegate --------------------------------------
class ForNumbersDelegate : public QStyledItemDelegate
{
public:
  ForNumbersDelegate(QObject *parent = 0, bool isCurrency = false);

  virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
                     const QModelIndex &index) const;

  virtual QString displayText(const QVariant & value, const QLocale & locale ) const;
 private:
  bool isCurrency_;
};
*/

// -- DetailsWindow -----------------------------------------------
class DetailsWindow : public QWidget
{
    Q_OBJECT

public:
    DetailsWindow(QWidget *parent = 0);
    ~DetailsWindow();

    enum {
      Details_Id = 0,
      Details_CatNum = 1,
      Details_Text = 2,
      Details_Qty = 3,
      Details_Qty2 = 4,
      Details_N = 5,
      Details_NR = 6,
      Details_Place = 7,
      Details_Brandname = 8,
      Details_Comment = 9,
      Details_TypeId = 10
    };
    
private slots:
    void filterRegExpChanged();
    void typeChanged();
    void showMovesDialog();
    void newRow();
    void copyRow();
    /*
    void showStoragesDialog();
    void showAddRequestDialog();
    void showRequestsDialog();
    //void sort(int);
    */

private:
    QLabel *findLabel;
    QLabel *typeLabel;
    QLineEdit *findEdit;
    QComboBox *typeComboBox;
    QTableView *tableView;
    DetailsModel *relModel;
    //QSqlRelationalTableModel *relModel;
    QSortFilterProxyModel *proxyModel;
};

#endif
