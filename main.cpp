#include <QApplication>
#include <QtSql>
#include <QMessageBox>

#include "mainwindow.h"

bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./tmp/db_cards.db");

/*
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setPort(3326);
    db.setDatabaseName("db_cards");
    db.setUserName("gm_user");
    db.setPassword("gm_user");
*/
    
    if (!db.open()) {
        QMessageBox::warning(0, QObject::tr("Database Error"),
                             db.lastError().text());
        return false;
    }
    return true;
}


int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  if (!createConnection())
        return 1;

  MainWindow mainWin;
  //mainWin.resize(1500,800);
  mainWin.setWindowState(Qt::WindowMaximized);
  mainWin.show();

  return app.exec();
}
