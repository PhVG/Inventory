#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class DataBase : public QObject
{
    Q_OBJECT

public:
    explicit DataBase(QWidget *parent = 0);
    ~DataBase();
};


class ItemCell : public QWidget
{
public:
    explicit ItemCell(QWidget *parent = 0);
    ~ItemCell();

};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
};

#endif // MAINWINDOW_H
