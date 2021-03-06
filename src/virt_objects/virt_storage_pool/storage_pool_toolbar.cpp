#include "storage_pool_toolbar.h"

StoragePoolToolBar::StoragePoolToolBar(QWidget *parent) :
    QToolBar(parent)
{
    start_Action = new QAction(this);
    start_Action->setIcon(QIcon::fromTheme("storagePool-start"));
    start_Action->setToolTip("Start");
    destroy_Action = new QAction(this);
    destroy_Action->setIcon(QIcon::fromTheme("storagePool-stop"));
    destroy_Action->setToolTip("Stop");
    create_Action = new QAction(this);
    create_Action->setIcon(QIcon::fromTheme("storagePool-create"));
    create_Action->setToolTip("Create");
    create_Menu = new OpenFileMenu(this, "create", "storagePool");
    create_Action->setMenu(create_Menu);
    connect(create_Action, SIGNAL(triggered()), this, SLOT(showMenu()));
    define_Action = new QAction(this);
    define_Action->setIcon(QIcon::fromTheme("storagePool-define"));
    define_Action->setToolTip("Define");
    define_Menu = new OpenFileMenu(this, "define", "storagePool");
    define_Action->setMenu(define_Menu);
    connect(define_Action, SIGNAL(triggered()), this, SLOT(showMenu()));
    undefine_Action = new QAction(this);
    undefine_Action->setIcon(QIcon::fromTheme("storagePool-undefine"));
    undefine_Action->setToolTip("Undefine");
    setAutostart_Action = new QAction(this);
    setAutostart_Action->setIcon(QIcon::fromTheme("storagePool-autostart"));
    setAutostart_Action->setToolTip("Change AutoStart State");
    delete_Menu = new Delete_Pool_Menu(this);
    delete_Action = new QAction(this);
    delete_Action->setIcon(QIcon::fromTheme("storageVol-delete"));
    delete_Action->setToolTip("Delete");
    delete_Action->setMenu(delete_Menu);
    getXMLDesc_Action = new QAction(this);
    getXMLDesc_Action->setIcon(QIcon::fromTheme("storagePool-xml"));
    getXMLDesc_Action->setToolTip("Get XML Description");
    overview_Action = new QAction(this);
    overview_Action->setIcon(QIcon::fromTheme("overview"));
    overview_Action->setToolTip("Overview Pool");
    _autoReload = new QPushButton(this);
    _autoReload->setToolTip("AutoReload Pool Overview");
    _autoReload->setIcon(QIcon::fromTheme("view-refresh"));
    _autoReload->setCheckable(true);

    addAction(start_Action);
    addAction(destroy_Action);
    addSeparator();
    addAction(create_Action);
    addAction(define_Action);
    addAction(undefine_Action);
    //addSeparator();
    addAction(setAutostart_Action);
    addAction(delete_Action);
    addSeparator();
    addAction(getXMLDesc_Action);
    addSeparator();
    addAction(overview_Action);
    addSeparator();
    autoReload = addWidget(_autoReload);
    addAction(autoReload);

    settings.beginGroup("VirtStoragePoolControl");
    interval = settings.value("UpdateTime", 3).toInt();
    _autoReload->setChecked(settings.value("AutoReload", false).toBool());
    settings.endGroup();

    connect(_autoReload, SIGNAL(toggled(bool)), this, SLOT(changeAutoReloadState(bool)));

    connect(create_Menu, SIGNAL(fileForMethod(QStringList&)),
            this, SLOT(repeatParameters(QStringList&)));
    connect(define_Menu, SIGNAL(fileForMethod(QStringList&)),
            this, SLOT(repeatParameters(QStringList&)));
    connect(delete_Menu, SIGNAL(execMethod(const QStringList&)),
            this, SIGNAL(execMethod(const QStringList&)));
    connect(this, SIGNAL(actionTriggered(QAction*)),
            this, SLOT(detectTriggerredAction(QAction*)));
}
StoragePoolToolBar::~StoragePoolToolBar()
{
    settings.beginGroup("VirtStoragePoolControl");
    settings.setValue("UpdateTime", interval);
    settings.setValue("AutoReload", _autoReload->isChecked());
    settings.endGroup();
    disconnect(_autoReload, SIGNAL(toggled(bool)),
               this, SLOT(changeAutoReloadState(bool)));
    disconnect(create_Menu, SIGNAL(fileForMethod(QStringList&)),
               this, SLOT(repeatParameters(QStringList&)));
    disconnect(define_Menu, SIGNAL(fileForMethod(QStringList&)),
               this, SLOT(repeatParameters(QStringList&)));
    disconnect(delete_Menu, SIGNAL(execMethod(const QStringList&)),
               this, SIGNAL(execMethod(const QStringList&)));
    disconnect(this, SIGNAL(actionTriggered(QAction*)),
               this, SLOT(detectTriggerredAction(QAction*)));

    disconnect(define_Action, SIGNAL(triggered()), this, SLOT(showMenu()));
    disconnect(create_Action, SIGNAL(triggered()), this, SLOT(showMenu()));

    delete start_Action;
    start_Action = NULL;
    delete destroy_Action;
    destroy_Action = NULL;
    delete create_Menu;
    create_Menu = NULL;
    delete define_Menu;
    define_Menu = NULL;
    delete create_Action;
    create_Action = NULL;
    delete define_Action;
    define_Action = NULL;
    delete undefine_Action;
    undefine_Action = NULL;
    delete setAutostart_Action;
    setAutostart_Action = NULL;
    delete delete_Menu;
    delete_Menu = NULL;
    delete delete_Action;
    delete_Action = NULL;
    delete getXMLDesc_Action;
    getXMLDesc_Action = NULL;
    delete overview_Action;
    overview_Action = NULL;
    delete _autoReload;
    _autoReload = NULL;
    delete autoReload;
    autoReload = NULL;
}

