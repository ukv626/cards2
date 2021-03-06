#include <QtGui>
#include <QSqlQuery>


#include "mainwindow.h"
#include "detailswindow.h"
#include "limitsdialog.h"
#include "actsdialog.h"
#include "DatesInputDialog.h"
#include "placesdialog.h"
#include "place-dialog.h"

MainWindow::MainWindow()
{
    detailsWindow = new DetailsWindow;
    setCentralWidget(detailsWindow);

    createActions();
    createMenus();
    createContextMenu();
    createToolBars();
    createStatusBar();

    query = new QSqlQuery;

    connect(detailsWindow, SIGNAL(typeChangedSig(int)),
            this, SLOT(detailsTypeChanged(int)));

    detailsTypeChanged(0);

    //readSettings();

    //findDialog = 0;

    //setWindowIcon(QIcon(":/images/icon.png"));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  writeSettings();
  event->accept();
}

void MainWindow::detailsTypeChanged(int typeId)
{
  QSqlQuery query;
  query.prepare("SELECT SUM(qty*nr) FROM tb_details WHERE typeId=:typeId");
  query.bindValue(":typeId_", typeId);
  query.exec();

  double sum = 0;
  if(query.next())
    sum = query.value(0).toDouble();

  formulaLabel->setText(QString("%1").arg(sum, 0, 'f', 2));
}

/*
void MainWindow::find()
{
    if (!findDialog) {
        findDialog = new FindDialog(this);
        connect(findDialog, SIGNAL(findNext(const QString &,
                                            Qt::CaseSensitivity)),
                spreadsheet, SLOT(findNext(const QString &,
                                           Qt::CaseSensitivity)));
        connect(findDialog, SIGNAL(findPrevious(const QString &,
                                                Qt::CaseSensitivity)),
                spreadsheet, SLOT(findPrevious(const QString &,
                                               Qt::CaseSensitivity)));
    }

    findDialog->show();
    findDialog->raise();
    findDialog->activateWindow();
}
*/

void MainWindow::limits()
{
  LimitsDialog dialog(detailsWindow->getCurTypeId(), this);
  dialog.exec();
}


void MainWindow::acts()
{
  ActsDialog dialog(detailsWindow->getCurTypeId(), this);
  dialog.exec();
}

void MainWindow::report1fully_(const QDate &date1, const QDate &date2,
			       bool mflag, QFile *file)
{
  QSqlQuery query;
  query.prepare("SELECT strftime('%d.%m.%Y',date_) "
		",p.text "
		",d.catNum "
		",d.text "
		",m.document "
		",m.n "
		",d.qty "
		",m.n*d.qty "
		"FROM tb_moves m "
		",tb_details d "
		",tb_places p "
		"WHERE 1=1 "
		"AND m.detailID=d.uid "
		"AND m.placeId=p.uid "
		"AND DATE(date_)>=:date1 "
		"AND DATE(date_)<=:date2 "
		"AND m.mflag=:mflag "
		"ORDER BY 2,1");
  query.bindValue(":date1", date1);
  query.bindValue(":date2", date2);
  query.bindValue(":mflag", mflag);
  query.exec();

  QTextStream fout(file);
  fout.setCodec("Windows-1251");
  
  if(mflag)
    fout << trUtf8("РАСХОД\n");
  else
    fout << trUtf8("ПРИХОД\n");
  
  while(query.next()) {
    for(qint8 i=0; i < 8; ++i) {
      if(i == 6 || i == 7)
	fout << query.value(i).toString().replace('.',',') << ";";
      else
	fout << query.value(i).toString() << ";";
    }
    fout << "\n";
  }
  fout << "\n\n";

  query.prepare("SELECT p.text"
		",SUM(m.n*d.qty) "
		"FROM tb_moves m "
		",tb_details d "
		",tb_places p "
		"WHERE 1=1 "
		"AND m.detailID=d.uid "
		"AND m.placeId=p.uid "
		"AND DATE(m.date_)>=:date1 "
		"AND DATE(m.date_)<=:date2 "
		"AND m.mflag=:mflag "
		"GROUP BY 1 "
		"ORDER BY 1");
  query.bindValue(":date1", date1);
  query.bindValue(":date2", date2);
  query.bindValue(":mflag", mflag);
  query.exec();


  double sum = 0;

  while(query.next()) {
    sum += query.value(1).toDouble();
    
    for(qint8 i = 0; i < 2; ++i) {
      if(i == 1)
	fout << query.value(i).toString().replace('.',',') << ";";
      else
	fout << query.value(i).toString() << ";";
    }
    fout << "\n";
  }

  fout << trUtf8("ИТОГО: %1\n").arg(sum, 0, 'f', 2);
  
  if(mflag)
    fout << "\n\n\n\n\n\n";
}

