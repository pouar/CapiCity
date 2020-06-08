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

#ifndef CAPITRADEITEM_H
#define CAPITRADEITEM_H

#include "src/Capi/capiplayer.h"
#include "src/Capi/capiestate.h"
#include "src/Capi/capicard.h"

class CapiTradeItem {

public:
        CapiTradeItem(CapiPlayer* fp, CapiPlayer* tp);

        CapiPlayer* getFromPlayer();
        CapiPlayer* getToPlayer();
        CapiEstate* getTradeEstate();
        CapiCard*   getTradeCard();
        int         getTradeMoney();

        void setFromPlayer(CapiPlayer* fp);
        void setToPlayer(CapiPlayer* tp);
        void setTradeEstate(CapiEstate* te);
        void setTradeCard(CapiCard* tc);
        void setTradeMoney(int tm);

        bool isEstateTrade();
        bool isCardTrade();
        bool isMoneyTrade();

        bool isValid();

private:
        CapiPlayer* fromPlayer;
        CapiPlayer* toPlayer;
        CapiEstate* tradeEstate;
        CapiCard* tradeCard;
        int tradeMoney;
};

#endif // CAPITRADEITEM_H
