#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDateTimeEdit>
#include <QTableView>
#include <QAction>
#include <QHeaderView>
#include <QApplication>
#include <QInputDialog>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QDataWidgetMapper>
#include <QDebug>

#include <QtSql>
//#include <QSqlRelationalTableModel>

#include "movedialog.h"

MoveDialog::MoveDialog(qint32 id, qint32 detailId, double qty, double nr, QWidget *parent)
  : QDialog(parent), id_(id), detailId_(detailId), qty_(qty), nr_(nr)
{
  dateTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime());
  dateTimeEdit->setCalendarPopup(true);
  dateTimeEdit->setDisplayFormat("dd.MM.yyyy hh:mm:ss");
  dateLabel = new QLabel(trUtf8("Дата"));
  dateLabel->setBuddy(dateTimeEdit);
  
  placeComboBox = new QComboBox;
  placeLabel = new QLabel(trUtf8("Назначение"));
  placeLabel->setBuddy(placeComboBox);

  documentEdit = new QLineEdit;
  documentLabel = new QLabel(trUtf8("Документ"));
  documentLabel->setBuddy(documentEdit);

  nSpinBox = new QDoubleSpinBox;
  nSpinBox->setRange(0, 999999999);
  nLabel = new QLabel(trUtf8("Кол-во"));
  nLabel->setBuddy(nSpinBox);

  ostSpinBox = new QDoubleSpinBox;
  ostSpinBox->setRange(0, 999999999);
  ostLabel = new QLabel(trUtf8("Остаток"));
  ostLabel->setBuddy(ostSpinBox);

  // limSpinBox = new QSpinBox;
  // limLabel = new QLabel(trUtf8("Лимитка"));
  // limLabel->setBuddy(limSpinBox);

  typeCheckBox = new QCheckBox(trUtf8("Расход"));
  typeLabel = new QLabel(trUtf8("Тип"));
  typeLabel->setBuddy(typeCheckBox);

  //connect(typeCheckBox, SIGNAL(stateChanged(int)),
  //  this, SLOT(typeChanged()), Qt::UniqueConnection);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                   | QDialogButtonBox::Cancel);

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(OkButtonPushed()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QGridLayout *topLayout = new QGridLayout;
  topLayout->addWidget(typeLabel, 0, 0);
  topLayout->addWidget(typeCheckBox, 0, 1);
  topLayout->addWidget(dateLabel, 1, 0);
  topLayout->addWidget(dateTimeEdit, 1, 1);
  topLayout->addWidget(placeLabel, 2, 0);
  topLayout->addWidget(placeComboBox, 2, 1);
  topLayout->addWidget(documentLabel, 3, 0);
  topLayout->addWidget(documentEdit, 3, 1);
  topLayout->addWidget(nLabel, 4, 0);
  topLayout->addWidget(nSpinBox, 4, 1);
  topLayout->addWidget(ostLabel, 5, 0);
  topLayout->addWidget(ostSpinBox, 5, 1);
  //topLayout->addWidget(limLabel, 6, 0);
  //topLayout->addWidget(limSpinBox, 6, 1);

  tableModel = new QSqlRelationalTableModel(this);
  tableModel->setTable("tb_moves");
  tableModel->setRelation(Move_PlaceId, QSqlRelation("tb_places", "uid", "text"));
  tableModel->setFilter(QString("tb_moves.uid=%1").arg(id));
  tableModel->select();
  tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
  
  placeModel = tableModel->relationModel(Move_PlaceId);
  placeModel->sort(1, Qt::AscendingOrder);
  //relModel->setFilter(QString("mflag=1"));

    //placeModel = new QSqlTableModel(this);
    //placeModel->setTable("tb_places");
    //placeModel->setFilter(QString("mflag=%1").arg(typeCheckBox->isChecked()));
    //placeModel->select();

  placeComboBox->setModel(placeModel);
  placeComboBox->setModelColumn(placeModel->fieldIndex("text"));
  //placeComboBox->setCurrentIndex(0);
  
  mapper = new QDataWidgetMapper(this);
  mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
  mapper->setModel(tableModel);
  mapper->setItemDelegate(new QSqlRelationalDelegate(this));
  mapper->addMapping(typeCheckBox, Move_Type);
  mapper->addMapping(dateTimeEdit, Move_Date);
  mapper->addMapping(placeComboBox, Move_PlaceId);
  mapper->addMapping(documentEdit, Move_Document);
  mapper->addMapping(nSpinBox, Move_N);
  mapper->addMapping(ostSpinBox, Move_Ost);
  //mapper->addMapping(limSpinBox, Move_Lim);
  
  if(id_ == -1)
    tableModel->insertRow(0);

  mapper->toFirst();

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(topLayout);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  setWindowTitle(trUtf8("Новое движение"));
}


MoveDialog::~MoveDialog()
{
  //delete queryModel;
  //delete proxyModel;
}

void MoveDialog::typeChanged()
{
  //placeModel->setFilter(QString("mflag=%1").arg(typeCheckBox->isChecked()));
  //placeModel->select();
}


double MoveDialog::getAmount()
{
  double amount = nSpinBox->value();
  if(typeCheckBox->isChecked())
    amount *= -1;

  return amount;
}

void MoveDialog::OkButtonPushed()
{
  if(id_ == -1 ) {
    tableModel->setData(tableModel->index(0,Move_DetailId), detailId_);
    tableModel->setData(tableModel->index(0,Move_Qty), qty_);

    tableModel->setData(tableModel->index(0,Move_Ost), nr_ + getAmount());
  }
  if(mapper->submit())  {
    if(tableModel->submitAll())
      //qDebug() << "ok: " << tableModel->lastError().text();
      accept();
  }
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