void MainWindow::report1fully()
{
  DatesInputDialog dialog(this);
  
  if(dialog.exec() != QDialog::Accepted)
    return;

  QFile file("./txt/report1fully.txt");
  if(file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    report1fully_(dialog.date1Edit->date(), dialog.date2Edit->date(), true, &file);
    report1fully_(dialog.date1Edit->date(), dialog.date2Edit->date(), false, &file);
    file.close();
    QMessageBox::information(0, trUtf8("Информация"), trUtf8("Выполнено.."));
  }
}


void MainWindow::report1()
{
  DatesInputDialog dialog(this);
  
  if(dialog.exec() != QDialog::Accepted)
    return;

  QSqlQuery query;
  query.prepare("SELECT p.text "
		",SUM(CASE WHEN m.mflag='true' THEN m.qty*m.n ELSE 0 END) "
		",SUM(CASE WHEN m.mflag='false' THEN m.qty*m.n ELSE 0 END) "
		"FROM tb_moves m, tb_places p, tb_details d "
		"WHERE m.placeID=p.uid "
		"AND m.detailID=d.uid "
		"AND DATE(m.date_)>=:date1 "
		"AND DATE(m.date_)<=:date2 "
		"GROUP BY 1 ORDER BY 1");

  query.bindValue(":date1", dialog.date1Edit->date());
  query.bindValue(":date2", dialog.date2Edit->date());
  query.exec();
    
  html = "";
  html += "<html><body>"+
    trUtf8("<H4>Движение за период с ") +
    trUtf8("%1 по ").arg(dialog.date1Edit->date().toString("dd.MM.yyyy")) +
    trUtf8("%1").arg(dialog.date2Edit->date().toString("dd.MM.yyyy")) +
    "</H4>" +
    "<table border=1 cellpadding=5 width=100%>";

  QStringList headers;
  headers << trUtf8("Наименование") << trUtf8("Расход") << trUtf8("Приход");

  html += "<tr>";
  foreach(QString header, headers) {
    html += "<th>";
    html += header;
    html += "</th>";
  }
  html += "</tr>";

  double sumRa = 0;
  double sumPr = 0;
  while(query.next()) {
    sumRa += query.value(1).toDouble();
    sumPr += query.value(2).toDouble();         

    html += "<tr>";
    for (int i = 0; i < 3; i++) {
      if(i == 1 || i == 2) {
	html += "<td align=right>";
	html += tr("%1").arg(query.value(i).toDouble(), 0, 'f', 2);
      }
      else {
	html += "<td>";
	html += query.value(i).toString();
      }
      
      html += "</td>";
    }
    html += "</tr>";
  }

  // total
  html += trUtf8("<tr>"
             "<td><b>ИТОГО</b></td>"
             "<td align=right>%1</td>"
             "<td align=right>%2</td>"
             "</tr>").arg(sumRa, 0, 'f', 2).arg(sumPr, 0, 'f', 2);
    
  html += "</table></body></html>";

  QPrinter printer(QPrinter::ScreenResolution);
  printer.setPaperSize(QPrinter::A4);

  QPrintPreviewDialog *printdialog = new QPrintPreviewDialog(&printer, this);
  printdialog->setWindowState(Qt::WindowMaximized);

  connect(printdialog, SIGNAL(paintRequested(QPrinter*)), 
	  this, SLOT(report1Print(QPrinter*)));

  printdialog->exec();
  delete printdialog;
}

