#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "gamefield.h"

#include <QLabel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>


MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    auto aloneItem = new ItemStack();
    aloneItem->setUncountable(true);
    ui->wGameFiekd->setLayout(new QVBoxLayout);
    ui->wGameFiekd->layout()->addWidget(aloneItem->getWidget());
    aloneItem->getWidget()->show();

    ui->twInventory->setSelectionMode(QAbstractItemView::NoSelection);

    inventory_ = new Inventory(9);

    auto item = inventory_->getItems().begin();

    for(int i = 0; i < 3; ++i)
    {
        for(int j = 0; j < 3; ++j)
        {
            auto wgt = (*item)->getWidget();
            wgt->setParent(ui->twInventory);
            ui->twInventory->setCellWidget(i, j, wgt);
            item++;
        }
    }

    connect(ui->tbMenu, &QToolButton::clicked, this, &MainWidget::onMenu);

    db_ = new DataBase(this);
    loadGame();
}


MainWidget::~MainWidget()
{
    delete inventory_;
    delete ui;
}


void MainWidget::onMenu()
{
    auto *dlg = new MenuDialog();

    int res = dlg->exec();

    if(res)
        resetGame();
    else
        close();
}

void MainWidget::closeEvent(QCloseEvent *event)
{
    saveGame();
    QWidget::closeEvent(event);
}

void MainWidget::loadGame()
{
    db_->loadData(inventory_->getItems());
    //из базы
}

void MainWidget::saveGame()
{
    db_->saveData(inventory_->getItems());
    //в базу
}

void MainWidget::resetGame()
{
    inventory_->reset();
    //обнуление ячеек инвентаря
}


DataBase::DataBase(QWidget *parent)
    : QObject(parent)
{
}

DataBase::~DataBase()
{
}


void DataBase::loadData(QList<ItemStack *> &list)
{
   //Подключаем базу данных
   QSqlDatabase db;
   db = QSqlDatabase::addDatabase("QSQLITE");
   db.setDatabaseName(QCoreApplication::applicationDirPath() + "\\inventory.db3");
   db.open();
   //Осуществляем запрос
   QSqlQuery query;
   query.exec("SELECT Inventory._id, Inventory.Item_id, Inventory.Number FROM Inventory");

   //Заполняем инвентарь
   while (query.next() && query.value(0).toInt() <= list.count())
   {
       list[query.value(0).toInt()-1]->add((TypeOfItem)query.value(1).toInt(), query.value(2).toInt());
   }

   db.close();
}

void DataBase::saveData(QList<ItemStack *> &list)
{
    //Подключаем базу данных
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QCoreApplication::applicationDirPath() + "\\inventory.db3");
    db.open();
    //Осуществляем запрос
    QSqlQuery query;
    for(int i = 0; i < list.count(); i++)
    {
        query.clear();
        query.prepare("UPDATE [Inventory] SET "
          "[Item_id] = :itemId, "
          "[Number] = :number "
          "WHERE [_id] = :id");

        query.bindValue(":itemId", (int)list[i]->getType());
        query.bindValue(":number", list[i]->count());
        query.bindValue(":id", i + 1);

        query.exec();
        db.commit();
    }

    db.close();
}


MenuDialog::MenuDialog(QWidget* parent)
    : QDialog(parent)
{
    setLayout(new QVBoxLayout);
    layout()->setMargin(2);
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint);

    auto *btn = new QToolButton(this);
    buttons_.push_back(btn);
    btn->setText(tr("Выход"));

    btn = new QToolButton(this);
    buttons_.push_back(btn);
    btn->setText(tr("Новая игра"));

    for (auto *button : buttons_)
    {
        connect(button, &QToolButton::clicked, this, [this, button]{ done(buttons_.indexOf(button)); });
        button->setMinimumWidth(150);
        button->setMaximumWidth(150);
        layout()->addWidget(button);
    }
}

