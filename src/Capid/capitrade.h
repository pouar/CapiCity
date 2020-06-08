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

#ifndef CAPITRADE_H
#define CAPITRADE_H

#include "capitradeitem.h"

#include <QMap>

class CapiTrade {

public:
        CapiTrade(int id, CapiPlayer* ac, CapiPlayer* sp);

        void updateEstateTrade(CapiPlayer* toPlayer, CapiEstate* tradeEstate);
        void updateCardTrade(CapiPlayer* toPlayer, CapiCard* tradeCard);
        void updateMoneyTrade(CapiPlayer* fromPlayer, CapiPlayer* toPlayer, int tradeMoney);

        void accept(CapiPlayer* p, int revision);
        bool playerInvolved(CapiPlayer* p);
        void reject();
        bool checkForAccepts();
        QString run();
        int getId();

private:
        int tradeid;
        int revision;
        QString type;
        CapiPlayer* actor;

        QList<CapiTradeItem*> estateTrades;
        QList<CapiTradeItem*> cardTrades;
        QList<CapiTradeItem*> moneyTrades;

        QMap<CapiPlayer*, QString> accepts;
        QString getUpdateString();

        void resetAccepts();
        void cleanupTrades();
        void sendUpdateToAll();
        bool isEmpty();
};

#endif // CAPITRADE_H