void MainWindow::report1Print(QPrinter * printer)
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

void MainWindow::report2()
{
  PlaceDialog dialog(this);
  if(dialog.exec() != QDialog::Accepted)
    return;
  
  // LimitsRequestDialog dialog(this);
  // if(dialog.exec() != QDialog::Accepted)
  //   return;
  
  // QAbstractItemModel *model = tableView->model();
  // QString lim = model->data(model->index(tableView->currentIndex().row(),
  // 					 Limit_Text)).toString();
  // QString customer = model->data(model->index(tableView->currentIndex().row(),
  // 					 Limit_Customer)).toString();
  // QString limDate = model->data(model->index(tableView->currentIndex().row(),
  // 					 Limit_Date)).toString();
  

  QSqlQuery query;
  QString queryStr = "SELECT 1"
                     " ,strftime('%d.%m.%Y',m.date_)"
                     " ,d.catNum"
                     " ,d.text"
                     " ,m.n"
                     " ,d.qty"
                     " ,m.n*d.qty "
                     " ,p.text "
                     " ,m.document "
                     "FROM tb_moves m"
                     " ,tb_details d "
                     " ,tb_places p " 
                     "WHERE 1=1"
                     " AND m.detailId=d.uid"
                     " AND m.placeId=p.uid"
                     " AND m.document like :document ";
  if(dialog.getText() != "") {
    queryStr += tr(" AND p.text='%1' ").arg(dialog.getText());
  }
  
  queryStr += "ORDER BY m.date_";
  query.prepare(queryStr);
  query.bindValue(":document", trUtf8("л/к%"));
  query.exec();

  html="";
  html += "<html><body>" +
    trUtf8("<H4>Отчет по лимиткам</H4>") +
    "<table border=1 cellpadding=5 width=100%>";

  QStringList headers;
  headers << trUtf8("№ п/п") << trUtf8("Дата") << trUtf8("Кат. номер")
	  << trUtf8("Наименование") << trUtf8("Кол-во") << trUtf8("Цена")
	  << trUtf8("Сумма") << trUtf8("Заказчик") << trUtf8("Документ");

  html += "<tr>";
  foreach(QString header, headers) {
    html += "<th>";
    html += header;
    html += "</th>";
  }
  html += "</tr>";

  int j = 0;
  double total = 0;
  while(query.next()) {
    total += query.value(6).toDouble();
    html += "<tr>";
    for (int i = 0; i < 9; i++) {
      if(i == 0 || i == 4 || i == 5 || i == 6)
	html += "<td align=right>";
      else if(i == 1)
	html += "<td align=center>";
      else
	html += "<td>";
      
      switch(i) {
      case 0:
	html += tr("%1").arg(++j);
	break;
      case 1:
      case 2:
      case 3:
      case 4:
      case 7:
      case 8:
	html += query.value(i).toString();
	break;
      case 5:
      case 6:
	html += tr("%1").arg(query.value(i).toDouble(), 0, 'f', 2);
	break;
      }
      
      html += "</td>";
    }
    html += "</tr>";
  }
  html += trUtf8("<tr>"
	     " <td></td>"
	     " <td><b>ИТОГО<b></td>"
	     " <td></td>"
	     " <td></td>"
	     " <td></td>"
	     " <td></td>"
	     " <td align=right><b>%1</b></td>"
	     " <td></td>"
	     " <td></td>"
	     "</tr>").arg(total, 0, 'f', 2);

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
      this, SLOT(report2Print(QPrinter*)));

  printdialog->exec();
  delete printdialog;
}

void MainWindow::report2Print(QPrinter * printer)
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

