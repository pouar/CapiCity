// Copyright 2009,2010 Thomas Kamps anubis1@linux-ecke.de
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the7 implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef CAPISERVERPLAYER_H
#define CAPISERVERPLAYER_H

#include "src/Capi/capiplayer.h"
#include <QTcpSocket>

class CapiServerPlayer : public QObject,  public CapiPlayer {
        Q_OBJECT

public:
        CapiServerPlayer(int id);
        ~CapiServerPlayer();

        QString getCookie();

        void setCookie(QString c);

        void send(QString msg);
        void sendMsg(QString msg, QString type="chat", int pid=-1);
        void sendClientInfo();
        void setState(int s);
        void setSocket(QTcpSocket* sock);
        QTcpSocket* getSocket();
        int getState();
        bool getMoved();
        void setMoved(bool m);

        void setPosMoveTo(int pos);

        void resetDoubles();
        void incDoubles();
        int getDoubles();
        void resetJailRolls();
        void incJailRolls();
        int getJailRolls();
        void tick();

        QString getLang();
        void setLang(QString lang);
        QString getUpdate(QString type="");

protected:
        QTcpSocket* sock;
        int state;
        //state == 0 handshaking: server sendet the first handshake and waits for .nName from client
        //state == 1 connected ready for Player commands
        //state == 2 connection lost. Client has 180 secs to reconnect
        //state == 3 dead. This object should be deleted
        bool moved;
        int posMoveTo; //The position that the client has animate to. Used if client loses connection

        int doubles;
        int jailRolls;
        int secondsDisconnected;
        QString lang;

        QString cookie;

private slots:
        void recieveMessage();
        void connectionLost();
                
signals:
        void gotMessage(int pid, QString msg);

};

#endif // CAPISERVERPLAYER_H
