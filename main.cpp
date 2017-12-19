#include "mainwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto *dlg = new MenuDialog({QObject::tr("Запустить в роли сервера"),
                                QObject::tr("Запустить в роли клиента")});

    int res = dlg->exec();
    NetMode net_mode;

    if(res)
        net_mode = server;
    else
        net_mode = client;

    MainWidget w(net_mode);
    w.show();

    return a.exec();
}
