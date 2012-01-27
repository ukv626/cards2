#include <QtGui>
#include <QtSql>

#include "detailswindow.h"
#include "movesdialog.h"


DetailsModel::DetailsModel(QObject *parent)
  : QSqlRelationalTableModel(parent)
{
}

// Qt::ItemFlags DetailsModel::flags(const QModelIndex &index) const
// {
//   Qt::ItemFlags flags = QSqlRelationalTableModel::flags(index);
//   //flags |= Qt::ItemIsEditable;

//   return flags;
// }

bool DetailsModel::select()
{
  bool result = QSqlRelationalTableModel::select();
  if(result) {
    while (canFetchMore()) fetchMore();
    return true;
  }
  else return false;
}

QVariant DetailsModel::data(const QModelIndex &index, int role) const
{
  QVariant value = QSqlRelationalTableModel::data(index, role);
  switch (role) {
  case Qt::DisplayRole:
    if (index.column() == DetailsWindow::Details_Qty ||
	index.column() == DetailsWindow::Details_Qty2)
      return tr("%1").arg(value.toDouble(), 0, 'f', 2);
    else
      return value;

    case Qt::TextAlignmentRole: // Выравнивание
      if(index.column() == DetailsWindow::Details_Qty ||
	 index.column() == DetailsWindow::Details_Qty2 ||
	 index.column() == DetailsWindow::Details_N ||
	 index.column() == DetailsWindow::Details_NR)
	return double(Qt::AlignRight | Qt::AlignVCenter);
      else
	return int(Qt::AlignLeft | Qt::AlignVCenter);
  }
  return value;
}


// bool DetailsModel::setData(const QModelIndex &index, const QVariant &value, int role)
// {
//   return QSqlRelationalTableModel::setData(index, value, role);
// }




/*
// -- ForNumbersDelegate --------------------------------------
ForNumbersDelegate::ForNumbersDelegate(QObject *parent, bool isCurrency)
: QStyledItemDelegate(parent), isCurrency_(isCurrency)
{
}

void ForNumbersDelegate::paint(QPainter *painter,
				   const QStyleOptionViewItem &option,
				   const QModelIndex &index) const
{
  QStyleOptionViewItem opt=option;
  opt.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
  QStyledItemDelegate::paint(painter, opt, index);
}

QString ForNumbersDelegate::displayText(const QVariant & value,
                                            const QLocale & locale ) const
{
  Q_UNUSED(locale);
  if(isCurrency_)
    return tr("%1").arg(value.toDouble(), 0, 'f', 2);
  else
    return value.toString();
}
// ----------------------------------------------------------------
*/


