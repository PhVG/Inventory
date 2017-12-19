#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "gamefield.h"

#include <QLabel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include <QTime>
#include <QDataStream>
#include <QDebug>


MainWidget::MainWidget(NetMode mode, QWidget *parent) :
    QWidget(parent),
    m_ptcpServer{},
    m_pTcpSocket{},
    m_nNextBlockSize{},
    net_mode_{mode},
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    setWindowTitle(tr("Inventory - ") + tr(mode ? "server" : "client"));

    if(mode)//Server
    {
        qDebug() << "AS Server";
        m_ptcpServer = new QTcpServer(this);
        if (!m_ptcpServer->listen(QHostAddress::Any, 2323))
        {
            QMessageBox::critical(0,
                                  "Server Error",
                                  "Unable to start the server:"
                                  + m_ptcpServer->errorString()
                                  );
            m_ptcpServer->close();
            return;
        }

        connect(m_ptcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));

        while(!m_ptcpServer->waitForNewConnection())
        {}
    }
    else// CLIENT
    {
        qDebug() << "AS Client";
        m_pTcpSocket = new QTcpSocket(this);

        m_pTcpSocket->connectToHost("localhost", 2323);
        connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
        connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
        connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this,         SLOT(slotError(QAbstractSocket::SocketError))
               );
    }

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

    if(mode)// SERVER
    {
        db_ = new DataBase(this);
        loadGame();

        connect(inventory_, &Inventory::inventoryChanged, this, &MainWidget::slotSendToClient);
    }
    else // CLIENT
    {
        connect(inventory_, &Inventory::inventoryChanged, this, &MainWidget::slotSendToServer);
    }
}


MainWidget::~MainWidget()
{
    delete inventory_;
    delete ui;
}


void MainWidget::onMenu()
{
    auto *dlg = new MenuDialog({tr("Выход"), tr("Новая игра")});

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

    if(!net_mode_)
        slotSendToServer("reset", NULL, NULL, NULL);
}


void MainWidget::slotNewConnection()
{
    m_pTcpSocket = m_ptcpServer->nextPendingConnection();
    connect(m_pTcpSocket, SIGNAL(disconnected()), m_pTcpSocket, SLOT(deleteLater()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));
}


void MainWidget::slotReadClient() //SERVER
{// читаем сообщение от клиента и реагируем
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_5_3);

    forever
    {
        if (!m_nNextBlockSize)
        {
            if (pClientSocket->bytesAvailable() < (int)sizeof(quint16))
                break;

            in >> m_nNextBlockSize;
        }

        if (pClientSocket->bytesAvailable() < m_nNextBlockSize)
            break;

        QString command;
        int type, index, number;

        in >> command;

        if(command == "add" || command == "get")
        {
            in >> type >> index >> number;
            inventory_->changeStack(command, type, index, number);
        }
        else if(command == "reset")
        {
            inventory_->reset();
        }

        m_nNextBlockSize = 0;
    }// forever
}


void MainWidget::sendToClient(QTcpSocket* pSocket, QString command, int type, int index, int number)
{// Отправляем сообщение клиенту
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);
    out << quint16(0) << command << type << index << number;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    pSocket->write(arrBlock);
}


void MainWidget::slotReadyRead()
{// Получаем сообщение от сервера и реагируем
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_5_3);
    for (;;) {
        if (!m_nNextBlockSize) {
            if (m_pTcpSocket->bytesAvailable() < (int)sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (m_pTcpSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }

        QString command;
        int type, index, number;

        in >> command;

        if(command == "add" || command == "get")
        {
            in >> type >> index >> number;
            inventory_->changeStack(command, type, index, number);
        }
        else if(command == "reset")
        {
            inventory_->reset();
        }

        m_nNextBlockSize = 0;
    }
}

// ----------------------------------------------------------------------
void MainWidget::slotError(QAbstractSocket::SocketError err)
{
    QString strError =
        "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                     "The host was not found." :
                     err == QAbstractSocket::RemoteHostClosedError ?
                     "The remote host is closed." :
                     err == QAbstractSocket::ConnectionRefusedError ?
                     "The connection was refused." :
                     QString(m_pTcpSocket->errorString())
                    );
   // qDebug() << strError;
}

// ----------------------------------------------------------------------
void MainWidget::slotSendToServer(QString command, int type, int index, int number)
{// Отправляем сообщение на сервер
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);
    out << quint16(0) << command << type << index << number;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    m_pTcpSocket->write(arrBlock);
}


void MainWidget::slotSendToClient(QString command, int type, int index, int number)
{// Подготовка к отправке сообщения клиенту
    sendToClient(m_pTcpSocket, command, type, index, number);
}

// ------------------------------------------------------------------
void MainWidget::slotConnected()
{
}



// ----------------------------------------------------------------------
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


MenuDialog::MenuDialog(QList<QString> btnNames, QWidget* parent)
    : QDialog(parent)
{
    setLayout(new QVBoxLayout);
    layout()->setMargin(2);
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint);

    QToolButton *button = nullptr;

    for (auto name_str : btnNames)
    {
        button = new QToolButton(this);
        buttons_.push_back(button);
        button->setText(name_str);
        connect(button, &QToolButton::clicked, this, [this, button]{ done(buttons_.indexOf(button)); });
        button->setMinimumWidth(150);
        button->setMaximumWidth(150);
        layout()->addWidget(button);
    }
}

