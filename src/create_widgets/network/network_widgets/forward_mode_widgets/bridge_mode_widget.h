#ifndef BRIDGE_MODE_WIDGET_H
#define BRIDGE_MODE_WIDGET_H

#include "create_widgets/domain/_qwidget.h"

class BRIDGE_Mode_widget : public _QWidget
{
    Q_OBJECT
public:
    explicit BRIDGE_Mode_widget(QWidget *parent = 0);

signals:

private:
    QLabel          *info;
    QVBoxLayout     *commonLayout;

public slots:

};

#endif // BRIDGE_MODE_WIDGET_H