// void MainWindow::updateTable(const QString &table, int uid, const QString &text)
// {
//   QSqlQuery query;
//   query.prepare("UPDATE "+table+" SET prim=:prim||prim WHERE uid=:uid");
//   query.bindValue(":uid",  uid);
//   query.bindValue(":prim", text);
//   query.exec();
// }

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Spreadsheet"),
            tr("<h2>Spreadsheet 1.1</h2>"
               "<p>Copyright &copy; 2008 Software Inc."
               "<p>Spreadsheet is a small application that "
               "demonstrates QAction, QMainWindow, QMenuBar, "
               "QStatusBar, QTableWidget, QToolBar, and many other "
               "Qt classes."));

  // QSqlQuery query("SELECT * FROM tb_details WHERE nr>0");
  // QSqlQuery query2;
  // bool found;
  // int eq=0, eq2=0 ,ne=0, nr;//more=0,less=0;

  // while (query.next()) {
  //   query2.prepare("SELECT uid,nr FROM tb_details0101 "
  // 		  "WHERE catNum=:catNum "
  // 		   // "AND text=:text "
  // 		  "AND qty=:qty");

  //   query2.bindValue(":catNum", query.value(1).toString());
  //   // query2.bindValue(":text",   query.value(2).toString());
  //   query2.bindValue(":qty",    query.value(3).toString());
  //   query2.exec();

  //   found = false;
  //   nr = 0;
  //   while (query2.next()) {
  //     found = true;
      
  //     if(query.value(6).toInt() == query2.value(1).toInt()) {
  // 	updateTable("tb_details", query.value(0).toInt(), "EE ");
  // 	updateTable("tb_details0101", query2.value(0).toInt(), "EE ");
	
  // 	eq++;
  // 	break;
  //     }
  //     else {
  // 	nr += query2.value(1).toInt();
  // 	if(query.value(6).toInt() == nr) {
  // 	  updateTable("tb_details", query.value(0).toInt(), "E1 ");
  // 	  eq2++;
  // 	  nr = 0;
  // 	  break;
  // 	}
  //     }
  //   }
  //   if(!found) {
  //     ne++;
  //     updateTable("tb_details", query.value(0).toInt(), "NE ");
  //   }
  // }
  // qDebug() << "EQ=" << eq << " EQ2=" << eq2 << " NE=" << ne;

  // QSqlQuery query("SELECT * FROM tb_details0101");
  // QSqlQuery query2;
  // bool found;
  
  // while (query.next()) {
  //   query2.prepare("SELECT uid,nr FROM tb_details "
  // 		  "WHERE catNum=:catNum "
  // 		  // "AND text=:text "
  // 		  "AND qty=:qty");

  //   query2.bindValue(":catNum", query.value(1).toString());
  //   // query2.bindValue(":text",   query.value(2).toString());
  //   query2.bindValue(":qty",    query.value(3).toString());
  //   query2.exec();

  //   found = false;
  //   while (query2.next()) {
  //     found = true;
  //     break;
  //   }

  //   if(!found)
  //     updateTable("tb_details0101", query.value(0).toInt(), "NEW ");

  // }
}

void MainWindow::places()
{
  PlacesDialog dialog(this);
  dialog.exec();
}


void MainWindow::updateStatusBar()
{
  //locationLabel->setText(tr(""));
  //formulaLabel->setText(tr(""));
}

/*
void MainWindow::spreadsheetModified()
{
    setWindowModified(true);
    updateStatusBar();
}
*/


