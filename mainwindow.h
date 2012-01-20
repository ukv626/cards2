#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QLabel;
class DetailsWindow;
class QSqlQuery;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void limits();
    void report1();
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
    QAction *placesAction;
    QAction *report1Action;
    QAction *report2Action;
    QAction *aboutAction;
    QAction *aboutQtAction;

    QSqlQuery *query;
    QString html;
};

#endif
