// Copyright 2009 Thomas Kamps anubis1@linux-ecke.de
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

#ifndef CAPICLIENT_H
#define CAPICLIENT_H

#include <QTcpSocket>
#include <QDomDocument>
#include <QSettings>
#include "src/Capi/capitemplate.h"
#include "src/Capi/capicard.h"
#include "src/Capic/Gui/capiclientplayer.h"
#include "src/Capic/capiclientgame.h"
#include "src/Capid/capiserver.h"

class CapiClient : public QObject {
        Q_OBJECT

public:
        CapiClient(QWidget *parent = 0);

        CapiClientPlayer* getPlayer(int pid);
        CapiClientPlayer* getPlayerFromList(int i);
        bool hasPlayer(int pid);
        CapiTemplate* getTemplate(QString t);
        CapiTemplate* getTemplateFromList(int n);
        CapiClientGame* getGame(int gid);
        CapiClientGame* getGameFromList(int n);
        int getNumPlayers();
        int getNumTemplates();
        int getNumGames();

        int getCurGameId();
        int getClientPlayerId();
        QString getClientCookie();

        void deletePlayer(CapiClientPlayer* p);
        void deleteGame(CapiClientGame* p);

        void setCurGameId(int id);
        void setClientPlayerId(int id);
        void setClientCookie(QString c);

        void leaveGame();
        void reconnect(); //reconnects to server after crash, if possible

        void setAutoroll(bool ar);

        bool getCapid();

private:
        QTcpSocket* soket;
        QString cache;
        QList<CapiClientPlayer*> players;
        QList<CapiTemplate*> templates;
        QList<CapiClientGame*> games;

        bool rolled;

        int curGameId;
        int clientPlayerId;
        QString clientCookie;
        QString host;

        //Are we playing with Capid?
        bool capid;

        bool reconnected;

        QSettings s;

        void parseDoc(QDomDocument d);
        void parseMsg(QDomElement m);
        void parseDisplay(QDomElement d);
        void parseGamelistUpdate(QDomElement g, QString type);
        void parsePlayer(QDomElement f);
        void parseEstate(QDomElement f);
        void parseCard(QDomElement c);
        void parseGroup(QDomElement g);
        void parseTrade(QDomElement t);
        void parseAuction(QDomElement a);
        void parseGame(QDomElement g);
        void parseConfig(QDomElement c);
        void parseConfigOption(QDomElement o);
        void showData(QString data);
        void updateEstateLabels();

        void emitPlayerMoved(CapiClientPlayer* p, int oldPos);

private slots:
        void readData();
        void showError(QAbstractSocket::SocketError e);

public slots:
        void connectToHost(QString host, int port);
        void send(QString msg);
        void disconnectFromHost(bool deketeData=true);

signals:
        //Messages
        void chatMessage(int playerid, QString msg);
        void serverMessage(QString type, QString msg);

        //Changes in the game
        void boardUpdate();
        void playerUpdated();
        void playerMoved(CapiClientPlayer* p, int newPos);

        //Signals for handling the players
        void playerAdded(CapiClientPlayer* p);
        void newClientPlayerID();

        //We got a fresh game list
        void newGameList();

        //We changed the game
        void curGameChanged();

        //The master has changed
        void gameMasterChanged(int gid, int newMaster);

        //Status of the current gamne has changed
        void statusChanged(QString status);

        //Signals for showing the display texts and buttons
        void gotEstate(int fid);
        void gotText(QString txt);
        void gotButton(QString txt, QString cmd);
        void clearedText();
        void clearedButtons();

        //A Card has changed
        void cardUpdated(CapiCard* c);

        //Game config signal
        void configUpdate(QString cmd, QString text, QString type, QString value);

        //Auction signals
        void auctionStarted();
        void auctionIdUpdate(int aid);
        void auctionEstateUpdate(int fid);
        void auctionBidUpdate(int pid, int bid);
        void auctionStatusUpdate(int status);

        //Trade signals
        void tradeUpdate(QString type, int tradeId, int actor, int rev);
        void tradePlayerUpdate(int tradeId, int playerid, bool accept);
        void tradeEstateUpdate(int tradeId, int estateId, int targetPlayerId);
        void tradeCardUpdate(int tradeId, int cardId, int targetPlayerId);
        void tradeMoneyUpdate(int tradeId, int fromPlayerId, int money, int targetPlayerId);

        //Connection signals
        void connected();
        void connectionLost();
        void gotHost(QString host);
};

#endif // CAPICLIENT_H
