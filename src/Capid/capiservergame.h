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

#ifndef CAPISERVERGAME_H
#define CAPISERVERGAME_H
#include "src/Capi/capigame.h"

#include <QDomDocument>
#include <QDomElement>
#include <QMutex>
#include "capiserverplayer.h"
#include "capiservercard.h"
#include "capiservergroup.h"
#include "capiserverestate.h"
#include "capidept.h"
#include "capitrade.h"

#include "messagetranslator.h"

class CapiServerGame : public CapiGame {

public:
        CapiServerGame(int id, MessageTranslator* tr);
        ~CapiServerGame();

        QString getDesc(QString lang);
        int getNumPlayer();
        int getMaxHouses();
        int getMaxHotels();
        int getStartMoney();
        bool getCollectFine();
        bool getShuffleCards();
        bool getAuctions();
        bool getDoubleMoneyOnExactLanding();
        bool getUnlimitedHouses();
        bool getJailNoRent();
        bool getAutoTax();
        bool getSpectatorsAllowed();
        bool getCanBeJoined();

        void setDesc(QString d, QString lang=QString());
        void setMaxHouses(int maxH);
        void setMaxHotels(int maxH);
        void setStartMoney(int startM);
        void setCollectFine(bool cf);
        void setShuffleCards(int sc);
        void setAuctions(int a);
        void setDoubleMoneyOnExactLanding(int dm);
        void setUnlimitedHouses(int uh);
        void setJailNoRent(int jnr);
        void setAutoTax(int at);
        void setSpectatorsAllowed(int se);

        void parseGame(QDomDocument doc);
        QString removePlayerFromGame(CapiServerPlayer* p);

        QString getUpdate(QString type="", QString lang=QString());
        QString getConfigUpdate(int cid = -1, QString lang="en_US");
        void setConfig(int id, QString val);
        QString getFullStatus(QString lang);
        QString maskXML(QString txt);

        void startGame();
        void command(QString cmd, int pid);

        void tick();

private:
        CapiServerEstate* getServerEstate(int eid, bool create=true);
        CapiServerGroup* getServerGroup(int gid, QString name, bool create=true);
        CapiServerCard* getServerCard(int cid, bool create=true);

        MessageTranslator* translator;
        QMap<QString, QString> vars;

        QMap<QString, QString> descriptions;
        int maxHouses;
        int maxHotels;
        int startmoney;
        bool collectFine;
        bool shuffleCards;
        bool auctions;
        bool doubleMoneyOnExactLanding;
        bool unlimitedHouses;
        bool jailNoRent;
        bool autoTax;
        bool spectatorsAllowed;

        QString bgColor;
        int goId;

        CapiServerPlayer* playerOnTurn;
        int playerNum;
        int playingPlayers;

        //Tolls for playing
        int r1;
        int r2;

        //Rolls for estate paying
        int payR1;
        int payR2;

        //Flag for handling player movements finished
        bool playersMoved;
        QMutex handleMovedLock;

        int turnState;
        //-1: game over
        //0: waitForRoll
        //1: waitForJail
        //2: payJail
        //3: waitForMove
        //4: waitForChoice
        //5: auction
        //6: waitForTax
        //7: waitForDepts

        int auctionState;
        //0: noAuction
        //1: auctionBegin
        //2: first...
        //3; second...
        //4: aucitonEnd

        //Auctions
        int auctionid;
        int auctionStatus;
        CapiServerPlayer* auctionActor;
        CapiServerEstate* auctionEstate;
        CapiServerPlayer* auctionHighBidder;
        int auctionHighestBid;
        int statusCount;

        bool directmove;
        int oldPos;
        bool playerMoved;
        QString replaceMath;

        //Dept variables;
        QList<CapiDept*> depts;
        int deptRound;

        //Tax options:
        int taxamount;
        int taxrate;

        //Trades
        QList<CapiTrade*> trades;
        int tradeid;

        void parseGeneralData(QDomElement e);
        void parseBoardData(QDomElement e);
        void parseCardgroup(QDomElement e);
        void parseEstategroup(QDomElement e);
        void parseEstate(QDomElement e);

        QString createButton(QString cmd, QString text);
        QString createDisplayMsg(QString text, int estateid, bool clearText=false, bool clearButtons=true, QString buttons="");

        void sendToAll(QString text, int exclude=-1);

        bool allPlayersMoved();
        bool playerOwnsOutOfJailCard(CapiServerPlayer* p);
        void roll(bool forEstate=false);

        //Status methods
        void waitForRoll();
        void waitForJail();
        void payJail();
        void waitForMove();
        void waitForChoice();
        void auction();
        void auctionBid(CapiServerPlayer* bidder, int bid);
        void waitForAuctionPay();
        void resetAuction();
        QString auctionUpdate(QString type);
        void waitForTax(int ta, int tr);
        void waitForDept();

        //functional methods
        void handleMoved();
        void checkForPayment();
        void jailPlayer();
        void unjailPlayer();
        void handleEstate();
        void movedPlayer();
        void endTurn();
        void endGame();
        void wasDouble();

        int assetsForPlayer(CapiPlayer* p);
        void bankruptPlayer(CapiDept* d);
        int getNumPlayingPlayers();
        bool getDeptsPayed();

        void setCanBuyOrSellHouses(CapiServerGroup* g);

        int getNumHouses(CapiPlayer* p=0);
        int getNumHotels(CapiPlayer* p=0);

};

#endif // CAPISERVERGAME_H
