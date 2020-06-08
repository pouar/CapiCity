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

#ifndef CAPICITY_H
#define CAPICITY_H

#include <QtWidgets/QMainWindow>
#include <QTableWidget>
#include <QStackedLayout>
#include <QList>
#include <QSettings>
#include "src/Capi/capigame.h"
#include "src/Capic/capiclient.h"
#include "src/Capi/capiestate.h"
#include "serverchooser.h"
#include "gameconfig.h"
#include "field.h"
#include "trade.h"
#include "config.h"
#include "notifier.h"

#include "src/Capid/capiserver.h"

namespace Ui {
        class CapiCity;
}

class CapiCity : public QMainWindow {
        Q_OBJECT

public:
        CapiCity(QWidget *parent = 0);
        ~CapiCity();

        CapiGame* getGame();

        void setClient(CapiClient* c);
        void setGame(CapiGame* g);

private:
        Ui::CapiCity *ui;
        QSettings s;

        QStackedLayout* layout;
        QTableWidget* games;
        GameConfig* gameConfig;
        Field* field;

        CapiClient* client;
        ServerChooser* serverChooser;
        Config* config;
        CapiGame* game;
        QList<Trade*> trades;
        QString eventHtml;
        QString chatHtml;

        //Widgets for the docks
        QMap<QDockWidget*, QWidget*> titlebarWidgets;
        QWidget* dummyDockPlayersTitlebarWidget;
        QWidget* dummyDockTurnTitlebarWidget;
        QWidget* dummyDockMessagesTitlebarWidget;
        QWidget* dummyDockChatTitlebarWidget;

        //The player that is currently moving
        CapiClientPlayer* curMovingPlayer;

        bool locked;
        bool firstShow;
        bool chatVisible;
        bool eventVisible;
        bool chatColor;
        bool eventColor;

        void newGames();
        void newGameConfig();
        void newField();
        void setMenuBarVisible(bool vis);

        Trade* getTrade(int tid);
        void removeTrade(Trade* t);
        void setLocked();

        //For emoicons
        QMap<QString, QString>* emos;
        QString emoicons(QString msg);
        QMenu* emoMenu;

        //Notifiyer
        Notifier* notifier;

        //Backgrounds for texts
        QColor textBackgr;
        QColor textAltBackgr;

        //Local server
        CapiServer* server;

public slots:
        void send(QString msg);
        void buyRow(int g);
        void sellRow(int g);

private slots:
        void drawField();
        void addToList(CapiClientPlayer* p);
        void showChat(int pid, QString msg);
        void showEvent(QString type, QString msg);
        void setCurGame();
        void setOptionsEnabled(int gid, int newMaster);
        void startGame(int r, int c);
        void roll();
        void buy();
        void auction();
        void endTurn();
        void jailRoll();
        void jailCard();
        void jailPay();

        void sendMsg();

        void playerUpdate();
        void updateGameList();
        void normalChatTitle(bool vis);
        void normalEventTitle(bool vis);

        void tradeUpdate(QString type, int tradeId, int actor, int rev);
        void tradePlayerUpdate(int tradeId, int playerid, bool accept);
        void tradeEstateUpdate(int tradeId, int estateId, int targetPlayerId);
        void tradeCardUpdate(int tradeId, int cardId, int targetPlayerId);
        void tradeMoneyUpdate(int tradeId, int fromPlayerId, int money, int targetPlayerId);

        void changeView(QAction* a);
        void setData(QString type, QString data);
        void setData(QString type, bool data);

        void showHost(QString host);

        void setShownPlayer(int pid);

        bool leaveGame();
        bool disconnectFromServer();
        void forceDisconnect();
        void exit();

        void showMenuBar();
        void showAbout();

        //Slots for emoicons
        void showEmoMenu();
        void addEmo(QAction* a);

        void setAutoroll();

        //Slots for animation
        void animateMovment(CapiClientPlayer* p, int newPos);
        void acPos();
        void stopAnim();
        void updateMovment();

        void startServer();

protected:
        void closeEvent(QCloseEvent* event);
};

#endif // CAPICITY_H
