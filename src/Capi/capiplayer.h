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

#ifndef CAPIPLAYER_H
#define CAPIPLAYER_H

#include <QString>
#include "capigame.h"
#include "capiobject.h"

class CapiGame;

class CapiPlayer : public CapiObject {

	public:
		CapiPlayer(int i);
		~CapiPlayer();
		CapiGame* getGame();
		QString   getAvatar();
		int       getMoney();
		int       getPos();
		bool      getInJail();
		bool      getHasTurn();
		bool      getCanRoll();
		bool      getCanBuyEstate();
		bool      getSpectator();
		bool      getHasDept();
		bool      getCanAuction();
		bool      getCanUseCard();
		bool      getBankrupt();
		bool      getDirectmove();

		void setGame(CapiGame* g);
		void setAvatar(QString a);
		void setMoney(int m);
		void setPos(int p);
		void setInJail(bool j);
		void setHasTurn(bool ht);
		void setCanRoll(bool cr);
		void setCanBuyEstate(bool cbe);
		void setSpectator(bool s);
		void setHasDept(bool hd);
		void setCanAuction(bool ca);
		void setCanUseCard(bool cuc);
		void setBankrupt(bool b);
		void setDirectmove(bool dm);

	protected:
		CapiGame* game;
		QString avatar;
		int money;
		int pos;
		bool inJail;
		bool hasTurn;
		bool canRoll;
		bool canBuyEstate;
		bool spectator;
		bool hasDept;
		bool canAuction;
		bool canUseCard;
		bool bankrupt;
		bool directmove;
};

#endif // CAPIPLAYER_H
