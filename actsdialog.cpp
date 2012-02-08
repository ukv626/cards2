#include <QtGui>
/*
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTableView>
#include <QAction>
#include <QHeaderView>
#include <QClipboard>
#include <QApplication>
#include <QInputDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QDate>
*/

#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QSqlQuery>
#include <QSqlError>

#include "actsdialog.h"

ActsModel::ActsModel(QObject *parent)
 : QSqlQueryModel(parent)
{
}


QVariant ActsModel::data(const QModelIndex &index, int role) const
{
  QVariant value = QSqlQueryModel::data(index, role);

  switch (role) {
  case Qt::DisplayRole: // Данные для отображения
    if (index.column() == ActsDialog::Act_Sum)
      return tr("%1").arg(value.toDouble(), 0, 'f', 2);
    else
      return value;
  case Qt::TextAlignmentRole: // Выравнивание
    if(index.column() == ActsDialog::Act_Count ||
       index.column() == ActsDialog::Act_Sum)
      return int(Qt::AlignRight | Qt::AlignVCenter);
    else
      return int(Qt::AlignLeft | Qt::AlignVCenter);
  }

  return value;
}


ActsDialog::ActsDialog(qint8 /* typeId */, QWidget *parent)
  : QDialog(parent)
{
  findLabel_ = new QLabel(trUtf8("&Поиск"));
  findEdit_ = new QLineEdit;
  findLabel_->setBuddy(findEdit_);

  QHBoxLayout *topLayout = new QHBoxLayout;
  topLayout->addWidget(findLabel_);
  topLayout->addWidget(findEdit_);

  QSqlQuery query;
  query.exec();
  
  query.prepare("SELECT strftime('%d.%m.%Y',m.date_)"
		" ,m.document"
		" ,p.text"
		" ,COUNT(1)"
		" ,SUM(m.n*d.qty) "
		"FROM tb_moves m "
		" ,tb_places p "
		" ,tb_details d "
		"WHERE 1=1 "
		" AND m.detailId=d.uid"
		// " AND d.typeId=:typeId"
		" AND m.placeId=p.uid"
		" AND m.document NOT LIKE :document "
		"GROUP BY 1,2,3 ORDER BY m.date_ DESC");
  // query.bindValue(":typeId", typeId);
  query.bindValue(":document", trUtf8("л/к%"));
  query.exec();

  tableView_ = new QTableView;
  queryModel_ = new ActsModel;
  
  queryModel_->setQuery(query);

  queryModel_->setHeaderData(Act_Date, Qt::Horizontal, trUtf8("Дата"));
  queryModel_->setHeaderData(Act_Text, Qt::Horizontal, trUtf8("Документ"));
  queryModel_->setHeaderData(Act_Customer, Qt::Horizontal, trUtf8("Заказчик"));
  queryModel_->setHeaderData(Act_Count, Qt::Horizontal, trUtf8("Кол-во"));
  queryModel_->setHeaderData(Act_Sum, Qt::Horizontal, trUtf8("Сумма"));

  proxyModel_ = new QSortFilterProxyModel;
  //proxyModel->setDynamicSortFilter(true);
  proxyModel_->setSourceModel(queryModel_);
  proxyModel_->setFilterKeyColumn(-1);
  //proxyModel->sort(Details_Text, Qt::AscendingOrder);

  connect(findEdit_, SIGNAL(textChanged(QString)),
  	  this, SLOT(filterRegExpChanged()), Qt::UniqueConnection);


  tableView_->setModel(proxyModel_);

  tableView_->setSelectionMode(QAbstractItemView::ExtendedSelection);
  tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
  
  tableView_->verticalHeader()->hide();
  tableView_->resizeColumnsToContents();
  tableView_->setAlternatingRowColors(true);


  //QAction *viewAction = new QAction(trUtf8("&Просмотр"), this);
  //copyAction->setShortcut(tr("Ctrl+X"));
  //connect(viewAction, SIGNAL(triggered()), this, SLOT(view()));

  QAction *viewAction = new QAction(trUtf8("Просмотр"), this);
  connect(viewAction, SIGNAL(triggered()), this, SLOT(view()));

  tableView_->addAction(viewAction);
  tableView_->setContextMenuPolicy(Qt::ActionsContextMenu);

  /*
  tableView->horizontalHeader()->setStretchLastSection(false);
  tableView->horizontalHeader()->setResizeMode(Storages_AmountBr,QHeaderView::Custom);
  tableView->horizontalHeader()->setResizeMode(Storages_ProductText,QHeaderView::Stretch);
  tableView->horizontalHeader()->setResizeMode(Storages_Amount,QHeaderView::Custom);
  */

  tableView_->resizeColumnsToContents();
  tableView_->setCurrentIndex(tableView_->model()->index(0, 0));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(topLayout);
  mainLayout->addWidget(tableView_);
  setLayout(mainLayout);

  setWindowTitle(trUtf8("Акты"));
  setFixedWidth(tableView_->horizontalHeader()->length()+50);
  setFixedHeight(380);
}

ActsDialog::~ActsDialog()
{
  delete proxyModel_;
  delete queryModel_;
}

void ActsDialog::filterRegExpChanged()
{
  QRegExp regExp(findEdit_->text());
  proxyModel_->setFilterRegExp(regExp);
  proxyModel_->setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void ActsDialog::view()
{
  QAbstractItemModel *model = tableView_->model();
  QString lim = model->data(model->index(tableView_->currentIndex().row(),
					 Act_Text)).toString();
  // QString customer = model->data(model->index(tableView_->currentIndex().row(),
  // 					 Limit_Customer)).toString();
  QString limDate = model->data(model->index(tableView_->currentIndex().row(),
					 Act_Date)).toString();
  

  QSqlQuery query;
  query.prepare("SELECT 1 "
		",d.catNum "
		",d.text "
		",'' "
		",m.n "
		",strftime('%d.%m.%Y',date_) "
		",d.qty "
		",m.n*d.qty "
		"FROM tb_moves m"
		" ,tb_details d "
		"WHERE 1=1 "
		"AND m.detailId=d.uid "
		"AND m.document=:document "
		"AND strftime('%d.%m.%Y',date_)=:date "
		"ORDER BY d.text");
  query.bindValue(":document", lim);
  query.bindValue(":date", limDate);
  query.exec();

  QFile file("./txt/acts.txt");
  if(file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    
    QTextStream fout(&file);
    fout.setCodec("Windows-1251");
    
    while(query.next()) {
      for(qint8 i=0; i < 8; ++i) {
	if(i == 6 || i == 7)
	  fout << query.value(i).toString().replace('.',',') << ";";
	else
	  fout << query.value(i).toString() << ";";
      }
      fout << "\n";
    }
    file.close();
    QMessageBox::information(0, trUtf8("Информация"), trUtf8("Выполнено.."));
  }
  
  // QProcess proc;
  // proc.startDetached("./txt/acts.txt");
}


