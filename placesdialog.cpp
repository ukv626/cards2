#include <QtGui>
#include <QtSql>

#include "placesdialog.h"
#include "LimitsRequestDialog.h"


PlacesDialog::PlacesDialog(QWidget *parent)
  : QDialog(parent)
{
  tableModel = new QSqlTableModel;
  tableModel->setTable("tb_places");
  tableModel->setSort(Place_Text, Qt::AscendingOrder);
  tableModel->select();

  tableModel->setHeaderData(Place_Text, Qt::Horizontal, trUtf8("Наименование"));

  tableView = new QTableView;
  
  tableView->setModel(tableModel);

  tableView->setSelectionMode(QAbstractItemView::SingleSelection);
  tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  
  tableView->verticalHeader()->hide();
  tableView->resizeColumnsToContents();
  tableView->setAlternatingRowColors(true);
  tableView->setColumnHidden(Place_Id, true);

  tableView->resizeColumnsToContents();
  tableView->setCurrentIndex(tableView->model()->index(0, 0));

  QAction *newAction = new QAction(trUtf8("Добавить.."), this);
  connect(newAction, SIGNAL(triggered()), this, SLOT(insert()));
  
  // QAction *removeAction = new QAction(trUtf8("Удалить"), this);
  // connect(removeAction, SIGNAL(triggered()), this, SLOT(remove()));

  tableView->addAction(newAction);
  // tableView->addAction(removeAction);
  tableView->setContextMenuPolicy(Qt::ActionsContextMenu);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(tableView);
  setLayout(mainLayout);

  setWindowTitle(trUtf8("Назначения"));
  setFixedWidth(tableView->horizontalHeader()->length()+50);
}

PlacesDialog::~PlacesDialog()
{
  delete tableModel;
}

void PlacesDialog::insert()
{
  QItemSelectionModel *selection = tableView->selectionModel();
  int row = selection->selectedIndexes().first().row();

  QAbstractItemModel *model = tableView->model();
  QModelIndex index = model->index(row, Place_Text);
  model->insertRow(row+1);

  tableView->setCurrentIndex(index);
}

void PlacesDialog::remove()
{
  //qDebug() << tableModel->lastError();
}
