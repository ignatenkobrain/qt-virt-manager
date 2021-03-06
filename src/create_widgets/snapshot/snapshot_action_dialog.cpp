#include "snapshot_action_dialog.h"

/*
 * https://libvirt.org/html/libvirt-libvirt-domain-snapshot.html
 */

SnapshotActionDialog::SnapshotActionDialog(
        QWidget         *parent,
        virConnectPtr    currConnect,
        QString          _domName) :
    QDialog(parent),
    currJobConnect(currConnect),
    domName(_domName)
{
    params.clear();
    QString winTitle = QString("<%1> Snapshot Actions").arg(domName);
    setWindowTitle(winTitle);
    revertFlagsMenu = new RevertSnapshotFlags(this);
    revertAction = new QAction(QIcon::fromTheme("document-revert"), "Revert To", this);
    revertAction->setMenu(revertFlagsMenu);
    deleteFlagsMenu = new DeleteSnapshotFlags(this);
    deleteAction = new QAction(QIcon::fromTheme("list-remove"), "Delete", this);
    deleteAction->setMenu(deleteFlagsMenu);
    refreshAction = new QAction(QIcon::fromTheme("view-refresh"), "Refresh", this);
    getXMLDescAction = new QAction(QIcon::fromTheme("application-xml"), "get XML Description", this);
    toolBar = new QToolBar(this);
    toolBar->addAction(revertAction);
    toolBar->addAction(deleteAction);
    toolBar->addAction(refreshAction);
    toolBar->addAction(getXMLDescAction);
    model = new SnapshotTreeModel;
    snapshotTree = new QTreeView(this);
    snapshotTree->setRootIsDecorated(true);
    snapshotTree->setModel(model);
    snapshotTree->setExpandsOnDoubleClick(true);
    info = new QLabel(
    "<a href='https://libvirt.org/html/libvirt-libvirt-domain-snapshot.html'>About</a>",
                this);
    info->setOpenExternalLinks(true);
    ok = new QPushButton("Ok", this);
    cancel = new QPushButton("Cancel", this);
    closeWdg = new QPushButton("Close", this);
    buttonsLayout = new QHBoxLayout(this);
    buttonsLayout->addWidget(info);
    buttonsLayout->addWidget(ok);
    buttonsLayout->addWidget(cancel);
    buttonsLayout->addWidget(closeWdg);
    buttonsWdg = new QWidget(this);
    buttonsWdg->setLayout(buttonsLayout);
    commonLayout = new QVBoxLayout(this);
    commonLayout->addWidget(toolBar);
    commonLayout->addWidget(snapshotTree);
    commonLayout->addWidget(buttonsWdg);
    commonLayout->addStretch(-1);
    setLayout(commonLayout);
    connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(cancelled()));
    connect(closeWdg, SIGNAL(clicked()), this, SLOT(reject()));
    setDomainSnapshots();
    connect(toolBar, SIGNAL(actionTriggered(QAction*)),
            this, SLOT(detectTriggeredAction(QAction*)));
    changeDialogState(false);
}
SnapshotActionDialog::~SnapshotActionDialog()
{
    if ( NULL!=domain ) virDomainFree(domain);
}

/* public slots */
QStringList SnapshotActionDialog::getParameters() const
{
    return params;
}

