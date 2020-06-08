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

#ifndef SERVERCHOOSER_H
#define SERVERCHOOSER_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QDomDocument>

#include "src/Capic/capiserverinfo.h"

namespace Ui {
        class ServerChooser;
}

class ServerChooser : public QWidget {
        Q_OBJECT

public:
        ServerChooser(QWidget* parent=0);
        ~ServerChooser();

private:
        Ui::ServerChooser *ui;
        //For getting Serverlist
        QNetworkAccessManager* manager;
        QNetworkReply* reply1;
        QNetworkReply* reply2;

        QList<QString> ips;
        QList<CapiServerInfo*> serverInfos;

        void parseDoc(QDomDocument d);

public slots:
        void loadServerList();

private slots:
        void parseList1();
        void parseList2();
        void parseServerList(QNetworkReply* reply);
        void setServer(int line);
        void connectToServer();
        void deleteServerInfo();

protected:
        void changeEvent(QEvent *e);

signals:
        void serverChoosen(QString host, int port);
        void localServerChoosen();
};

#endif // SERVERCHOOSER_H
