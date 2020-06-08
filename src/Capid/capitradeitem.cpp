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

#include "capitradeitem.h"

CapiTradeItem::CapiTradeItem(CapiPlayer* fp, CapiPlayer* tp) {
        fromPlayer = fp;
        toPlayer = tp;
}

CapiPlayer* CapiTradeItem::getFromPlayer() {
        return fromPlayer;
}

CapiPlayer* CapiTradeItem::getToPlayer() {
        return toPlayer;
}

CapiEstate* CapiTradeItem::getTradeEstate() {
        return tradeEstate;
}

CapiCard* CapiTradeItem::getTradeCard() {
        return tradeCard;
}

int CapiTradeItem::getTradeMoney() {
        return tradeMoney;
}

void CapiTradeItem::setFromPlayer(CapiPlayer* fp) {
        fromPlayer = fp;
}

void CapiTradeItem::setToPlayer(CapiPlayer* tp) {
        toPlayer = tp;
}

void CapiTradeItem::setTradeEstate(CapiEstate* te) {
        tradeEstate = te;
        tradeCard = 0;
        tradeMoney = 0;
}

void CapiTradeItem::setTradeCard(CapiCard* tc) {
        tradeEstate = 0;
        tradeCard = tc;
        tradeMoney = 0;
}

void CapiTradeItem::setTradeMoney(int tm) {
        tradeEstate = 0;
        tradeCard = 0;
        tradeMoney = tm;
}

bool CapiTradeItem::isEstateTrade() {
        return tradeEstate != 0;
}

bool CapiTradeItem::isCardTrade() {
        return tradeCard != 0;
}

bool CapiTradeItem::isMoneyTrade() {
        return tradeMoney != 0;
}

bool CapiTradeItem::isValid() {
        if (isEstateTrade())
                return ((tradeEstate->getOwner() == fromPlayer) && (tradeEstate->getHouses() == 0));

        if (isCardTrade())
                return (tradeCard->getOwner() == fromPlayer);

        if (isMoneyTrade())
                return (fromPlayer->getMoney() >= tradeMoney);

        //qDebug() << "bug: This tradeitem is empty!";
        return false;
}
