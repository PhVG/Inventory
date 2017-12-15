#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QDialog>
#include <QList>


namespace Ui {
class MainWidget;
}


namespace
{
    const QString dbFileName = "";
}


class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

private slots:
    void onMenu();

protected:
    void closeEvent(QCloseEvent *event);

private:
    // загружает ячейки инвентаря
    void loadGame();
    // пишет в базу ячейки инвентаря
    void saveGame();
    // обнуляет ячейки
    void resetGame();

    class Inventory *inventory_;

    class DataBase *db_;
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
enum
{
    ExitGame,
    NewGame
};

public:
    explicit MenuDialog(QWidget* parent = 0);
    ~MenuDialog(){}

//private slots:
//	void on_btn();

private:
    QList<class QToolButton*> buttons_;

};

#endif // MAINWIDGET_H
