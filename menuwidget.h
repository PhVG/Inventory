#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include <QDialog>

namespace Ui {
class MenuWidget;
}

class MenuWidget : public QDialog
{
    Q_OBJECT

public:
    explicit MenuWidget(QWidget *parent = 0);
    ~MenuWidget();

private:
    Ui::MenuWidget *ui;
};

#endif // MENUWIDGET_H
