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

#include "limitsdialog.h"
#include "LimitsRequestDialog.h"
//#include "addrequestdialog.h"
//#include "editLimitDialog.h"

LimitsModel::LimitsModel(QObject *parent)
 : QSqlQueryModel(parent)
{
}


QVariant LimitsModel::data(const QModelIndex &index, int role) const
{
  QVariant value = QSqlQueryModel::data(index, role);

  switch (role) {
  case Qt::DisplayRole: // Данные для отображения
    if (index.column() == LimitsDialog::Limit_Sum)
      return tr("%1").arg(value.toDouble(), 0, 'f', 2);
    else
      return value;
  case Qt::TextAlignmentRole: // Выравнивание
    if(index.column() == LimitsDialog::Limit_Count ||
       index.column() == LimitsDialog::Limit_Sum)
      return int(Qt::AlignRight | Qt::AlignVCenter);
    else
      return int(Qt::AlignLeft | Qt::AlignVCenter);
  }

  return value;
}


LimitsDialog::LimitsDialog(QWidget *parent)
  : QDialog(parent)
{
  QSqlQuery query;
  query.prepare("SELECT strftime('%Y-%m-%d',m.date_)"
		" ,m.document"
		" ,p.text"
		" ,COUNT(1)"
		" ,SUM(m.n*d.qty) "
		"FROM tb_moves m "
		" ,tb_places p "
		" ,tb_details d "
		"WHERE 1=1 "
		" AND m.detailId=d.uid"
		" AND m.placeId=p.uid"
		" AND m.document LIKE :document "
		"GROUP BY 1,2,3 ORDER BY 1 DESC");
  query.bindValue(":type", trUtf8("л/к%"));
  query.exec();

  tableView = new QTableView;
  queryModel = new LimitsModel;
  
  queryModel->setQuery(query);

  queryModel->setHeaderData(Limit_Date, Qt::Horizontal, trUtf8("Дата"));
  queryModel->setHeaderData(Limit_Text, Qt::Horizontal, trUtf8("Документ"));
  queryModel->setHeaderData(Limit_Customer, Qt::Horizontal, trUtf8("Заказчик"));
  queryModel->setHeaderData(Limit_Count, Qt::Horizontal, trUtf8("Кол-во"));
  queryModel->setHeaderData(Limit_Sum, Qt::Horizontal, trUtf8("Сумма"));

  tableView->setModel(queryModel);

  tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  
  tableView->verticalHeader()->hide();
  tableView->resizeColumnsToContents();
  tableView->setAlternatingRowColors(true);


  //QAction *viewAction = new QAction(trUtf8("&Просмотр"), this);
  //copyAction->setShortcut(tr("Ctrl+X"));
  //connect(viewAction, SIGNAL(triggered()), this, SLOT(view()));

  QAction *printAction = new QAction(trUtf8("Просмотр"), this);
  connect(printAction, SIGNAL(triggered()), this, SLOT(printPreview()));


  //tableView->addAction(viewAction);
  tableView->addAction(printAction);
  tableView->setContextMenuPolicy(Qt::ActionsContextMenu);

  /*
  tableView->horizontalHeader()->setStretchLastSection(false);
  tableView->horizontalHeader()->setResizeMode(Storages_AmountBr,QHeaderView::Custom);
  tableView->horizontalHeader()->setResizeMode(Storages_ProductText,QHeaderView::Stretch);
  tableView->horizontalHeader()->setResizeMode(Storages_Amount,QHeaderView::Custom);
  */

  tableView->resizeColumnsToContents();
  tableView->setCurrentIndex(tableView->model()->index(0, 0));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(tableView);
  setLayout(mainLayout);

  setWindowTitle(trUtf8("Лимитки"));
  setFixedWidth(tableView->horizontalHeader()->length()+50);
}

LimitsDialog::~LimitsDialog()
{
  delete queryModel;
}

/*
void LimitsDialog::view()
{
  QAbstractItemModel *model = tableView->model();
  QString lim = model->data(model->index(tableView->currentIndex().row(),
					 Limit_Text)).toString();

  ViewLimitDialog dialog(lim);
  dialog.exec();
}
*/


/*
void LimitsDialog::filterStringChanged()
{
  //QRegExp regExp(lineEdit->text());
  proxyModel->setFilterFixedString(comboBox->currentText());
  //model->setFilterCaseSensitivity(Qt::CaseInsensitive);
  ///model->sort();
}
*/

