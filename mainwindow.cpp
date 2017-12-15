#include "mainwindow.h"
#include "qsql.h"



DataBase::DataBase(QWidget *parent)
    : QObject(parent)
{
}

DataBase::~DataBase()
{
}


ItemCell::ItemCell(QWidget *parent)
    : QWidget(parent)
{

}

ItemCell::~ItemCell()
{

}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
}

MainWindow::~MainWindow()
{
}
