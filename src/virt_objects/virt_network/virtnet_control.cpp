#include "virtnet_control.h"

VirtNetControl::VirtNetControl(QWidget *parent) :
    VirtEntityControl(parent)
{
    setObjectName("VirtNetControl");
    setWindowTitle("VirtNet Control");
    setWindowIcon(QIcon::fromTheme("network"));
    virtNetModel = new VirtNetModel();
    entityList->setModel(virtNetModel);
    //connect(entityList, SIGNAL(doubleClicked(const QModelIndex&)),
    //        this, SLOT(entityDoubleClicked(const QModelIndex&)));
    connect(entityList, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(entityClicked(const QPoint&)));
    settings.beginGroup("VirtNetControl");
    entityList->setColumnWidth(0, settings.value("column0", 132).toInt());
    entityList->setColumnWidth(1, settings.value("column1", 32).toInt());
    entityList->setColumnWidth(2, settings.value("column2", 32).toInt());
    entityList->setColumnWidth(3, settings.value("column3", 32).toInt());
    int area_int = settings.value("ToolBarArea", 4).toInt();
    settings.endGroup();
    toolBar = new VirtNetToolBar(this);
    addToolBar(toolBar->get_ToolBarArea(area_int), toolBar);
    connect(toolBar, SIGNAL(fileForMethod(const QStringList&)),
            this, SLOT(newVirtEntityFromXML(const QStringList&)));
    connect(toolBar, SIGNAL(execMethod(const QStringList&)),
            this, SLOT(execAction(const QStringList&)));
}
VirtNetControl::~VirtNetControl()
{
    settings.beginGroup("VirtNetControl");
    settings.setValue("column0", entityList->columnWidth(0));
    settings.setValue("column1", entityList->columnWidth(1));
    settings.setValue("column2", entityList->columnWidth(2));
    settings.setValue("column3", entityList->columnWidth(3));
    settings.setValue("ToolBarArea", toolBarArea(toolBar));
    settings.endGroup();
    settings.sync();
    //disconnect(entityList, SIGNAL(doubleClicked(const QModelIndex&)),
    //           this, SLOT(entityDoubleClicked(const QModelIndex&)));
    disconnect(entityList, SIGNAL(customContextMenuRequested(const QPoint&)),
               this, SLOT(entityClicked(const QPoint&)));
    disconnect(toolBar, SIGNAL(fileForMethod(const QStringList&)),
               this, SLOT(newVirtEntityFromXML(const QStringList&)));
    disconnect(toolBar, SIGNAL(execMethod(const QStringList&)),
               this, SLOT(execAction(const QStringList&)));

    stopProcessing();

    delete toolBar;
    toolBar = NULL;

    if (virtNetModel!=NULL) {
        delete virtNetModel;
        virtNetModel = NULL;
    };

    if (entityList!=NULL) {
        delete entityList;
        entityList = NULL;
    };
}

