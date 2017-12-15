#ifndef MIMEDATA_H
#define MIMEDATA_H

#include <QDrag>
#include <QMimeData>
#include <QWidget>


class MimeData : public QMimeData
{
public:
    MimeData() : QMimeData() {}
    virtual ~MimeData() {}

    static QString mimeType() { return "InventoryItem"; }
    void setCount(int c) { count_ = c; }
    int getCount() const { return count_; }
    void setType(int t) { type_ = t; }
    int getType() const { return type_; }
    void setWidget(QWidget *wgt);
    QWidget* widget() const { return widget_; }

private:
    QWidget* widget_;
    int count_;
    int type_;
};


class WidgetDrag : public QDrag
{
public:
    WidgetDrag(QWidget* source_widget = 0) : QDrag(source_widget) {}

    void setWidget(QWidget* wgt);
};
#endif // MIMEDATA_H
