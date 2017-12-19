#include "gamefield.h"
#include "mimedata.h"
#include <QMediaPlayer>
#include <QtWidgets>



Inventory::Inventory(int size, QObject *parent)
    : QObject(parent)
{
    for(int i = 0; i < size; i++)
    {
        itemStacks_.push_back(new ItemStack);
        connect(itemStacks_.last(), &ItemStack::itemChanged, this, &Inventory::slotStackChanged);
    }
}


Inventory::~Inventory()
{
    for(auto it: itemStacks_)
        delete it;
}


void Inventory::reset()
{
    for(auto *it: itemStacks_)
    {
        it->get(it->count());
    }
}


void Inventory::changeStack(QString command, int type, int index, int number)
{

    if(command == "add")
        itemStacks_[index]->add((TypeOfItem)type, number, false);
    else if(command == "get")
        itemStacks_[index]->get(number, false);
}


void Inventory::slotStackChanged(QString command, int number)
{
    auto *itemStack = dynamic_cast<ItemStack*>(sender());
    emit inventoryChanged(command, itemStack->getType(), itemStacks_.indexOf(itemStack), number);
}


//--------------------------------------------------
ItemStack::ItemStack(QObject *parent)
  : QObject(parent),
    type_{},
    uncountable_{}
{
    widget_ = new ItemStackWidget(nullptr, this);
}


ItemStack::~ItemStack()
{

}


bool ItemStack::add(TypeOfItem type, int number, bool key)
{
    if(uncountable_)
        return true;

    if(type == none || (type_ != none && type_ != type))
        return false;

    if(number > 0)
    {
        for(int i = 0; i < number; i++)
        {
            items_.push_back(FruitFactory::createFruit(type));
        }

        type_ = type;

        widget_->setPixmap(QPixmap(items_[0]->getPicturePath()));
    }

    if(key)
        emit itemChanged("add", number);

    return true;
}


void ItemStack::get(int number, bool key)
{
    if(uncountable_)
        return;

    if(number >= items_.count())
    {
        for(auto it: items_)
            delete it;

        items_.clear();
        type_ = none;
        getWidget()->setPixmap(QPixmap());
    }
    else if (number > 0)
    {
        for(int i = 0; i < number; i++)
        {
            delete items_.back();
            items_.pop_back();
        }
    }

    if(key)
        emit itemChanged("get", number);
}


void ItemStack::setUncountable(bool b)
{
    uncountable_ = b;

    if(b)
    {
        items_.clear();
        items_.push_back(FruitFactory::createFruit(apple));
        type_ = apple;
        widget_->setPixmap(items_[0]->getPicturePath());
    }
}


//--------------------------------------------------
ItemStackWidget::ItemStackWidget(QWidget *parent, ItemStack *baseStack)
    : QLabel{parent},
      drag_pos_{},
      player_{}
{
    setMinimumSize(64,64);
    setMaximumSize(64,64);
    player_ = new QMediaPlayer(this);
    baseStack_ = baseStack;
    setAcceptDrops(true);
}


ItemStackWidget::~ItemStackWidget()
{
    delete player_;
}


void ItemStackWidget::paintEvent(QPaintEvent *e)
{
    QLabel::paintEvent(e);

    QPainter p(this);
    QString str;

    if(getBaseStack()->getType() && !getBaseStack()->getUncountable())
        str = QString::number(getBaseStack()->count());
    else
        str = "";

    QFontMetrics fm(font());
    auto txtWidth = fm.width(str);
    p.drawText(width() - txtWidth - 2, height() - 2, str);
}


void ItemStackWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        drag_pos_ = event->pos();

    if(getBaseStack()->getUncountable())
        return;

    if(getBaseStack()->count() <= 0)
        return QLabel::mousePressEvent(event);

    if (event->button() == Qt::RightButton)
    {
        getBaseStack()->get(1);

        player_->setMedia(QUrl("qrc:/resources/eat.mp3"));
        player_->setVolume(50);
        player_->play();

        QLabel::mousePressEvent(event);
    }
}


void ItemStackWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        int distance = (event->pos() - drag_pos_).manhattanLength();

        if (distance > QApplication::startDragDistance())
            startDrag();
    }

    QWidget::mouseMoveEvent(event);
}


void ItemStackWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(MimeData::mimeType()))
        event->acceptProposedAction();
}


void ItemStackWidget::dropEvent(QDropEvent *event)
{
    const auto* mimedata = static_cast<const MimeData*>(event->mimeData());

    if (mimedata)
        getBaseStack()->add((TypeOfItem)mimedata->getType(), mimedata->getCount());
}


void ItemStackWidget::startDrag()
{
    auto* drag = new WidgetDrag(this);
    auto *mimedata = new MimeData;
    mimedata->setWidget(this);
    mimedata->setType((int)getBaseStack()->getType());
    mimedata->setCount(getBaseStack()->count());
    drag->setMimeData(mimedata);
    drag->exec(Qt::CopyAction);
    getBaseStack()->get(getBaseStack()->count());
    //забрали сюда всё (если не бесконечный)
}


//--------------------------------------------------
AbstractItem *FruitFactory::createFruit(TypeOfItem type)
{
    AbstractItem* fruit = nullptr;

    switch(type)
    {
    case apple:
        fruit = new AppleItem();
        break;
    default:
        break;
    }

    return fruit;
}