/* private slots */
void SnapshotActionDialog::clearSnapshotTree()
{
    while ( model->rootItemChildCount()>0 ) {
        model->removeRow(0, snapshotTree->rootIndex());
    };
}
void SnapshotActionDialog::addSnapshotChild(int row, const QModelIndex &parent, char *name)
{
    // flags: extra flags; not used yet, so callers should always pass 0
    virDomainSnapshotPtr snapShot =
            virDomainSnapshotLookupByName(domain, name, 0);
    // flags: extra flags; not used yet, so callers should always pass 0
    int current = virDomainSnapshotIsCurrent(snapShot, 0);
    if ( NULL!=snapShot ) {
        model->insertRow(row, parent);
        model->setData(model->index(row, 0, parent), name, Qt::DisplayRole);
        model->setData(model->index(row, 0, parent), (current>0), Qt::DecorationRole);
        // By default, this command covers only direct children; use flag=0
        int namesLen = virDomainSnapshotNumChildren(snapShot, 0);
        if ( namesLen>0 ) {
            char *names;
            int ret = virDomainSnapshotListChildrenNames(
                        snapShot, &names, namesLen, 0);
            if ( ret>0 ) {
                for (uint i = 0; i<ret; i++) {
                    addSnapshotChild(i, model->index(row, 0, parent), (&names)[i]);
                };
            };
        };
        virDomainSnapshotFree(snapShot);
    };
}
void SnapshotActionDialog::setDomainSnapshots()
{
    domain = virDomainLookupByName(
                currJobConnect, domName.toUtf8().data());
    int namesLen = virDomainSnapshotNum(
                domain, VIR_DOMAIN_SNAPSHOT_LIST_ROOTS);
    if ( namesLen>0 ) {
        char *names;
        int ret = virDomainSnapshotListNames(
                    domain, &names, namesLen, VIR_DOMAIN_SNAPSHOT_LIST_ROOTS);
        if ( ret>0 ) {
            for (uint i = 0; i<ret; i++) {
                addSnapshotChild(i, snapshotTree->rootIndex(), (&names)[i]);
            };
        };
    };
}
void SnapshotActionDialog::accept()
{
    TreeItem *item = static_cast<TreeItem*>(
            snapshotTree->currentIndex().internalPointer());
    if ( NULL==item ) {
        cancelled();
    } else {
        params.append(item->data(0).toString());
        params.append(QString::number(flags));
        //qDebug()<<params;
        done(result());
    };
}
void SnapshotActionDialog::reject()
{
    done(0);
}
void SnapshotActionDialog::cancelled()
{
    flags = 0;
    params.clear();
    setResult(0);
    changeDialogState(false);
}
void SnapshotActionDialog::changeDialogState(bool state)
{
    snapshotTree->setEnabled(!state);
    toolBar->setEnabled(!state);
    ok->setEnabled(state);
    cancel->setEnabled(state);
    closeWdg->setEnabled(!state);
}
void SnapshotActionDialog::detectTriggeredAction(QAction *act)
{
    if ( act==revertAction ) {
        changeDialogState(true);
        params.append("revertVirtDomainSnapshot");
        flags = revertFlagsMenu->getCompositeFlag();
        setResult(REVERT_TO_DOMAIN_SNAPSHOT);
    } else if ( act==deleteAction ) {
        changeDialogState(true);
        params.append("deleteVirtDomainSnapshot");
        flags = deleteFlagsMenu->getCompositeFlag();
        setResult(DELETE_DOMAIN_SNAPSHOT);
    } else if ( act==refreshAction ) {
        snapshotTree->setEnabled(false);
        clearSnapshotTree();
        setDomainSnapshots();
        snapshotTree->setEnabled(true);
    } else if ( act==getXMLDescAction ) {
        showSnapsotXMLDesc();
    };
}
void SnapshotActionDialog::showSnapsotXMLDesc()
{
    if ( snapshotTree->currentIndex().isValid() ) {
        TreeItem *item = static_cast<TreeItem*>(
                snapshotTree->currentIndex().internalPointer());
        if ( NULL!=item ) {
            // flags: extra flags; not used yet, so callers should always pass 0
            virDomainSnapshotPtr snapShot =
                    virDomainSnapshotLookupByName(
                        domain, item->data(0).toByteArray().data(), 0);
            char *xmlDesc = virDomainSnapshotGetXMLDesc(snapShot, 0);
            if ( NULL!=xmlDesc ) {
                QTemporaryFile f;
                f.setAutoRemove(false);
                f.setFileTemplate(
                            QString("%1%2XML_Desc-XXXXXX.xml")
                            .arg(QDir::tempPath())
                            .arg(QDir::separator()));
                bool read = f.open();
                if (read) f.write(xmlDesc);
                QString xml = f.fileName();
                f.close();
                free(xmlDesc);
                QDesktopServices::openUrl(QUrl(xml));
            };
        };
    };
}