/* public slots */
bool VirtNetControl::getThreadState() const
{
    return true;
}
void VirtNetControl::stopProcessing()
{
    // clear Network list
    while ( virtNetModel->DataList.count() ) {
        virtNetModel->removeRow(0);
    };
    virtNetModel->setHeaderData(0, Qt::Horizontal, QString("Name"), Qt::EditRole);
    setEnabled(false);

}
bool VirtNetControl::setCurrentWorkConnect(virConnect *conn)
{
    stopProcessing();
    currWorkConnect = conn;
    toolBar->enableAutoReload();
    return true;
}
void VirtNetControl::setListHeader(QString &connName)
{
    virtNetModel->setHeaderData(0, Qt::Horizontal, QString("Name (Conn: \"%1\")")
                                .arg(connName), Qt::EditRole);
    currConnName = connName;
    setEnabled(true);
    // for initiation content
    QStringList args;
    args.prepend("reloadVirtNetwork");
    args.prepend(QString::number(GET_ALL_ENTITY));
    args.prepend(currConnName);
    emit addNewTask(currWorkConnect, args);
}
void VirtNetControl::resultReceiver(Result data)
{
    //qDebug()<<data.action<<data.name<<"result";
    if ( data.action == GET_ALL_ENTITY ) {
        if ( data.msg.count() > virtNetModel->DataList.count() ) {
            int _diff = data.msg.count() - virtNetModel->DataList.count();
            for ( int i = 0; i<_diff; i++ ) {
                virtNetModel->insertRow(1);
                //qDebug()<<i<<"insert";
            };
        };
        if ( virtNetModel->DataList.count() > data.msg.count() ) {
            int _diff = virtNetModel->DataList.count() - data.msg.count();
            for ( int i = 0; i<_diff; i++ ) {
                virtNetModel->removeRow(0);
                //qDebug()<<i<<"remove";
            };
        };
        int i = 0;
        foreach (QString _data, data.msg) {
            QStringList chain = _data.split(DFR);
            if (chain.isEmpty()) continue;
            int count = chain.size();
            for (int j=0; j<count; j++) {
                virtNetModel->setData(virtNetModel->index(i,j), chain.at(j), Qt::EditRole);
            };
            i++;
        };
    } else if ( data.action == GET_XML_DESCRIPTION ) {
        if ( !data.msg.isEmpty() ) {
            QString xml = data.msg.first();
            data.msg.removeFirst();
            data.msg.append(QString("to <a href='%1'>%1</a>").arg(xml));
            QString msg = data.msg.join(" ");
            msgRepeater(msg);
            QDesktopServices::openUrl(QUrl(xml));
        };
    } else if ( data.action < GET_XML_DESCRIPTION ) {
        if ( !data.msg.isEmpty() ) {
            QString msg = data.msg.join(" ");
            msgRepeater(msg);
        };
        if ( data.result ) {
            QStringList args;
            args.prepend("reloadVirtNetwork");
            args.prepend(QString::number(GET_ALL_ENTITY));
            args.prepend(currConnName);
            emit addNewTask(currWorkConnect, args);
            // for different action's specified manipulation
            switch (data.action) {
            case _EMPTY_ACTION:
                // some job;
                break;
            default:
                break;
            }
        };
    };
}

