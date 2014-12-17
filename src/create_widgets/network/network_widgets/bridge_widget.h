#ifndef BRIDGE_WIDGET_H
#define BRIDGE_WIDGET_H

#include "create_widgets/domain/_qwidget.h"

class Bridge_Widget : public _QWidget
{
    Q_OBJECT
public:
    explicit Bridge_Widget(QWidget *parent = 0);
    QCheckBox       *title;
    QLineEdit       *bridgeName;
    QCheckBox       *stp;
    QSpinBox        *delay;
    QComboBox       *macTableManager;

private:
    QLabel          *delayLabel,* macTabLabel;
    QHBoxLayout     *bridgeLt, *stpLt, *macTabLt;
    QWidget         *bridgeWdg, *stpWdg, *macTabWdg;
    QVBoxLayout     *commonLayout;
};

#endif // BRIDGE_WIDGET_H