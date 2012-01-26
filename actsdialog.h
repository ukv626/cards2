#ifndef ACTSDIALOG_H
#define ACTSDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>

class QLabel;
class QLineEdit;
//class QComboBox;
class QTableView;
class QSqlQueryModel;
class QSortFilterProxyModel;


class ActsModel : public QSqlQueryModel
{
  Q_OBJECT
    
public:
  ActsModel(QObject *parent = 0);
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const;
};


class ActsDialog : public QDialog
{
    Q_OBJECT

public:
  ActsDialog(qint8 typeId, QWidget *parent = 0);
  ~ActsDialog();
  enum {
    Act_Date = 0,
    Act_Text = 1,
    Act_Customer = 2,
    Act_Count = 3,
    Act_Sum = 4
  };

private slots:
    void filterRegExpChanged();
    void view();

private:
  QLabel *findLabel_;
  QLineEdit *findEdit_;
  //QComboBox *comboBox;
  QTableView *tableView_;
  ActsModel *queryModel_;
  QSortFilterProxyModel *proxyModel_;
};

#endif