void LimitsDialog::printPreview()
{
  LimitsRequestDialog dialog(this);
  if(dialog.exec() != QDialog::Accepted)
    return;
  
  QAbstractItemModel *model = tableView->model();
  QString lim = model->data(model->index(tableView->currentIndex().row(),
					 Limit_Text)).toString();
  QString customer = model->data(model->index(tableView->currentIndex().row(),
					 Limit_Customer)).toString();
  QString limDate = model->data(model->index(tableView->currentIndex().row(),
					 Limit_Date)).toString();
  

  QSqlQuery query;
  query.prepare("SELECT 1"
		" ,d.catNum"
		" ,d.text"
		" ,''"
		" ,m.n"
		" ,strftime('%d.%m.%Y',date_)"
		" ,d.qty"
		" ,m.n*d.qty "
		"FROM tb_moves m"
		" ,tb_details d "
		"WHERE 1=1"
		" AND m.detailId=d.uid"
		" AND m.document=:document"
		" AND strftime('%Y-%m-%d',date_)=:date "
		"ORDER BY d.text");
  query.bindValue(":document", lim);
  query.bindValue(":date", limDate);
  query.exec();

  html="";
  html += "<html><body>"+
    trUtf8("<PRE><H4>Лимитная карта  <FONT size=\"+2\">%1</FONT><BR>"
	   "на отпуск запчастей со склада № 8<BR><BR>"
	   "на капитальный ремонт ДСТ <u><i><b><FONT size=\"+2\">%2</FONT></b></i></u>           Заказчик <u><i><b><FONT size=\"+2\">%3</FONT></b></i></u>     Дислокация <u><i><b><FONT size=\"+2\">%4</FONT></b></i></u></H4></PRE>").arg(dialog.limNumber->text()).arg(dialog.technics->currentText(), -10).arg(customer).arg(dialog.place->text())+
    "<table border=1 cellpadding=5 width=100%>";

  QStringList headers;
  headers << trUtf8("№ п/п") << trUtf8("Кат. номер") << trUtf8("Наименование")
	  << trUtf8("Ед. изм.") << trUtf8("Кол-во") << trUtf8("Дата")
	  << trUtf8("Цена") << trUtf8("Сумма") << trUtf8("№ техники")
	  << trUtf8("Подпись") << trUtf8("Расшифровка");

  html += "<tr>";
  foreach(QString header, headers) {
    html += "<th>";
    html += header;
    html += "</th>";
  }
  html += "</tr>";

  int j=0;
  while(query.next()) {
    html += "<tr>";
    for (int i = 0; i < 11; i++) {
      if(i == 0 || i == 4 || i == 6 || i == 7 || i ==8)
	html += "<td align=right>";
      else if(i == 1 || i == 3)
	html += "<td align=center>";
      else
	html += "<td>";
      
      switch(i) {
      case 0:
	html += tr("%1").arg(++j);
	break;
      case 3:
	html += trUtf8("шт.");
	break;
      case 1:
      case 2:
      case 4:
      case 5:
	html += query.value(i).toString();
	break;
      case 6:
      case 7:
	html += tr("%1").arg(query.value(i).toDouble(), 0, 'f', 2);
	break;
      case 8:
	html += dialog.technicsNumber->text();
	break;
      }
      
      html += "</td>";
    }
    html += "</tr>";
  }

  /*
  html += "<tr>";
  html += "<td border=0 colspan=7>Final</td>";
  html += "<td align=right border=0>0.00</td>";
  html += "<td border=0 colspan=3></td>";
  html += "</tr>";
  */

  html += "</table></body></html>";
  
  QPrinter printer(QPrinter::ScreenResolution);
  printer.setPaperSize(QPrinter::A4);
  
  printer.setOrientation(QPrinter::Landscape);
  //printer.setFullPage(true);
  QPrintPreviewDialog *printdialog = new QPrintPreviewDialog(&printer, this);
  printdialog->setWindowState(Qt::WindowMaximized);
  
  connect(printdialog, SIGNAL(paintRequested(QPrinter*)), 
      this, SLOT(print(QPrinter*)));

  printdialog->exec();
  delete printdialog;
}

void LimitsDialog::print(QPrinter * printer)
{
  QTextDocument *doc = new QTextDocument();
  doc->setHtml(html);

  QSizeF paperSize;
  paperSize.setWidth(printer->width());
  paperSize.setHeight(printer->height());

  doc->setPageSize(paperSize);
  doc->print(printer);

  delete doc;
}

/*
void LimitsDialog::paste()
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
    model->setQuery(model->query().lastQuery());
}
*/

