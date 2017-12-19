#ifndef GAMEFIELD_H
#define GAMEFIELD_H
#include <QLabel>
#include <qstring.h>
#include <QString>
#include <QMap>
#include <QPoint>


enum TypeOfItem
{
    none,
    apple
};


class Inventory : public QObject
{
    Q_OBJECT
public:
    Inventory(int size, QObject *parent = 0);
    ~Inventory();

    //возвращает список айтемов
    QList<class ItemStack*> &getItems() { return itemStacks_; }

    //обнуляет количество предметов в ячейках
    void reset();
    void changeStack(QString command, int type, int index, int number);

private slots:
    void slotStackChanged(QString command, int number);

signals:
    void inventoryChanged(QString command, int type, int index, int number);

private:
    QList<class ItemStack*> itemStacks_;
};


class ItemStack : public QObject
{
    Q_OBJECT
public:
    explicit ItemStack(QObject *parent = 0);
    ~ItemStack();

    // добавляет предметы
    bool add(TypeOfItem type, int number, bool key = true);

    // удаялет предметы
    void get(int number, bool key = true);

    // возвращает тип предмета
    TypeOfItem getType() { return type_; }

    // возвращает количество предметов
    int count() { return items_.count(); }

    // преключает в режим бесконечного
    void setUncountable(bool b);

    // возвращает режим (бесконечный или обычный)
    bool getUncountable() { return uncountable_; }

    // возвращает виджет изображения
    class ItemStackWidget *getWidget() {return widget_; }

signals:
    void itemChanged(QString command, int number);

private:
    QList<class AbstractItem*> items_;

    class ItemStackWidget *widget_;
    TypeOfItem type_;
    bool uncountable_;
};


class ItemStackWidget : public QLabel
{
    Q_OBJECT

public:
    explicit ItemStackWidget(QWidget *parent = 0, ItemStack *baseStack = nullptr);
    ~ItemStackWidget();

    ItemStack *getBaseStack() { return baseStack_; }

protected:
    void paintEvent     (QPaintEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent (QMouseEvent*);
    void dragEnterEvent (QDragEnterEvent*);
    void dropEvent      (QDropEvent*);

private:
    // действия в начале перетаскивания
    void startDrag();

    QPoint drag_pos_;
    class QMediaPlayer *player_;
    ItemStack *baseStack_;

};


class AbstractItem
{
public:
    AbstractItem(){}
    virtual ~AbstractItem(){}

    // возвращает путь к иконке предмета
    virtual QString getPicturePath() = 0;
    // возвращает тип предмета
    virtual TypeOfItem getType() = 0;
};


class AppleItem : public AbstractItem
{
public:
    AppleItem(){}
    ~AppleItem(){}

    /*virtual */QString getPicturePath() override { return ":/apple.png"; }
    /*virtual */TypeOfItem getType() override {return apple; }
};


class FruitFactory
{
public:
    // возвращает потомка AbstractItem переданного типа
    static AbstractItem *createFruit(TypeOfItem);
};


#endif // GAMEFIELD_H