void MainWindow::createActions()
{
    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("Exit the application"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    limitsAction = new QAction(trUtf8("Лимитки.."), this);
    //limitsAction->setIcon(QIcon(":/images/find.png"));
    //findAction->setShortcut(QKeySequence::Find);
    //findAction->setStatusTip(tr("Find a matching cell"));
    connect(limitsAction, SIGNAL(triggered()), this, SLOT(limits()));

    actsAction = new QAction(trUtf8("Акты.."), this);
    connect(actsAction, SIGNAL(triggered()), this, SLOT(acts()));

    placesAction = new QAction(trUtf8("Назначения"), this);
    //aboutAction->setStatusTip(tr("Show the application's About box"));
    connect(placesAction, SIGNAL(triggered()), this, SLOT(places()));

    report1Action = new QAction(trUtf8("Движение за период.."), this);
    connect(report1Action, SIGNAL(triggered()), this, SLOT(report1()));

    report1fullyAction = new QAction(trUtf8("Движение за период (xls).."), this);
    connect(report1fullyAction, SIGNAL(triggered()), this, SLOT(report1fully()));

    report2Action = new QAction(trUtf8("Все лимитки"), this);
    connect(report2Action, SIGNAL(triggered()), this, SLOT(report2()));

    aboutAction = new QAction(tr("&About"), this);
    aboutAction->setStatusTip(tr("Show the application's About box"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAction = new QAction(tr("About &Qt"), this);
    aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(trUtf8("&Файл"));
    //fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(trUtf8("&Редактирование"));

    //editMenu->addSeparator();
    
    //editMenu->addAction(goToCellAction);

    toolsMenu = menuBar()->addMenu(trUtf8("&Инструменты"));
    toolsMenu->addAction(limitsAction);
    toolsMenu->addAction(actsAction);
    toolsMenu->addAction(placesAction);
    //toolsMenu->addAction(recalculateAction);
    //toolsMenu->addAction(sortAction);

    reportsMenu = menuBar()->addMenu(trUtf8("&Отчеты"));
    reportsMenu->addAction(report1Action);
    reportsMenu->addAction(report1fullyAction);
    reportsMenu->addAction(report2Action);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(trUtf8("&Помощь"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
}

void MainWindow::createContextMenu()
{
    // spreadsheet->addAction(cutAction);
    // spreadsheet->addAction(copyAction);
    // spreadsheet->addAction(pasteAction);
    // spreadsheet->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void MainWindow::createToolBars()
{
    // fileToolBar = addToolBar(tr("&File"));
    // fileToolBar->addAction(newAction);
    // fileToolBar->addAction(openAction);
    // fileToolBar->addAction(saveAction);

    // editToolBar = addToolBar(tr("&Edit"));
    // editToolBar->addAction(cutAction);
    // editToolBar->addAction(copyAction);
    // editToolBar->addAction(pasteAction);
    // editToolBar->addSeparator();
    // editToolBar->addAction(findAction);
    // editToolBar->addAction(goToCellAction);
}

void MainWindow::createStatusBar()
{
  locationLabel = new QLabel(trUtf8("<b>Итого: <b>"));
  locationLabel->setAlignment(Qt::AlignHCenter);
  locationLabel->setMinimumSize(locationLabel->sizeHint());

  formulaLabel = new QLabel;
  formulaLabel->setIndent(3);

  statusBar()->addWidget(locationLabel);
  statusBar()->addWidget(formulaLabel, 1);

    // connect(spreadsheet, SIGNAL(currentCellChanged(int, int, int, int)),
    //         this, SLOT(updateStatusBar()));
    // connect(spreadsheet, SIGNAL(modified()),
    //         this, SLOT(spreadsheetModified()));

  updateStatusBar();
}

void MainWindow::readSettings()
{
    // QSettings settings("Software Inc.", "Spreadsheet");

    // restoreGeometry(settings.value("geometry").toByteArray());

    // recentFiles = settings.value("recentFiles").toStringList();
    // updateRecentFileActions();

    // bool showGrid = settings.value("showGrid", true).toBool();
    // showGridAction->setChecked(showGrid);

    // bool autoRecalc = settings.value("autoRecalc", true).toBool();
    // autoRecalcAction->setChecked(autoRecalc);
}

void MainWindow::writeSettings()
{
    // QSettings settings("Software Inc.", "Spreadsheet");

    // settings.setValue("geometry", saveGeometry());
    // settings.setValue("recentFiles", recentFiles);
    // settings.setValue("showGrid", showGridAction->isChecked());
    // settings.setValue("autoRecalc", autoRecalcAction->isChecked());
}

