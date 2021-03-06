#include "disk.h"

/*
 * http://libvirt.org/formatdomain.html#elementsDisks
 */

Disk::Disk(QWidget *parent,
        virConnectPtr conn) :
    _QWidget(parent, conn)
{
    sourceLabel = new QLabel("Source:", this);
    source = new QComboBox(this);
    source->addItem(
                "Storage Volume to use as the disk", "volume");
    source->addItem(
                "Host Device to serve as the disk", "block");
    source->addItem(
                "Directory to use as the disk", "dir");
    source->addItem(
                "File holding the disk", "file");
    source->addItem(
                "Network volume/image to use as the disk", "network");
    sourceLayout = new QHBoxLayout(this);
    sourceLayout->addWidget(sourceLabel);
    sourceLayout->addWidget(source);
    sourceWdg = new QWidget(this);
    sourceWdg->setLayout(sourceLayout);

    info = new QStackedWidget(this);
    info->addWidget(new Volume_Disk(this, currWorkConnect));
    info->addWidget(new Block_Disk(this, currWorkConnect));
    info->addWidget(new Dir_Disk(this, currWorkConnect));
    info->addWidget(new File_Disk(this, currWorkConnect));
    info->addWidget(new Network_Disk(this, currWorkConnect));

    commonLayout = new QVBoxLayout(this);
    commonLayout->addWidget(sourceWdg);
    commonLayout->addWidget(info);
    commonLayout->addStretch(-1);
    setLayout(commonLayout);
    connect(source, SIGNAL(currentIndexChanged(int)),
            info, SLOT(setCurrentIndex(int)));
}

/* public slots */
QDomDocument Disk::getDataDocument() const
{
    _Disk *wdg = static_cast<_Disk*>(info->currentWidget());
    return wdg->getDataDocument();
}
