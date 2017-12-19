#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QDialog>
#include <QList>
#include <QTcpSocket>


class QTcpServer;


namespace Ui {
class MainWidget;
}


enum SyncCommands
{
    add,
    get,
    resetInventory
};


enum NetMode
{
    server,
    client
};


namespace
{
    const QString dbFileName = "";
}


class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(NetMode mode, QWidget *parent = 0);
    ~MainWidget();

public slots:
    void onMenu();

    // Получаем сообщение от сервера и реагируем
    void slotReadyRead   ();
    void slotError       (QAbstractSocket::SocketError);

    // Отправляем сообщение на сервер
    void slotSendToServer(QString command, int type, int index, int number);

    // Подготовка к отправке сообщения клиенту
    void slotSendToClient(QString command, int type, int index, int number);
    void slotConnected   ();

public slots:
    // Обрабатываем подключение клиента к серверу
    void slotNewConnection();

    // читаем сообщение от клиента и реагируем
    void slotReadClient   ();

protected:
    // действия при закрытии
    void closeEvent(QCloseEvent *event);

private:
    // загружает ячейки инвентаря
    void loadGame();
    // пишет в базу ячейки инвентаря
    void saveGame();
    // обнуляет ячейки
    void resetGame();

    // Отправляем сообщение клиенту
    void sendToClient(QTcpSocket* pSocket, QString command, int type, int index, int number);

    class Inventory *inventory_;

    class DataBase *db_;
    QTcpServer* m_ptcpServer;
    QTcpSocket* m_pTcpSocket;
    quint16     m_nNextBlockSize;
    NetMode     net_mode_;

    Ui::MainWidget *ui;
};


class DataBase : public QObject
{
    Q_OBJECT

public:
    explicit DataBase(QWidget *parent = 0);
    ~DataBase();

    void loadData(QList<class ItemStack*>& list);
    void saveData(QList<class ItemStack*>& list);
};


class MenuDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MenuDialog(QList<QString> btnNames, QWidget* parent = 0);
    ~MenuDialog(){}

//private slots:
//	void on_btn();

private:
    QList<class QToolButton*> buttons_;

};

#endif // MAINWIDGET_H
