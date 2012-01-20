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
  findLabel = new QLabel(trUtf8("&Поиск"));
  findEdit = new QLineEdit;
  findLabel->setBuddy(findEdit);

  QHBoxLayout *topLayout = new QHBoxLayout;
  topLayout->addWidget(findLabel);
  topLayout->addWidget(findEdit);

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
		" AND m.placeId=p.uid"
		" AND m.document LIKE :document "
		"GROUP BY 1,2,3 ORDER BY m.date_ DESC");
  query.bindValue(":document", trUtf8("л/к%"));
  query.exec();

  tableView = new QTableView;
  queryModel = new LimitsModel;
  
  queryModel->setQuery(query);

  queryModel->setHeaderData(Limit_Date, Qt::Horizontal, trUtf8("Дата"));
  queryModel->setHeaderData(Limit_Text, Qt::Horizontal, trUtf8("Документ"));
  queryModel->setHeaderData(Limit_Customer, Qt::Horizontal, trUtf8("Заказчик"));
  queryModel->setHeaderData(Limit_Count, Qt::Horizontal, trUtf8("Кол-во"));
  queryModel->setHeaderData(Limit_Sum, Qt::Horizontal, trUtf8("Сумма"));

  proxyModel = new QSortFilterProxyModel;
  //proxyModel->setDynamicSortFilter(true);
  proxyModel->setSourceModel(queryModel);
  proxyModel->setFilterKeyColumn(-1);
  //proxyModel->sort(Details_Text, Qt::AscendingOrder);

  connect(findEdit, SIGNAL(textChanged(QString)),
  	  this, SLOT(filterRegExpChanged()), Qt::UniqueConnection);


  tableView->setModel(proxyModel);

  tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  
  tableView->verticalHeader()->hide();
  tableView->resizeColumnsToContents();
  tableView->setAlternatingRowColors(true);


  //QAction *viewAction = new QAction(trUtf8("&Просмотр"), this);
  //copyAction->setShortcut(tr("Ctrl+X"));
  //connect(viewAction, SIGNAL(triggered()), this, SLOT(view()));

  QAction *viewAction = new QAction(trUtf8("Просмотр"), this);
  connect(viewAction, SIGNAL(triggered()), this, SLOT(viewPreview()));

  QAction *roadAction = new QAction(trUtf8("Накладная"), this);
  connect(roadAction, SIGNAL(triggered()), this, SLOT(roadPreview()));


  tableView->addAction(viewAction);
  tableView->addAction(roadAction);
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
  mainLayout->addLayout(topLayout);
  mainLayout->addWidget(tableView);
  setLayout(mainLayout);

  setWindowTitle(trUtf8("Лимитки"));
  setFixedWidth(tableView->horizontalHeader()->length()+50);
  setFixedHeight(380);
}

LimitsDialog::~LimitsDialog()
{
  delete proxyModel;
  delete queryModel;
}

