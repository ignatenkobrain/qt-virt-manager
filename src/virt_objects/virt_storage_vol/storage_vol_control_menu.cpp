#include "storage_vol_control_menu.h"

StorageVolControlMenu::StorageVolControlMenu(QWidget *parent, QStringList params, bool state) :
    QMenu(parent), parameters(params), autoReloadState(state)
{
    if ( !parameters.isEmpty() ) {
        delete_Action = new QAction("Delete", this);
        delete_Action->setIcon(QIcon::fromTheme("storageVol-delete"));
        download_Action = new QAction("Download", this);
        download_Action->setIcon(QIcon::fromTheme("storageVol-download"));
        resize_Action = new QAction("Resize", this);
        resize_Action->setIcon(QIcon::fromTheme("storageVol-resize"));
        upload_Action = new QAction("Upload", this);
        upload_Action->setIcon(QIcon::fromTheme("storageVol-upload"));
        wipe_Menu = new WipeMenu(this);
        wipe_Action = new QAction("Wipe", this);
        wipe_Action->setIcon(QIcon::fromTheme("storageVol-wipe"));
        wipe_Action->setMenu(wipe_Menu);
        connect(wipe_Menu, SIGNAL(execMethod(const QStringList&)),
                this, SIGNAL(execMethod(const QStringList&)));
        getXMLDesc_Action = new QAction("get XML Description", this);
        getXMLDesc_Action->setIcon(QIcon::fromTheme("storageVol-xml"));

        addAction(delete_Action);
        addAction(download_Action);
        addAction(resize_Action);
        addAction(upload_Action);
        addAction(wipe_Action);
        addSeparator();
        addAction(getXMLDesc_Action);
        addSeparator();
    };
    reload = new QAction("Reload Volume OverView", this);
    reload->setIcon(QIcon::fromTheme("view-refresh"));
    reload->setEnabled(!autoReloadState);

    addAction(reload);
    connect(this, SIGNAL(triggered(QAction*)), this, SLOT(emitExecMethod(QAction*)));
}
StorageVolControlMenu::~StorageVolControlMenu()
{
    disconnect(this, SIGNAL(triggered(QAction*)), this, SLOT(emitExecMethod(QAction*)));
    if ( !parameters.isEmpty() ) {
        disconnect(wipe_Menu, SIGNAL(execMethod(const QStringList&)),
                   this, SIGNAL(execMethod(const QStringList&)));
        delete delete_Action;
        delete_Action = NULL;
        delete download_Action;
        download_Action = NULL;
        delete resize_Action;
        resize_Action = NULL;
        delete upload_Action;
        upload_Action = NULL;
        delete wipe_Menu;
        wipe_Menu = NULL;
        delete wipe_Action;
        wipe_Action = NULL;
        delete getXMLDesc_Action;
        getXMLDesc_Action = NULL;
    };
    delete reload;
    reload = NULL;
}
void StorageVolControlMenu::emitExecMethod(QAction *action)
{
    QStringList paramList;
    if ( !parameters.isEmpty() ) {
        if ( action == resize_Action) {
            paramList.append("resizeVirtStorageVol");
        } else if ( action == delete_Action ) {
            paramList.append("deleteVirtStorageVol");
        } else if ( action == download_Action ) {
            paramList.append("downloadVirtStorageVol");
        } else if ( action == upload_Action ) {
            paramList.append("uploadVirtStorageVol");
        } else if ( action == wipe_Action ) {
            paramList.append("wipeVirtStorageVol");
            paramList.append("0");
        } else if ( action == getXMLDesc_Action ) {
            paramList.append("getVirtStorageVolXMLDesc");
        } else if ( action == reload ) {
            paramList.append("reloadVirtStorageVol");
        } else return;
    } else if ( action == reload ) {
        paramList.append("reloadVirtStorageVol");
    } else return;
    //qDebug()<<paramList<<"paramList from menu";
    emit execMethod(paramList);
}
