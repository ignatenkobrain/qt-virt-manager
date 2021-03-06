#ifndef CONN_LIST_WIDGET_H
#define CONN_LIST_WIDGET_H

#include <QTreeView>
#include <QMessageBox>
#include <QInputDialog>
#include "conn_menu.h"
#include "conn_item_model.h"
#include "progressbar_delegate.h"
#include "virt_objects/virt_connect/conn_element.h"
#include <QDebug>

#define TO_RUN true
#define TO_STOP false
typedef QMap<QString, ElemConnect*> CONN_LIST;

class ConnectList : public QTreeView
{
    Q_OBJECT
public:
    ConnectList(QWidget *parent);
    ~ConnectList();

    CONN_LIST           *connects;
    ConnItemModel       *connItemModel;

signals:
    void clickedItem(QString);
    void removeConnect(QString&);
    void messageShowed();
    void warning(QString&);
    void connPtr(virConnect*, QString&);
    void connectClosed(virConnect*);
    void domResult(Result);

public slots:
    int  connectItemEditAction();
    void addConnectItem(QString&);
    void deleteCurrentConnect();
    void openConnect(QModelIndex&);
    void showConnect(QModelIndex&);
    void closeConnect(QModelIndex&);
    virConnectPtr getConnect(QString&);
    void stopProcessing();

private :
    ConnSettings         *sDialog;
    ProgressBarDelegate  *progressBarDlg;

private slots:
    void connectItemClicked(const QPoint&);
    void connectItemDoubleClicked(const QModelIndex&);
    void connectItemKillAction();
    void connectItemRunAction();
    void connectItemShowAction();
    void createConnect(QModelIndex&);
    void checkConnect(QModelIndex&, bool);
    void deleteCancelledCreation();
    void showMessage(QString, QString);
    void sendWarning(QString&);
    void mainWindowUp();
    void sendConnPtr(virConnect*, QString&);
    void getAuthCredentials(QString&);
};

#endif   // CONN_LIST_WIDGET_H