void LimitsDialog::filterRegExpChanged()
{
  QRegExp regExp(findEdit->text());
  proxyModel->setFilterRegExp(regExp);
  proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
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

void LimitsDialog::viewPreview()
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
		" AND strftime('%d.%m.%Y',date_)=:date "
		"ORDER BY d.text");
  query.bindValue(":document", lim);
  query.bindValue(":date", limDate);
  query.exec();

  html="";
  html += "<html><body><FONT size=\"+1\">"+
    trUtf8("<H1>Лимитная карта № %1</H1>"
	   "на отпуск запчастей со склада № 8"
	   "<PRE>на капитальный ремонт ДСТ <u><i><b><FONT size=\"+2\">%2</FONT></b></i></u>           Заказчик <u><i><b><FONT size=\"+2\">%3</FONT></b></i></u>     Дислокация <u><i><b><FONT size=\"+2\">%4</FONT></b></i></u></PRE>").arg(dialog.limNumber->text()).arg(dialog.technics->currentText(), -10).arg(customer).arg(dialog.place->text())+
    "<table border=1 cellpadding=5 width=100%>";

  QStringList headers;
  headers << trUtf8("№ п/п") << trUtf8("Кат. номер") << trUtf8("Наименование")
	  << trUtf8("Ед. изм.") << trUtf8("Кол-во") << trUtf8("Дата")
	  << trUtf8("Цена") << trUtf8("Сумма") << trUtf8("№ техники")
	  << trUtf8("Подпись") << trUtf8("Расшиф-ровка");

  html += "<tr>";
  // foreach(QString header, headers) {
  //   html += "<th>";
  //   html += header;
  //   html += "</th>";
  // }
  html += "<th width=4%>" + headers[0]+ "</th>";
  html += "<th width=13%>" + headers[1]+ "</th>";
  html += "<th width=17%>" + headers[2]+ "</th>";
  html += "<th width=5%>" + headers[3]+ "</th>";
  html += "<th width=5%>" + headers[4]+ "</th>";
  html += "<th width=10%>" + headers[5]+ "</th>";
  html += "<th width=10%>" + headers[6]+ "</th>";
  html += "<th width=10%>" + headers[7]+ "</th>";
  html += "<th width=8%>" + headers[8]+ "</th>";
  html += "<th width=9%>" + headers[9]+ "</th>";
  html += "<th width=9%>" + headers[10]+ "</th>";
  html += "</tr>";

  int j=0;
  while(query.next()) {
    html += "<tr>";
    for (int i = 0; i < headers.size(); i++) {
      
      switch(i) {
      case 0:
  	html += "<td align=right>" + tr("%1").arg(++j);
  	break;
      case 1:
  	html += "<td align=center>" + query.value(i).toString();
  	break;
      case 2:
  	html += "<td align=center>" + query.value(i).toString();
  	break;
      case 3:
  	html += "<td align=center>" + trUtf8("шт.");
  	break;
      case 4:
  	html += "<td align=right>" + query.value(i).toString();
  	break;
      case 5:
  	html += "<td align=center>" + query.value(i).toString();
  	break;
      case 6:
      case 7:
  	html += "<td align=right>" + tr("%1").arg(query.value(i).toDouble(),0,'f',2);
  	break;
      case 8:
  	html += "<td align=right>" + dialog.technicsNumber->text();
  	break;
      default:
  	html += "<td>";
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

  html += "</table></FONT></body></html>";
  
  QPrinter printer(QPrinter::ScreenResolution);
  printer.setPaperSize(QPrinter::A4);
  
  printer.setOrientation(QPrinter::Landscape);
  //printer.setFullPage(true);
  QPrintPreviewDialog *printdialog = new QPrintPreviewDialog(&printer, this);
  printdialog->setWindowState(Qt::WindowMaximized);
  
  connect(printdialog, SIGNAL(paintRequested(QPrinter*)), 
      this, SLOT(view(QPrinter*)));

  printdialog->exec();
  delete printdialog;
}

void LimitsDialog::view(QPrinter * printer)
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


void LimitsDialog::roadPreview()
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
		" ,d.qty"
		" ,m.n*d.qty "
		"FROM tb_moves m"
		" ,tb_details d "
		"WHERE 1=1"
		" AND m.detailId=d.uid"
		" AND m.document=:document"
		" AND strftime('%d.%m.%Y',date_)=:date "
		"ORDER BY d.text");
  query.bindValue(":document", lim);
  query.bindValue(":date", limDate);
  query.exec();

  // html="";
  // html += "<html><body>"+
  //   trUtf8("<PRE><H4>Лимитная карта  <FONT size=\"+2\">%1</FONT><BR>"
  // 	   "на отпуск запчастей со склада № 8<BR><BR>"
  // 	   "на капитальный ремонт ДСТ <u><i><b><FONT size=\"+2\">%2</FONT></b></i></u>           Заказчик <u><i><b><FONT size=\"+2\">%3</FONT></b></i></u>     Дислокация <u><i><b><FONT size=\"+2\">%4</FONT></b></i></u></H4></PRE>").arg(dialog.limNumber->text()).arg(dialog.technics->currentText(), -10).arg(customer).arg(dialog.place->text())+
  //   

  html="";
  html += "<html><body> <FONT size=\"+1\">"+
    trUtf8("<table border=0 width=100% cellspacing=10> "
	   "<tr> <td  width=50%>Разрешил: </td><td align=right>Дата: %1</td> </tr>"
	   "<tr> <td align=center colspan=2><H1>Накладная № %2</H1</td> </tr>"
	   "<tr> <td>От кого: <i>СУПНР уч-к №8 г.Надым <hr/></i> </td><td>Кому: <i>%3</i><hr/> </td> </tr>"
	   "<tr> <td>Через кого: <i>%4</i><hr/> </td><td>а/машина: <i>%5</i> <hr/> </td> </tr>"
	   "</table>").arg(limDate).arg(dialog.limNumber->text()).arg(dialog.technics->currentText()).arg(dialog.place->text()).arg(dialog.technicsNumber->text())+
    "<table border=1 cellpadding=5 width=100% >";

  QStringList headers;
  headers << trUtf8("№ п/п") << trUtf8("Кат. номер") << trUtf8("Наименование")
	  << trUtf8("Ед. изм.") << trUtf8("Кол-во") << trUtf8("Цена")
	  << trUtf8("Сумма") << trUtf8("Примеча-ние");

  html += "<tr>";
  // foreach(QString header, headers) {
  //   html += "<th>";
  //   html += header;
  //   html += "</th>";
  // }
  
  html += "<th width=6%>" + headers[0]+ "</th>";
  html += "<th width=14%>" + headers[1]+ "</th>";
  html += "<th width=21%>" + headers[2]+ "</th>";
  html += "<th width=8%>" + headers[3]+ "</th>";
  html += "<th width=8%>" + headers[4]+ "</th>";
  html += "<th width=14%>" + headers[5]+ "</th>";
  html += "<th width=14%>" + headers[6]+ "</th>";
  html += "<th width=15%>" + headers[7]+ "</th>";
  html += "</tr>";

  int j = 0;
  while(query.next()) {
    html += "<tr>";
    for (int i = 0; i < headers.size(); i++) {
      switch(i) {
      case 0:
	html += "<td align=right>" + tr("%1").arg(++j);
	break;
      case 1:
	html += "<td align=center>" + query.value(i).toString();
	break;
      case 2:
	html += "<td>" + query.value(i).toString();
	break;
      case 3:
	html += "<td align=center>" + trUtf8("шт.");
	break;
      case 4:
	html += "<td align=right>" + query.value(i).toString();
	break;
      case 5:
	html += "<td align=right>" +
	  tr("%1").arg(query.value(i).toDouble(), 0, 'f', 2);
	break;
      case 6:
	html += "<td align=right>" +
	  tr("%1").arg(query.value(i).toDouble(), 0, 'f', 2);
	break;
      default:
	html += "<td>";
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

  html += "</table>";
  html += trUtf8("<br> <table border=0 width=100% cellspacing=10> "
		 " <tr>"
		 "  <td width=50%>Груз отпустил:</td>"
		 "  <td>Груз получил:</td>"
		 " </tr>"
		 "</table></FONT></body></html>");
  
  QPrinter printer(QPrinter::ScreenResolution);
  printer.setPaperSize(QPrinter::A4);
  
  printer.setOrientation(QPrinter::Portrait);
  //printer.setFullPage(true);
  QPrintPreviewDialog *printdialog = new QPrintPreviewDialog(&printer, this);
  printdialog->setWindowState(Qt::WindowMaximized);
  
  connect(printdialog, SIGNAL(paintRequested(QPrinter*)), 
      this, SLOT(road(QPrinter*)));

  printdialog->exec();
  delete printdialog;
}

void LimitsDialog::road(QPrinter * printer)
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

