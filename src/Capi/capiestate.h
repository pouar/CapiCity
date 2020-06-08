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

#ifndef CAPIESTATE_H
#define CAPIESTATE_H

#include <QString>
#include <QRect>
#include "capiplayer.h"
#include "capigroup.h"
#include "capiobject.h"

class CapiPlayer;
class CapiGroup;

class CapiEstate : public CapiObject {

	public:
		CapiEstate(int i);
		~CapiEstate();

		QString     getPicture();
		int         getMoney();
		bool        getBuyable();
		int         getPrice();
		CapiPlayer* getOwner();
		int         getMortageVal();
		int         getUnmortageVal();
		bool        getMortaged();
		bool        getCanToggleMortage();
		int         getRent(int i);
		CapiGroup*  getGroup();
		QString     getColor();
		QString     getBgColor();
		int         getHousePrice();
		int         getSellHousePrice();
		int         getHouses();
		bool        getCanBuyHouse();
		bool        getCanSellHouse();

		void setPicture(QString p);
		void setMoney(int m);
		void setBuyable(bool b);
		void setPrice(int p);
		void setOwner(CapiPlayer* p);
		void setMortageVal(int mv);
		void setUnmortageVal(int umv);
		void setMortaged(bool m);
		void setCanToggleMortage(bool ctm);
		void setRent(int i, int r);
		void setGroup(CapiGroup* g);
		void setColor(QString col);
		void setBgColor(QString col);
		void setHousePrice(int hp);
		void setSellHousePrice(int shp);
		void setHouses(int h);
		void setCanBuyHouse(bool cbh);
		void setCanSellHouse(bool csh);

	protected:
		//All Fields
		QString picture;
		int money;
		CapiGroup* group;
		QString color;
		QString bgColor;

		//Buyable Fields
		int buyable;
		int price;
		CapiPlayer* owner;
		int mortageVal;
		int unmortageVal;
		bool mortaged;
		bool canToggleMortage;
		int rent[6];

		//Streets
		int housePrice;
		int sellHousePrice;
		int houses;
		bool canBuyHouse;
		bool canSellHouse;
};

#endif // CAPIESTATE_H
