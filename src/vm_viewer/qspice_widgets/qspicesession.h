/*
   Copyright (C) 2014 Lindsay Mathieson (lindsay.mathieson@gmail.com).

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QSPICESESSION_H
#define QSPICESESSION_H

#include <QObject>

#include "qspicechannel.h"

// Forward Declarations
class QSpiceHelper;

class QSpiceSession : public QSpiceObject
{
    Q_OBJECT
public:
    explicit QSpiceSession(QObject *parent = 0);
    
signals:
    void channelNew(QSpiceChannel *channel);
    void channelDestroyed(QSpiceChannel *channel);
    
public slots:

private:
    friend class QSpiceHelper;

protected:
    void init();


public:
    bool Connect();
    void Disconnect();

    Q_GPROP_STR(Uri, "uri")
};

#endif // QSPICESESSION_H