// -- DetailsWindow -----------------------------------------------
DetailsWindow::DetailsWindow(QWidget *parent)
  : QWidget(parent)
{
  findLabel = new QLabel(trUtf8("&Поиск"));
  findEdit = new QLineEdit;
  findLabel->setBuddy(findEdit);

  typeLabel = new QLabel(trUtf8("&Тип склада"));
  typeComboBox = new QComboBox;
  typeComboBox->addItems(QStringList() << trUtf8("Запчасти") << trUtf8("ДЗВ")
			 << trUtf8("Хранение") << trUtf8("Спец. одежда")
			 << trUtf8("Разное"));
  typeLabel->setBuddy(findEdit);

  connect(typeComboBox, SIGNAL(currentIndexChanged(QString)),
          this, SLOT(typeChanged()), Qt::UniqueConnection);
    
  QHBoxLayout *topLayout = new QHBoxLayout;
  topLayout->addWidget(findLabel);
  topLayout->addWidget(findEdit);
  topLayout->addWidget(typeLabel);
  topLayout->addWidget(typeComboBox);
  
  //topLayout->addStretch();
    
  relModel = new DetailsModel;
  relModel->setTable("tb_details");
  //relModel->setRelation(Client_TypeId, QSqlRelation("tb_types", "id", "text"));
  //relModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
  relModel->setSort(Details_Text, Qt::AscendingOrder);
  relModel->setFilter(QString("tb_details.typeId=%1").arg(typeComboBox->currentIndex()));
  relModel->select();
  
  relModel->setHeaderData(Details_CatNum, Qt::Horizontal, trUtf8("Каталожный\nномер"));
  relModel->setHeaderData(Details_Text, Qt::Horizontal, trUtf8("Наименование"));
  relModel->setHeaderData(Details_Qty, Qt::Horizontal, trUtf8("Цена\nучетная"));
  relModel->setHeaderData(Details_Qty2, Qt::Horizontal, trUtf8("Цена\nпродажная"));
  relModel->setHeaderData(Details_N, Qt::Horizontal, trUtf8("Кол-во"));
  relModel->setHeaderData(Details_NR, Qt::Horizontal, trUtf8("В\nналичии"));
  relModel->setHeaderData(Details_Place, Qt::Horizontal, trUtf8("Место"));
  relModel->setHeaderData(Details_Brandname, Qt::Horizontal, trUtf8("Марка"));
  relModel->setHeaderData(Details_Comment, Qt::Horizontal, trUtf8("Комментарий"));

  proxyModel = new QSortFilterProxyModel;
  //proxyModel->setDynamicSortFilter(true);
  proxyModel->setSourceModel(relModel);
  proxyModel->setFilterKeyColumn(-1);
  //proxyModel->sort(Details_Text, Qt::AscendingOrder);

  connect(findEdit, SIGNAL(textChanged(QString)),
  	  this, SLOT(filterRegExpChanged()), Qt::UniqueConnection);

  tableView = new QTableView;
  tableView->setModel(proxyModel);

  tableView->setItemDelegate(new QSqlRelationalDelegate(tableView));
  tableView->setSelectionMode(QAbstractItemView::SingleSelection);
  tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableView->setColumnHidden(Details_Id, true);
  tableView->setColumnHidden(Details_TypeId, true);

  // tableView->setItemDelegateForColumn(Details_Qty, new ForNumbersDelegate(this, true));
  // tableView->setItemDelegateForColumn(Details_Qty2, new ForNumbersDelegate(this, true));
  // tableView->setItemDelegateForColumn(Details_N, new ForNumbersDelegate(this));
  // tableView->setItemDelegateForColumn(Details_NR, new ForNumbersDelegate(this));

  //tableView->setItemDelegateForColumn(Client_TypeId, new ClientTypeDelegate(this));
  //tableView->resizeRowsToContents();

  QAction *newRowAction = new QAction(trUtf8("Новая"), this);
  connect(newRowAction, SIGNAL(triggered()), this, SLOT(newRow()));

  QAction *copyRowAction = new QAction(trUtf8("Копия"), this);
  connect(copyRowAction, SIGNAL(triggered()), this, SLOT(copyRow()));

  QAction *showMovesAction = new QAction(trUtf8("Движение.."), this);
  connect(showMovesAction, SIGNAL(triggered()), this, SLOT(showMovesDialog()));

  tableView->addAction(newRowAction);
  tableView->addAction(copyRowAction);
  tableView->addAction(showMovesAction);
  
  tableView->setContextMenuPolicy(Qt::ActionsContextMenu);
  tableView->setAlternatingRowColors(true);
  
  tableView->sortByColumn(Details_Text, Qt::AscendingOrder);
  tableView->setSortingEnabled(true);
  
  //tableView->setEditTriggers(QAbstractItemTableView::NoEditTriggers);

  tableView->verticalHeader()->hide();
  tableView->resizeColumnsToContents();
  tableView->setColumnWidth(Details_CatNum, 160);
  tableView->setColumnWidth(Details_Text, 250);
  tableView->setColumnWidth(Details_Place, 80);
  tableView->setColumnWidth(Details_Brandname, 130);
  tableView->horizontalHeader()->setStretchLastSection(true);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(topLayout);
  mainLayout->addWidget(tableView);
  setLayout(mainLayout);

  setWindowTitle(tr("Details"));
}


DetailsWindow::~DetailsWindow()
{
  delete proxyModel;
  delete relModel;
}

void DetailsWindow::filterRegExpChanged()
{
  QRegExp regExp(findEdit->text());
  proxyModel->setFilterRegExp(regExp);
  proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
}


void DetailsWindow::typeChanged()
{
  relModel->setFilter(QString("tb_details.typeId=%1").arg(typeComboBox->currentIndex()));
  relModel->select();

  emit typeChangedSig(typeComboBox->currentIndex());
}

qint8 DetailsWindow::getCurTypeId()
{
  return typeComboBox->currentIndex();
}

void DetailsWindow::newRow()
{
  QItemSelectionModel *selection = tableView->selectionModel();
  int row = selection->selectedIndexes().first().row();

  QAbstractItemModel *model = tableView->model();
  QModelIndex index = model->index(row, Details_CatNum);
  model->insertRow(row + 1);
  model->setData(model->index(row +1, Details_TypeId), typeComboBox->currentIndex());
  model->submit();

  tableView->setCurrentIndex(index);
}


void DetailsWindow::copyRow()
{
  QItemSelectionModel *selection = tableView->selectionModel();
  int row = selection->selectedIndexes().first().row();

  //QSqlRecord record = relModel->record();
  QAbstractItemModel *model = tableView->model();
  /*
  record.setValue(Details_CatNum, model->data(model->index(row, Details_CatNum)));
  record.setValue(Details_Text, model->data(model->index(row, Details_Text)));
  record.setValue(Details_Qty, model->data(model->index(row, Details_Qty)));
  record.setValue(Details_Qty2, model->data(model->index(row, Details_Qty2)));
  record.setValue(Details_Brandname, model->data(model->index(row, Details_Brandname)));
  */
  QModelIndex index = model->index(row, Details_Text);
  
  model->insertRow(row + 1);
  model->setData(model->index(row + 1, Details_CatNum),
		 model->data(model->index(row, Details_CatNum)));
  model->setData(model->index(row + 1, Details_Text),
		 model->data(model->index(row, Details_Text)));
  model->setData(model->index(row + 1, Details_TypeId), typeComboBox->currentIndex());
   		 
  // model->setData(model->index(row+1, Details_Qty2),
  // 		 model->data(model->index(row, Details_Qty2)));
  model->setData(model->index(row + 1, Details_Brandname),
		 model->data(model->index(row, Details_Brandname)));
  model->submit();

  tableView->setCurrentIndex(index);
}