/* public slots */
Qt::ToolBarArea StoragePoolToolBar::get_ToolBarArea(int i) const
{
  Qt::ToolBarArea result;
  switch (i) {
  case 1:
    result = Qt::LeftToolBarArea;
    break;
  case 2:
    result = Qt::RightToolBarArea;
    break;
  case 4:
    result = Qt::TopToolBarArea;
    break;
  case 8:
    result = Qt::BottomToolBarArea;
    break;
  default:
    result = Qt::TopToolBarArea;
    break;
  };
  return result;
}
void StoragePoolToolBar::enableAutoReload()
{
    if ( _autoReload->isChecked() ) timerId = startTimer(interval*1000);
}
void StoragePoolToolBar::stopProcessing()
{
    if ( timerId ) {
        killTimer(timerId);
        timerId = 0;
    }
}
bool StoragePoolToolBar::getAutoReloadState() const
{
    return _autoReload->isChecked();
}

/* private slots */
void StoragePoolToolBar::timerEvent(QTimerEvent *event)
{
    int _timerId = event->timerId();
    //qDebug()<<_timerId<<timerId;
    if ( _timerId && timerId==_timerId && isVisible() ) {
        QStringList parameters;
        parameters << "reloadVirtStoragePool";
        emit execMethod(parameters);
    };
}
void StoragePoolToolBar::repeatParameters(QStringList &p)
{
    emit fileForMethod(p);
}
void StoragePoolToolBar::showHoveredMenu()
{
    QAction *act = static_cast<QAction*>(sender());
    if ( act==create_Action ) {
        if ( define_Action->menu()->isVisible() )
            define_Action->menu()->hide();
    } else if ( act==define_Action ) {
        if ( create_Action->menu()->isVisible() )
            create_Action->menu()->hide();
    } else {
        if ( define_Action->menu()->isVisible() )
            define_Action->menu()->hide();
        if ( create_Action->menu()->isVisible() )
            create_Action->menu()->hide();
        return;
    };
    //act->menu()->popup(mapToGlobal(this->pos()), act);
    act->menu()->show();
    act->menu()->move(QCursor::pos());
}
void StoragePoolToolBar::showMenu()
{
    QAction *act = static_cast<QAction*>(sender());
    if ( act->menu()->isVisible() ) act->menu()->hide();
    else {
        act->menu()->show();
        act->menu()->move(QCursor::pos());
    };
}
void StoragePoolToolBar::detectTriggerredAction(QAction *action)
{
    QStringList parameters;
    if ( action == start_Action) {
        parameters << "startVirtStoragePool";
    } else if ( action == destroy_Action ) {
        parameters << "destroyVirtStoragePool";
    } else if ( action == undefine_Action ) {
        parameters << "undefineVirtStoragePool";
    } else if ( action == setAutostart_Action ) {
        parameters << "setAutostartVirtStoragePool";
    } else if ( action == getXMLDesc_Action ) {
        parameters << "getVirtStoragePoolXMLDesc";
    } else if ( action == overview_Action ) {
        parameters << "overviewVirtStoragePool";
    } else return;
    emit execMethod(parameters);
}
void StoragePoolToolBar::changeAutoReloadState(bool state)
{
    if ( state ) enableAutoReload();
    else stopProcessing();
}
