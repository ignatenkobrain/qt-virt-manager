#ifndef _STORAGE_AUTH_H
#define _STORAGE_AUTH_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QGridLayout>

class _Storage_Auth : public QWidget
{
    Q_OBJECT
public:
    explicit _Storage_Auth(QWidget *parent = NULL);
    QCheckBox       *auth;
    QLineEdit       *userName, *usage;
    QComboBox       *usageType;

signals:
    void             dataChanged();

private:
    QLabel          *userLabel;
    QGridLayout     *authLayout;
    QWidget         *authWdg;
    QVBoxLayout     *commonLayout;

public slots:

private slots:
    void             usageTypeChanged(int);

};

#endif // _STORAGE_AUTH_H
