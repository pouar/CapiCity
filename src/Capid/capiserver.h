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

#ifndef CAPISERVER_H
#define CAPISERVER_H

#include <QTcpServer>
#include <QList>
#include <QMap>
#include <QTimer>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "capiserverplayer.h"
#include "src/Capi/capitemplate.h"
#include "src/Capid/capiservergame.h"
#include "messagetranslator.h"

class CapiServer : public QObject {
        Q_OBJECT

public:
        CapiServer(QObject* parent=0);
        CapiServer(QString port, QObject* parent=0);
        ~CapiServer();

        static QString maskXML(QString txt);
        static bool hasSemicolon(QString txt, int* pos);

private:
        QTimer* timer;

        QTcpServer* serv;
        QList<CapiServerPlayer*> players;
        QMap<QString, CapiServerPlayer*> playerMap;
        QList<CapiTemplate*> templates;
        QList<CapiServerGame*> games;

        int pID;
        int gID;

        void init();

        CapiServerPlayer* getPlayerById(int id);
        CapiServerGame* getGameById(int id);

        void setRegisterAtGators(bool reg);
        void setRegisterIntrerval(int val);

        bool nameUsed(QString name);
        void sendToAll(QString msg, int gid=-2); //Send a message to all players within the same game, or to all, when gid is not given
        void deletePlayer(int pid);
        CapiTemplate* getTemplate(QString type);
        CapiServerGame* createGame(CapiTemplate* tmpl);
        QString getTemplateUpdate(QString name, QString desc, QString type);

        QString host;
        QString port;
        QString version;
        bool registerAtGators;
        int registerInterval;
        int secsSinceLastRegister;
        QNetworkReply* oldReply;
        QNetworkAccessManager* man;
        MessageTranslator* translator;

        bool isListening();

private slots:
        void tick();
        void manageConnection();
        void handleMessage(int pid, QString msg);
        void registerGator();
};

#endif // CAPISERVER_H
