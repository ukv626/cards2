#include <QtGui>
#include <QtSql>

#include "movesdialog.h"
#include "movedialog.h"

MySqlRelationalTableModel::MySqlRelationalTableModel(QObject *parent)
  : QSqlRelationalTableModel(parent)
{
}

QVariant MySqlRelationalTableModel::data(const QModelIndex &index, int role) const
{
  QVariant value = QSqlQueryModel::data(index, role);
  switch (role) {
  case Qt::DisplayRole:
    if (index.column() == MoveDialog::Move_Date)
      return value.toDateTime().toString("dd.MM.yyyy hh:mm");
    else if (index.column() == MoveDialog::Move_Type) {
      if(!value.toBool())
        return trUtf8("Приход");
      else 
        return trUtf8("Расход");
    }
    else
      return value;

    
    case Qt::TextAlignmentRole: // Выравнивание
      if(index.column() == MoveDialog::Move_N ||
	 index.column() == MoveDialog::Move_Ost )
	return double(Qt::AlignRight | Qt::AlignVCenter);
      else
	return int(Qt::AlignLeft | Qt::AlignVCenter);
  }
  return value;
}

MovesDialog::MovesDialog(int detailId, double qty, int nr, QWidget *parent)
  : QDialog(parent), detailId_(detailId), qty_(qty), nr_(nr), needUpdate_(false)
{
  tableView = new QTableView;

  tableModel = new MySqlRelationalTableModel(this);
  tableModel->setTable("tb_moves");
  tableModel->setRelation(MoveDialog::Move_PlaceId, QSqlRelation("tb_places", "uid", "text"));
  tableModel->setFilter(QString("tb_moves.detailID=%1").arg(detailId_));
  tableModel->setSort(MoveDialog::Move_Date, Qt::AscendingOrder);
  tableModel->select();
  

  tableModel->setHeaderData(MoveDialog::Move_Date, Qt::Horizontal, trUtf8("Дата"));
  tableModel->setHeaderData(MoveDialog::Move_PlaceId, Qt::Horizontal, trUtf8("Назначение"));
  tableModel->setHeaderData(MoveDialog::Move_Document, Qt::Horizontal, trUtf8("Документ"));
  tableModel->setHeaderData(MoveDialog::Move_N, Qt::Horizontal, trUtf8("Кол-во"));
  tableModel->setHeaderData(MoveDialog::Move_Ost, Qt::Horizontal, trUtf8("Остаток"));
  tableModel->setHeaderData(MoveDialog::Move_Type, Qt::Horizontal, trUtf8("Тип"));

  /*
  proxyModel->setDynamicSortFilter(true);
  proxyModel->setSourceModel(queryModel);
  proxyModel->setFilterKeyColumn(Storages_StorageText);
  proxyModel->setFilterFixedString(comboBox->currentText());
  proxyModel->sort(Storages_ProductText, Qt::AscendingOrder);
  */
  
  tableView->setModel(tableModel);
  tableView->setItemDelegate(new QSqlRelationalDelegate(tableView));

  tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  
  tableView->setColumnHidden(MoveDialog::Move_Id, true);
  tableView->setColumnHidden(MoveDialog::Move_DetailId, true);
  tableView->setColumnHidden(MoveDialog::Move_Qty, true);
  
  tableView->verticalHeader()->hide();
  tableView->resizeColumnsToContents();
  tableView->setAlternatingRowColors(true);
  tableView->setCurrentIndex(tableView->model()->index(0, 0));

  QAction *newAction = new QAction(trUtf8("Добавить.."), this);
  connect(newAction, SIGNAL(triggered()), this, SLOT(insert()));
  
  QAction *editAction = new QAction(trUtf8("Редактировать.."), this);
  connect(editAction, SIGNAL(triggered()), this, SLOT(edit()));

  QAction *removeAction = new QAction(trUtf8("Удалить"), this);
  connect(removeAction, SIGNAL(triggered()), this, SLOT(remove()));

  tableView->addAction(newAction);
  tableView->addAction(editAction);
  tableView->addAction(removeAction);
  tableView->setContextMenuPolicy(Qt::ActionsContextMenu);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(tableView);
  setLayout(mainLayout);

  setWindowTitle(trUtf8("Движение"));
  setFixedWidth(tableView->horizontalHeader()->length()+50);
}


MovesDialog::~MovesDialog()
{
  delete tableModel;
  //delete proxyModel;
}

