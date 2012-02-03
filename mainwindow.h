#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QLabel;
class DetailsWindow;
class QSqlQuery;
class QDate;
class QFile;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void detailsTypeChanged(int TypeId);
    void limits();
    void acts();
    void report1();
    void report1fully();
    void report2();
    void report1Print(QPrinter * printer);
    void report2Print(QPrinter * printer);
    void about();
    void places();
    void updateStatusBar();

private:
    void createActions();
    void createMenus();
    void createContextMenu();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    void report1fully_(const QDate &date1, const QDate &date2,
		       bool mflag, QFile *file);
    /* void updateTable(const QString &table, int uid, const QString &text); */

    DetailsWindow *detailsWindow;
    QLabel *locationLabel;
    QLabel *formulaLabel;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *toolsMenu;
    QMenu *reportsMenu;
    QMenu *helpMenu;
    
    //QToolBar *fileToolBar;
    //QToolBar *editToolBar;

    QAction *exitAction;
    QAction *limitsAction;
    QAction *actsAction;
    QAction *placesAction;
    QAction *report1Action;
    QAction *report1fullyAction;
    QAction *report2Action;
    QAction *aboutAction;
    QAction *aboutQtAction;

    QSqlQuery *query;
    QString html;
};

#endif
