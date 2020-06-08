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

#include "capitrade.h"

#include "capiserver.h"
#include "capiserverestate.h"
#include "capiserverplayer.h"
#include "capiservercard.h"


class CapiServer;

CapiTrade::CapiTrade(int id, CapiPlayer* ac, CapiPlayer* sp) {
	tradeid = id;
	revision = 0;

	type = "new";

	actor = ac;

	accepts.insert(actor, "-1");
	accepts.insert(sp, "-1");

	sendUpdateToAll();

	type="edit";
}

void CapiTrade::updateEstateTrade(CapiPlayer* toPlayer, CapiEstate* tradeEstate) {
	if (type == "rejected") return;
	if (type == "completed") return;

	//Get the TradeItem given by fromPlayer and tradeEstate
	CapiTradeItem* tradeitem = 0;
	foreach(CapiTradeItem* ti, estateTrades) {
		if (ti->getTradeEstate()->getId() == tradeEstate->getId()) {
			tradeitem = ti;
			break;
		}
	}

	if (tradeitem == 0) {
		//Not found
		if ((toPlayer != 0) && (tradeEstate->getOwner() != toPlayer)) {
			//If toPlayer is not the dummy player or the owner of the estate, Create one
			tradeitem = new CapiTradeItem(tradeEstate->getOwner(), toPlayer);
			tradeitem->setTradeEstate(tradeEstate);
			estateTrades.append(tradeitem);

			//Add this Player, if needed
			accepts.insert(toPlayer, "-1");
			accepts.insert(tradeEstate->getOwner(), "-1");
			resetAccepts();
		}
	} else {
		//Trade found modyfy: it
		if (tradeitem->getToPlayer() != toPlayer) {
			//only if there is a change
			tradeitem->setToPlayer(toPlayer);

			//Add toPlayer, if needed
			if (toPlayer != 0)
				accepts.insert(toPlayer, "-1");

			resetAccepts();
		}
	}
	sendUpdateToAll();
}

void CapiTrade::updateCardTrade(CapiPlayer* toPlayer, CapiCard* tradeCard) {
	if (type == "rejected") return;
	if (type == "completed") return;

	//Get the TradeItem given by fromPlayer and tradeEstate
	CapiTradeItem* tradeitem = 0;
	foreach(CapiTradeItem* ti, cardTrades) {
		if (ti->getTradeCard()->getId() == tradeCard->getId()) {
			tradeitem = ti;
			break;
		}
	}

	if (tradeitem == 0) {
		//Not found
		if ((toPlayer != 0) && (tradeCard->getOwner() != toPlayer)) {
			//If toPlayer is not the dummy player or the owner of the card, Create one
			tradeitem = new CapiTradeItem(tradeCard->getOwner(), toPlayer);
			tradeitem->setTradeCard(tradeCard);
			cardTrades.append(tradeitem);

			//Add this Player, if needed
			accepts.insert(toPlayer, "-1");
			accepts.insert(tradeCard->getOwner(), "-1");
			resetAccepts();
		}
	} else {
		//Trade found modyfy: it
		if (tradeitem->getToPlayer() != toPlayer) {
			//only if there is a change
			tradeitem->setToPlayer(toPlayer);

			//Add toPlayer, if needed
			if (toPlayer != 0)
				accepts.insert(toPlayer, "-1");

			resetAccepts();
		}
	}
	sendUpdateToAll();
}

void CapiTrade::updateMoneyTrade(CapiPlayer* fromPlayer, CapiPlayer* toPlayer, int tradeMoney) {
	if (type == "rejected") return;
	if (type == "completed") return;

	//Get the TradeItem given by fromPlayer and tradeEstate
	CapiTradeItem* tradeitem = 0;
	foreach(CapiTradeItem* ti, moneyTrades) {
		if ((ti->getToPlayer() == toPlayer) && (ti->getFromPlayer() == fromPlayer)) {
			tradeitem = ti;
			break;
		}
	}

	if (tradeitem == 0) {
		//Not found
		if ((toPlayer != 0) && (fromPlayer != 0) && (toPlayer != fromPlayer)) {
			//If toPlayer is not the dummy player or the owner of the estate, Create one
			tradeitem = new CapiTradeItem(fromPlayer, toPlayer);
			tradeitem->setTradeMoney(tradeMoney);
			moneyTrades.append(tradeitem);

			//Add this Player, if needed
			accepts.insert(toPlayer, "-1");
			accepts.insert(fromPlayer, "-1");
			resetAccepts();
		}
	} else {
		//Trade found modyfy: it
		if (tradeitem->getTradeMoney() != tradeMoney) {
			//only if there is a change
			tradeitem->setTradeMoney(tradeMoney);

			resetAccepts();
		}
	}
	sendUpdateToAll();
}