/* private slots */
void VirtNetControl::changeDockVisibility()
{
    toolBar->setEnabled( !toolBar->isEnabled() );
    entityList->setEnabled( !entityList->isEnabled() );
}
void VirtNetControl::entityClicked(const QPoint &p)
{
    //qDebug()<<"custom Menu request";
    QModelIndex idx = entityList->indexAt(p);
    QStringList params;
    if ( idx.isValid() && virtNetModel->DataList.count()>idx.row() ) {
        //qDebug()<<virtNetModel->DataList.at(idx.row())->getName();
        params<<virtNetModel->DataList.at(idx.row())->getName();
        params<<virtNetModel->DataList.at(idx.row())->getState();
        params<<virtNetModel->DataList.at(idx.row())->getAutostart();
        params<<virtNetModel->DataList.at(idx.row())->getPersistent();
    } else {
        entityList->clearSelection();
    };
    bool state = toolBar->getAutoReloadState();
    VirtNetControlMenu *netControlMenu = new VirtNetControlMenu(this, params, state);
    connect(netControlMenu, SIGNAL(execMethod(const QStringList&)),
            this, SLOT(execAction(const QStringList&)));
    netControlMenu->move(QCursor::pos());
    netControlMenu->exec();
    disconnect(netControlMenu, SIGNAL(execMethod(const QStringList&)),
               this, SLOT(execAction(const QStringList&)));
    netControlMenu->deleteLater();
}
void VirtNetControl::entityDoubleClicked(const QModelIndex &index)
{
    if ( index.isValid() ) {
        qDebug()<<virtNetModel->DataList.at(index.row())->getName();
    }
}
void VirtNetControl::execAction(const QStringList &l)
{
    QStringList args;
    QModelIndex idx = entityList->currentIndex();
    if ( idx.isValid() && virtNetModel->DataList.count()>idx.row() ) {
        QString networkName = virtNetModel->DataList.at(idx.row())->getName();
        args.append(networkName);
        if        ( l.first()=="startVirtNetwork" ) {
            args.prepend(l.first());
            args.prepend(QString::number(START_ENTITY));
            args.prepend(currConnName);
            emit addNewTask(currWorkConnect, args);
        } else if ( l.first()=="destroyVirtNetwork" ) {
            args.prepend(l.first());
            args.prepend(QString::number(DESTROY_ENTITY));
            args.prepend(currConnName);
            emit addNewTask(currWorkConnect, args);
        } else if ( l.first()=="undefineVirtNetwork" ) {
            args.prepend(l.first());
            args.prepend(QString::number(UNDEFINE_ENTITY));
            args.prepend(currConnName);
            emit addNewTask(currWorkConnect, args);
        } else if ( l.first()=="setAutostartVirtNetwork" ) {
            /* set the opposite value */
            QString autostartState =
                (virtNetModel->DataList.at(idx.row())->getAutostart()=="yes")
                 ? "0" : "1";
            args.append(autostartState);
            args.prepend(l.first());
            args.prepend(QString::number(CHANGE_ENTITY_AUTOSTART));
            args.prepend(currConnName);
            emit addNewTask(currWorkConnect, args);
        } else if ( l.first()=="getVirtNetworkXMLDesc" ) {
            args.prepend(l.first());
            args.prepend(QString::number(GET_XML_DESCRIPTION));
            args.prepend(currConnName);
            emit addNewTask(currWorkConnect, args);
        } else if ( l.first()=="reloadVirtNetwork" ) {
            args.prepend(l.first());
            args.prepend(QString::number(GET_ALL_ENTITY));
            args.prepend(currConnName);
            emit addNewTask(currWorkConnect, args);
        };
    } else if ( l.first()=="reloadVirtNetwork" ) {
        args.prepend(l.first());
        args.prepend(QString::number(GET_ALL_ENTITY));
        args.prepend(currConnName);
        emit addNewTask(currWorkConnect, args);
    };
}
void VirtNetControl::newVirtEntityFromXML(const QStringList &_args)
{
    if ( !_args.isEmpty() ) {
        Actions act;
        QString actName;
        if ( _args.first().startsWith("create") ) {
            act = CREATE_ENTITY;
            actName = "createVirtNetwork";
        } else {
            act = DEFINE_ENTITY;
            actName = "defineVirtNetwork";
        };
        QStringList args = _args;
        args.removeFirst();
        if ( !args.isEmpty() ) {
            if ( args.first()=="manually" ) {
                args.removeFirst();
                //QString source = args.first();
                args.removeFirst();
                QString xml;
                bool show = false;
                // show SRC Creator widget
                CreateVirtNetwork *createVirtNet = new CreateVirtNetwork(this);
                int result = createVirtNet->exec();
                if ( createVirtNet!=NULL && result ) {
                    // get path for method12
                    xml = createVirtNet->getXMLDescFileName();
                    show = createVirtNet->getShowing();
                    QStringList data;
                    data.append("New Network XML'ed");
                    data.append(QString("to <a href='%1'>%1</a>").arg(xml));
                    QString msg = data.join(" ");
                    msgRepeater(msg);
                    if ( show ) QDesktopServices::openUrl(QUrl(xml));
                };
                delete createVirtNet;
                createVirtNet = NULL;
                //qDebug()<<xml<<"path"<<result;
                args.prepend(xml);
            };
            args.prepend(actName);
            args.prepend(QString::number(act));
            args.prepend(currConnName);
            emit addNewTask(currWorkConnect, args);
        };
    };
}
