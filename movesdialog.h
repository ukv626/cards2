#ifndef MOVESDIALOG_H
#define MOVESDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QSqlRelationalTableModel>

class QTableView;

class MySqlRelationalTableModel : public QSqlRelationalTableModel {
     Q_OBJECT
 public:
     MySqlRelationalTableModel(QObject *parent = 0);
     QVariant data(const QModelIndex &index,
                   int role = Qt::DisplayRole) const;
 };

class MovesDialog : public QDialog
{
    Q_OBJECT

public:
  MovesDialog(int detailId, double qty, double nr, QWidget *parent = 0);
  ~MovesDialog();
  bool isDirty() const { return needUpdate_; }
  double getNr() const { qDebug() << nr_; return nr_; }
    
private slots:
    void insert();
    void edit();
    void remove();

private:
    QTableView *tableView;
    MySqlRelationalTableModel *tableModel;
    int detailId_;
    double qty_;
    double nr_;
    bool needUpdate_;
};

#endif
