#ifndef TERMWIDGET_H
#define TERMWIDGET_H

#ifndef QT4_BUILD
#include "qtermwidget4/qtermwidget.h"
#else
#include "qtermwidget5/qtermwidget.h"
#endif

#include <QAction>


class TermWidgetImpl : public QTermWidget
{
    Q_OBJECT

//        QMap< QString, QAction * > actionMap;

    public:

        TermWidgetImpl(const QString & wdir, const QString & shell=QString(), QWidget * parent=0);
        void propertiesChanged();

    signals:
        void renameSession();
        void removeCurrentSession();

    public slots:
        void zoomIn();
        void zoomOut();
        void zoomReset();

    private slots:
        void customContextMenuCall(const QPoint & pos);
        void activateUrl(const QUrl& url);
};


class TermWidget : public QWidget
{
    Q_OBJECT

    QVBoxLayout * m_layout;
    QColor m_border;

    public:
        TermWidget(const QString & wdir, const QString & shell=QString(), QWidget * parent=0);
        TermWidgetImpl * m_term;

        void propertiesChanged(); 
        QStringList availableKeyBindings() { return m_term->availableKeyBindings(); }

        TermWidgetImpl * impl() { return m_term; }
        int getPtySlaveFd() const;

    signals:
        void finished();
        void renameSession();
        void removeCurrentSession();
        void splitHorizontal(TermWidget * self);
        void splitVertical(TermWidget * self);
        void splitCollapse(TermWidget * self);
        void termGetFocus(TermWidget * self);

    public slots:
        void term_termGetFocus();

    protected:
        void paintEvent (QPaintEvent * event);

    private slots:
        void term_termLostFocus();
};

#endif