QString CapiTrade::getUpdateString() {
	//Create full update
	QString content = "";
	foreach (CapiTradeItem* ti, estateTrades) {
		CapiPlayer* toPlayer = ti->getToPlayer();
		int toId = -1;
		if (toPlayer != 0) toId = toPlayer->getId();

		content += QString("<tradeestate estateid=\"%1\" targetplayer=\"%2\" />").arg(ti->getTradeEstate()->getId()).arg(toId);
	}
	foreach (CapiTradeItem* ti, cardTrades) {
		CapiPlayer* toPlayer = ti->getToPlayer();
		int toId = -1;
		if (toPlayer != 0) toId = toPlayer->getId();

		content += QString("<tradecard cardid=\"%1\" targetplayer=\"%2\" />").arg(ti->getTradeCard()->getId()).arg(toId);
	}
	foreach (CapiTradeItem* ti, moneyTrades) {
		CapiPlayer* toPlayer = ti->getToPlayer();
		int toId = -1;
		if (toPlayer != 0) toId = toPlayer->getId();

		content += QString("<trademoney playerfrom=\"%1\" playerto=\"%2\" money=\"%3\" />").arg(ti->getFromPlayer()->getId()).arg(toId).arg(ti->getTradeMoney());
	}
	foreach (CapiPlayer* p, accepts.keys()) {
		content += QString("<tradeplayer playerid=\"%1\" accept=\"%2\" />").arg(p->getId()).arg(accepts.value(p).toInt() == revision);
	}

	cleanupTrades();

	return QString("<tradeupdate tradeid=\"%1\" revision=\"%2\" actor=\"%3\" type=\"%4\" >%5</tradeupdate>").arg(tradeid).arg(revision).arg(actor->getId()).arg(type, content);
}

void CapiTrade::accept(CapiPlayer* p, int rev) {
	if (isEmpty()) return;

	if (accepts.keys().contains(p)) {
		accepts.insert(p, QString::number(rev));
		sendUpdateToAll();
	}
}

bool CapiTrade::playerInvolved(CapiPlayer* p) {
	return accepts.keys().contains(p);
}

void CapiTrade::reject() {
	type = "rejected";
	sendUpdateToAll();
}

bool CapiTrade::checkForAccepts() {
	bool ok = true;
	foreach (CapiPlayer* p, accepts.keys()) {
		ok &= (accepts.value(p, "0").toInt() == revision);
	}

	if (ok) {
		//All players accepted
		type = "completed";
		sendUpdateToAll();
		return true;
	}
	return false;
}

QString CapiTrade::run() {
	bool valid = true;
	if (type != "completed") return "";

	//Check if the trade can be runed
	foreach (CapiTradeItem* ti, estateTrades) {
		valid &= ti->isValid();
	}
	foreach (CapiTradeItem* ti, cardTrades) {
		valid &= ti->isValid();
	}
	foreach (CapiTradeItem* ti, moneyTrades) {
		valid &= ti->isValid();
	}

	if (!valid) return "";

	QString update = "";

	//Now run all trades
	foreach (CapiTradeItem* ti, estateTrades) {
		CapiServerEstate* e = (CapiServerEstate*) ti->getTradeEstate();
		e->setOwner(ti->getToPlayer());
		update += e->getUpdate("owner");
	}
	foreach (CapiTradeItem* ti, cardTrades) {
		CapiServerCard* c = (CapiServerCard*) ti->getTradeCard();
		c->setOwner(ti->getToPlayer());
		update += c->getUpdate();
	}
	foreach (CapiTradeItem* ti, moneyTrades) {
		CapiServerPlayer* from = (CapiServerPlayer*) ti->getFromPlayer();
		CapiServerPlayer* to   = (CapiServerPlayer*) ti->getToPlayer();
		int m = ti->getTradeMoney();
		from->setMoney(from->getMoney()-m);
		to->setMoney(to->getMoney()+m);
		update += from->getUpdate("money");
		update += to->getUpdate("money");
	}

	return update;
}

int CapiTrade::getId() {
	return tradeid;
}

void CapiTrade::resetAccepts() {
	revision++;
	type = "edit";
	foreach (CapiPlayer* p, accepts.keys())
		accepts.insert(p, "-1");
}

void CapiTrade::cleanupTrades() {

	int i = 0;
	while (i < estateTrades.size()) {
		if (estateTrades.value(i)->getToPlayer() != 0) i++;
		else estateTrades.removeAt(i);
	}

	i = 0;
	while (i < cardTrades.size()) {
		if (cardTrades.value(i)->getToPlayer() != 0) i++;
		else cardTrades.removeAt(i);
	}

	i = 0;
	while (i < moneyTrades.size()) {
		if (moneyTrades.value(i)->getTradeMoney() != 0) i++;
		else moneyTrades.removeAt(i);
	}
}

void CapiTrade::sendUpdateToAll() {
	QString msg = getUpdateString();
	foreach (CapiPlayer* q, accepts.keys()) {
		CapiServerPlayer* p = (CapiServerPlayer*) q;
		p->send(msg);
	}
}

bool CapiTrade::isEmpty() {
	return ((estateTrades.size()+cardTrades.size()+moneyTrades.size()) == 0);
}
