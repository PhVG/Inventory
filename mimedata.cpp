#include "mimedata.h"



void MimeData::setWidget(QWidget* wgt)
{
    widget_ = wgt;
    setData(mimeType(), QByteArray());
}

//---------------------------------------
void WidgetDrag::setWidget(QWidget* wgt)
{
     auto* mimedata = new MimeData;
     mimedata->setWidget(wgt);
     setMimeData(mimedata);
}

