#include "filesystems_edit.h"

FileSystems_Edit::FileSystems_Edit(
        QWidget *parent,
        virConnectPtr conn) :
    FileSystems(parent, conn)
{
    connect(type, SIGNAL(currentIndexChanged(int)),
            this, SLOT(stateChanged()));
    for (uint i=0; i<info->count(); i++) {
        connect(info->widget(i), SIGNAL(dataChanged()),
                this, SLOT(stateChanged()));
    };
}

/* public slots */
void FileSystems_Edit::setDataDescription(QString &xmlDesc)
{
    //qDebug()<<xmlDesc;
    QDomDocument doc;
    doc.setContent(xmlDesc);
    QDomElement _device;
    _device = doc.firstChildElement("device")
            .firstChildElement("filesystem");
    QString _type = _device.attribute("type");
    int idx = type->findText(_type, Qt::MatchContains);
    type->setCurrentIndex( (idx<0)? 0:idx );
    type->setEnabled(false);
    _QWidget *wdg = static_cast<_QWidget*>(info->currentWidget());
    if ( NULL!=wdg ) wdg->setDataDescription(xmlDesc);
}