/*
void DetailsWindow::showStoragesDialog()
{
  StoragesDialog dialog;
  dialog.exec();
}
*/

void DetailsWindow::showMovesDialog()
{
  QAbstractItemModel *model = tableView->model();

  int detailId = model->data(model->index(tableView->currentIndex().row(), Details_Id)).toInt();
  double qty = model->data(model->index(tableView->currentIndex().row(), Details_Qty)).toDouble();
  double nr = model->data(model->index(tableView->currentIndex().row(), Details_NR)).toDouble();

  MovesDialog dialog(detailId, qty, nr, this);
  dialog.exec();
  if(dialog.isDirty()) {
    model->setData(model->index(tableView->currentIndex().row(), Details_NR), dialog.getNr());

    QModelIndex index = model->index(tableView->currentIndex().row(), Details_Text);
    model->submit();

    tableView->setCurrentIndex(index);
  }

}

/*
void DetailsWindow::showAddRequestDialog()
{
  QStringList items;
  items << trUtf8("Заявка") << trUtf8("Расход");

  bool ok;
  QString item = QInputDialog::getItem(this, trUtf8("Выбор"),
				       trUtf8("Тип"), items, 0, false, &ok);
  
  if (!ok || item.isEmpty())
    return;

  QAbstractItemModel *model = tableView->model();
  QItemSelectionModel *selection = tableView->selectionModel();
  QModelIndex index = selection->selectedIndexes().first();

  int clientId = model->data(index).toInt();
     
  QClipboard *clipboard = QApplication::clipboard();
  QString text = clipboard->text();
  QStringList list = text.split("\n");

  bool needUpdate = false;
   
  foreach(QString row, list) {
    QStringList columns = row.split(";");

    if(columns.size()!=8)
      return;

    AddRequestDialog dialog(this,
	trUtf8("Со склада: <b>")+columns[StoragesDialog::Storages_StorageText]+"</b><br>"+
	trUtf8("Наименование: <b>")+columns[StoragesDialog::Storages_ProductText]+"</b><br>"+
	trUtf8("Остаток: <b>")+columns[StoragesDialog::Storages_Amount]+"</b>",
			    columns[5].toInt());
    const int OkButton = dialog.exec();

    
    if(OkButton) {
      QSqlQuery query;
      // еще нет
      query.prepare("INSERT INTO tb_requests(date_,clientid,storageid,amount,price,comment) "
		    "VALUES(:date_,:clientid,:storageid,:amount,:price,:comment)");
      query.bindValue(":date_", dialog.getDate());
      query.bindValue(":clientid", clientId);
      query.bindValue(":storageid", columns[StoragesDialog::Storages_Id].toInt());
      query.bindValue(":amount", dialog.getAmount());
      query.bindValue(":price", dialog.getPrice());
      query.bindValue(":comment", dialog.getComment());
      query.exec();

      // уменьшить кол-во на складе-источнике
      query.prepare("UPDATE tb_storages SET amountBr=amountBr-:n WHERE id=:id");
      query.bindValue(":n", dialog.getAmount());
      query.bindValue(":id", columns[StoragesDialog::Storages_Id].toInt());
      query.exec();

      needUpdate = true;
    } // OkButton
    
  } // foreach
  
  // обновить представление
  //if(needUpdate)
  //tableTableViewProxyModel->setQuery(tableTableViewProxyModel->query().lastQuery());
}


void DetailsWindow::sort(int section)
{
  Qt::SortOrder order = proxyModel->sortOrder();
  if (proxyModel->sortColumn() == section)
    order = proxyModel->sortOrder() == Qt::AscendingOrder ? Qt::DescendingOrder : Qt::AscendingOrder;

  if (qApp->keyboardModifiers() & Qt::CTRL) {
    if (proxyModel->isColumnInSort(section)) {
      Qt::SortOrder order = proxyModel->sortOrder(section) == Qt::AscendingOrder
	? Qt::DescendingOrder
	: Qt::AscendingOrder;
      proxyModel->changeSortedColumn(section, order);
    }
    else {
      proxyModel->addSortedColumn(section, proxyModel->sortOrder());
    }
    
  }
  else {
    Qt::SortOrder order = proxyModel->sortOrder();
    if (proxyModel->isColumnInSort(section)) {
      order = proxyModel->sortOrder(section) == Qt::AscendingOrder
	? Qt::DescendingOrder
	: Qt::AscendingOrder;
    }
    proxyModel->clearSort();
    proxyModel->addSortedColumn(section, order);
  }
}
*/