void MovesDialog::insert()
{
  MoveDialog dialog(-1, detailId_, qty_, nr_);
  if(dialog.exec() == QDialog::Accepted) {
    nr_ += dialog.getAmount();
    
    // QSqlQuery query;
    // query.prepare("UPDATE tb_details SET nr = :nr WHERE uid=:detailId");
    // query.bindValue(":nr", nr_);
    // query.bindValue(":detailId", detailId_);
    // query.exec();
    
    tableModel->select();
    needUpdate_ = true;
  }
}

void MovesDialog::edit()
{
  QAbstractItemModel *model = tableView->model();
  if(model->rowCount()>0) {

    int id = model->data(model->index(tableView->currentIndex().row(), 0)).toInt();
    
    MoveDialog dialog(id);
    if(dialog.exec() == QDialog::Accepted)
      tableModel->select();
  }
}

void MovesDialog::remove()
{
  int r = QMessageBox::warning(this, trUtf8("Подтверждение"),
	trUtf8("Действительно удалить запись?"),
	QMessageBox::Yes,
	QMessageBox::No | QMessageBox::Default | QMessageBox::Escape);
  
  if (r == QMessageBox::No)
    return;


}

/*
void MovesDialog::copy()
{
  QString selectedText;
  QAbstractItemModel *model = tableView->model();
  QItemSelectionModel *selection = tableView->selectionModel();
  QModelIndexList indexes = selection->selectedIndexes();
  QModelIndex previous = indexes.first();

  foreach(QModelIndex current, indexes) {
    QVariant data = model->data(current);
    
    if(current.row() != previous.row())
      selectedText.append("\n");
    
    selectedText.append(data.toString());
    selectedText.append(';');
    previous = current;
  }

  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(selectedText);
}

void MovesDialog::paste()
{
  QClipboard *clipboard = QApplication::clipboard();
  QString text = clipboard->text();
  QStringList list = text.split("\n");

  QModelIndex index=comboBox->model()->index(comboBox->currentIndex(),0);
  int currentUnitId=comboBox->model()->data(index).toInt();
  bool needUpdate = false;
   
  foreach(QString row, list) {
    QStringList columns = row.split(";");

    if(columns.size()!=8)
      return;

    // проверить на перемещение самому себе
    if(currentUnitId == columns[Storages_UnitId].toInt())
      return;

    bool OkButton;
    int n = QInputDialog::getInteger(this, trUtf8("Перемещение"),
		trUtf8("Со склада: <b>")+columns[Storages_StorageText]+"</b><br>"+
		trUtf8("Наименование: <b>")+columns[Storages_ProductText]+"</b><br>"+
		trUtf8("Остаток: <b>")+columns[Storages_Amount]+"</b>",
		1, 1, columns[5].toInt(), 1, &OkButton);

    if(OkButton) {
      // Проверяем наличие перемещаемого продукта на принимаемом складе
      QSqlQuery query;
      query.prepare("SELECT id FROM tb_storages "
		    "WHERE unitid=:unitId AND productid=:productId");
      query.bindValue(":unitId", currentUnitId);
      query.bindValue(":productId", columns[Storages_ProductId]);
      query.exec();

      if(query.next()) {
	// уже есть
	int id=query.value(0).toInt();
	query.prepare("UPDATE tb_storages SET amount=amount+:n WHERE id=:id");
	query.bindValue(":n", n);
	query.bindValue(":id", id);
	query.exec();
      }
      else {
	// еще нет
	query.prepare("INSERT INTO tb_storages(unitid, productid, amount, amountBr) "
		      "VALUES(:unitId, :productId, :amount, :amountBr)");
	query.bindValue(":unitId", currentUnitId);
	query.bindValue(":productId", columns[Storages_ProductId]);
	query.bindValue(":amount", n);
	query.bindValue(":amountBr", n);
	query.exec();
      }

      // уменьшить кол-во на складе-источнике
      query.prepare("UPDATE tb_storages SET amount=amount-:n, amountBr=amountBr-:n WHERE id=:id");
      query.bindValue(":n", n);
      query.bindValue(":id", columns[Storages_Id].toInt());
      query.exec();

      needUpdate = true;
    } // OkButton
    
  } // foreach
  
  // обновить представление
  if(needUpdate)
    queryModel->setQuery(queryModel->query().lastQuery());
}
*/

