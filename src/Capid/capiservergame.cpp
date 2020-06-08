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

#include <QtScript>
#include "capiservergame.h"
#include "capiserver.h"

class CapiServer;

CapiServerGame::CapiServerGame(int id, MessageTranslator* tr) : CapiGame(id) {
	maxHouses = 999;
	maxHotels = 999;
	startmoney = 1500;
	collectFine = true;
	shuffleCards = false;
	auctions = true;
	doubleMoneyOnExactLanding = false;
	unlimitedHouses = false;
	jailNoRent = true;
	autoTax = false;
	spectatorsAllowed = false;
	bgColor = "";
	goId = 0;

	playersMoved = false;
	//handleMovedLock.unlock();

	turnState = -1;
	auctionState = 0;
	auctionid = -1;
	auctionStatus = 0;
	auctionActor = 0;
	auctionEstate = 0;
	auctionHighBidder = 0;
	auctionHighestBid = 0;
	statusCount = 0;

	playerOnTurn = 0;

	deptRound = 0;
	replaceMath = "";

	tradeid = 1;

	translator = tr;
}

CapiServerGame::~CapiServerGame() {
	while (trades.size() > 0) {
		CapiTrade* t = trades.value(0);
		trades.removeAll(t);
		delete t;
	}
}

QString CapiServerGame::maskXML(QString txt) {
	QString msg = txt.replace("&", "&#38;");
	msg = msg.replace("<", "&#60;");
	msg = msg.replace(">", "&#62;");
	msg = msg.replace("=", "&#61;");
	msg = msg.replace("\"", "&#34;");
	return msg;
}

QString CapiServerGame::getDesc(QString lang) {
	if (descriptions.keys().contains(lang)) return descriptions.value(lang);
	return CapiGame::getDesc();
}

int CapiServerGame::getNumPlayer() {
	int p = 0;
	foreach(CapiPlayer* pl, players)
		if (!pl->getSpectator()) p++;
	return p;
}

int CapiServerGame::getMaxHouses() {
	if (unlimitedHouses) return 99999;
	return maxHouses;
}

int CapiServerGame::getMaxHotels() {
	if (unlimitedHouses) return 99999;
	return maxHouses;
}

int CapiServerGame::getStartMoney() {
	return startmoney;
}

bool CapiServerGame::getCollectFine() {
	return collectFine;
}

bool CapiServerGame::getShuffleCards() {
	return shuffleCards;
}

bool CapiServerGame::getAuctions() {
	return auctions;
}

bool CapiServerGame::getDoubleMoneyOnExactLanding() {
	return doubleMoneyOnExactLanding;
}

bool CapiServerGame::getUnlimitedHouses() {
	return unlimitedHouses;
}

bool CapiServerGame::getJailNoRent() {
	return jailNoRent;
}

bool CapiServerGame::getAutoTax() {
	return autoTax;
}

bool CapiServerGame::getSpectatorsAllowed() {
	return spectatorsAllowed;
}

bool CapiServerGame::getCanBeJoined() {
	return ((players.count() < maxPlayer) && (status == "config"));
}

void CapiServerGame::setDesc(QString d, QString lang) {
	if (lang.isNull()) CapiGame::setDesc(d);
	else descriptions.insert(lang, d);
}

void CapiServerGame::setMaxHouses(int maxH) {
	maxHouses = maxH;
}

void CapiServerGame::setMaxHotels(int maxH) {
	maxHotels = maxH;
}

void CapiServerGame::setStartMoney(int startM) {
	startmoney = startM;
}

void CapiServerGame::setCollectFine(bool cf) {
	collectFine = cf;
}

void CapiServerGame::setShuffleCards(int sc) {
	shuffleCards = sc;
}

void CapiServerGame::setAuctions(int a) {
	auctions = a;
}

void CapiServerGame::setDoubleMoneyOnExactLanding(int dm) {
	doubleMoneyOnExactLanding = dm;
}

void CapiServerGame::setUnlimitedHouses(int uh) {
	unlimitedHouses = uh;
}

void CapiServerGame::setJailNoRent(int jnr) {
	jailNoRent = jnr;
}

void CapiServerGame::setAutoTax(int at) {
	autoTax = at;
}

void CapiServerGame::setSpectatorsAllowed(int se) {
	spectatorsAllowed = se;
}

void CapiServerGame::tick() {
	//this method is called every second

	if (auctionid != -1) {
		if (auctionHighestBid > 0) statusCount++;
		if (statusCount > 5) {
			auctionStatus++;
			if (auctionStatus <= 3) {
				statusCount = 0;
				sendToAll(auctionUpdate("status"));
				if (auctionStatus == 3) {
					//Auction ended
					//sendToAll(createDisplayMsg(QString("%1 won the auction for %2").arg(auctionHighBidder->getName(), auctionEstate->getName()), playerOnTurn->getPos()));

					//new code
					QMap<QString, QString> vars;
					vars.insert("%{player}", auctionHighBidder->getName());
					foreach (CapiPlayer* q, players) {
						CapiServerPlayer* p = (CapiServerPlayer*) q;
						vars.insert("%{estate}", auctionEstate->getName(p->getLang()));

						QString msg = translator->translateMessage("%{player} won the auction for %{estate}", p->getLang(), &vars);
						p->send(createDisplayMsg(msg, playerOnTurn->getPos(), false, true, ""));
					}

					waitForAuctionPay();
				}
			} else {
				auctionStatus = 3;
			}
		}
	}

	if (allPlayersMoved()) {
		//handleMoved();
	}
}

//Returns the Estate with given id. If no such field exists, one wil be createed and returned
CapiServerEstate* CapiServerGame::getServerEstate(int id, bool create) {
	for (int i = 0; i < estates.size(); i++)
		if (estates.value(i)->getId() == id) return (CapiServerEstate*) estates.value(i);

	if (create) {
		CapiServerEstate* f = new CapiServerEstate(id);
		estates.append(f);
		return f;
	} else {
		return 0;
	}
}

//Returns the Estate with given id. If no such field exists, one wil be createed and returned
CapiServerGroup* CapiServerGame::getServerGroup(int id, QString name, bool create) {
	foreach (CapiGroup* g, groups)
		if (g->getId() == id) return (CapiServerGroup*) g;

	if (!create) return 0;

	CapiServerGroup* g = new CapiServerGroup(id, name);
	groups.append(g);
	return g;
}

CapiServerCard* CapiServerGame::getServerCard(int id, bool create) {
	foreach (CapiCard* c, cards) {
		if (c->getId() == id) return (CapiServerCard*) c;
	}
	if (!create) return 0;

	CapiServerCard* c = new CapiServerCard(id);
	cards.append(c);
	return c;
}

QString CapiServerGame::getUpdate(QString type, QString lang) {
	QString ret = "<gameupdate ";
	int m = -1;
	if (getMaster() != 0) m = getMaster()->getId();

	ret += "gameid=\""+QString::number(getId())+"\" ";

	if ((type == "") || (type.contains("gametype")))        ret += QString("gametype=\"%1\" ").arg(getType());
	if ((type == "") || (type.contains("name")))            ret += QString("name=\"%1\" ").arg(maskXML(getName()));
	if ((type == "") || (type.contains("description")))     ret += QString("description=\"%1\" ").arg(maskXML(getDesc(lang)));
	if ((type == "") || (type.contains("status")))          ret += QString("status=\"%1\" ").arg(getStatus());
	if ((type == "") || (type.contains("players")))         ret += QString("players=\"%1\" ").arg(players.count());
	if ((type == "") || (type.contains("canbejoined")))     ret += QString("canbejoined=\"%1\" ").arg(getCanBeJoined());
	if ((type == "") || (type.contains("minplayers")))      ret += QString("minplayers=\"%1\" ").arg(getMinPlayer());
	if ((type == "") || (type.contains("mayplayers")))      ret += QString("maxplayers=\"%1\" ").arg(getMaxPlayer());
	if ((type == "") || (type.contains("master")))          ret += QString("master=\"%1\" ").arg(m);
	if ((type == "") || (type.contains("allowestatesales"))) ret += QString("allowestatesales=\"%1\" ").arg(getSellEnabled());
	ret += "/>";
	return ret;
}

void CapiServerGame::parseGame(QDomDocument doc) {
	QDomElement root = doc.documentElement();
	QDomElement e = root.firstChildElement();
	while (!e.isNull()) {
		if (e.nodeName() == "general") parseGeneralData(e);
		else if (e.nodeName() == "board") parseBoardData(e);
		else if (e.nodeName() == "cardgroup") parseCardgroup(e);
		else if (e.nodeName() == "estategroup") parseEstategroup(e);
		else if (e.nodeName() == "estate") parseEstate(e);
		else qDebug() << "Unknon Object: " << e.nodeName() << "\n";
		e = e.nextSiblingElement();
	}
}

void CapiServerGame::parseGeneralData(QDomElement e) {
	QDomElement f = e.firstChildElement();
	while (!f.isNull()) {
		if (f.nodeName() == "name") setName(f.text());
		else if (f.nodeName() == "description") {
			QString lang = f.attribute("lang", "");
			if (lang == QString(""))
				setDesc(f.text());
			else
				setDesc(f.text(), lang);
		}
		else if (f.nodeName() == "minplayers") setMinPlayer(f.text().toInt());
		else if (f.nodeName() == "maxplayers") setMaxPlayer(f.text().toInt());
		else if (f.nodeName() == "houses") setMaxHouses(f.text().toInt());
		else if (f.nodeName() == "hotels") setMaxHotels(f.text().toInt());
		else if (f.nodeName() == "startmoney") setStartMoney(f.text().toInt());
		else qDebug() << "Unknon General data: " << f.nodeName() << "\n";

		f = f.nextSiblingElement();
	}
}

void CapiServerGame::parseBoardData(QDomElement e) {
	QDomElement f = e.firstChildElement();
	while (!f.isNull()) {
		if (f.nodeName() == "bgcolor") bgColor = f.text();
		else if (f.nodeName() == "go") goId = f.text().toInt();
		else qDebug() << "Unknon Board data: " << f.nodeName() << "\n";

		f = f.nextSiblingElement();
	}
}

void CapiServerGame::parseCardgroup(QDomElement e) {
	QString grName = e.attribute("name", "");
	QDomElement c = e.firstChildElement();
	while (!c.isNull()) {
		CapiServerCard* sc = getServerCard(cards.size()+1);
		sc->setOwner(0);
		sc->setGroup(grName);

		QDomElement cp = c.firstChildElement();
		while (!cp.isNull()) {
			if (cp.nodeName() == "text") {
				QString lang = cp.attribute("lang", "");
				if (lang == "")
					sc->setText(cp.text());
				else
					sc->setText(cp.text(), lang);
			}
			else if (cp.nodeName() == "advancetonextof") sc->setAdvanceToNextOf(cp.text());
			else if (cp.nodeName() == "rentmath") sc->setRentMath(cp.text());
			else if (cp.nodeName() == "tojail") sc->setToJail(cp.text() == "1");
			else if (cp.nodeName() == "advanceto") sc->setAdvanceTo(cp.text().toInt());
			else if (cp.nodeName() == "advance") sc->setAdvance(cp.text().toInt());
			else if (cp.nodeName() == "pay") sc->setPay(cp.text().toInt());
			else if (cp.nodeName() == "payeach") sc->setPayEach(cp.text().toInt());
			else if (cp.nodeName() == "payhouse") sc->setPayHouse(cp.text().toInt());
			else if (cp.nodeName() == "payhotel") sc->setPayHotel(cp.text().toInt());
			else if (cp.nodeName() == "canbeowned") sc->setCanBeOwned(cp.text() == "1");
			else if (cp.nodeName() == "outofjail") sc->setOutOfJail(cp.text() == "1");
			else if (cp.nodeName() == "roll") sc->setRoll(cp.text() == "1");
			else if (cp.nodeName() == "title") sc->setTitle(cp.text());
			else qDebug() << "Unknon Card property: " << cp.nodeName() << "\n";

			cp = cp.nextSiblingElement();
		}

		c = c.nextSiblingElement();
	}
}

void CapiServerGame::parseEstategroup(QDomElement e) {
	QString grName = e.attribute("name", "");
	int id = e.attribute("id", "0").toInt();
	CapiServerGroup* g = getServerGroup(id, grName);

	QDomElement f = e.firstChildElement();
	while (!f.isNull()) {
		if (f.nodeName() == "color") g->setColor(f.text());
		else if (f.nodeName() == "bgcolor") g->setBgColor(f.text());
		else if (f.nodeName() == "houseprice") g->setHousePrice(f.text().toInt());
		else if (f.nodeName() == "price") g->setPrice(f.text().toInt());
		else if (f.nodeName() == "rentmath") g->setRentMath(f.text());
		else if (f.nodeName() == "name") g->setName(f.text(), f.attribute("lang"));
		else qDebug() << "Unknon Estategroup property: " << f.nodeName() << "\n";

		f = f.nextSiblingElement();
	}
}

void CapiServerGame::parseEstate(QDomElement e) {
	QString esName = e.attribute("name", "");
	int id = e.attribute("id", "0").toInt();
	CapiServerEstate* es = getServerEstate(id, true);
	es->setOwner(0);
	es->setPayTarget(0);
	es->setName(esName);
	es->setBgColor(bgColor);

	QDomElement f = e.firstChildElement();
	while (!f.isNull()) {
		if (f.nodeName() == "group") {
			es->setGroup(getGroup(f.text().toInt()));
			CapiServerGroup* g = (CapiServerGroup*) es->getGroup();
			g->addEstate(es);
			es->setHousePrice(g->getHousePrice());
			es->setSellHousePrice(g->getHousePrice()/2);
			es->setColor(g->getColor());
			if (g->getBgColor() != "") es->setBgColor(g->getBgColor());
			if (g->getPrice() >= 0) es->setPrice(g->getPrice());
			es->setBuyable(es->getPrice() > 0);
		}
		else if (f.nodeName() == "icon") es->setPicture(f.text());
		else if (f.nodeName() == "price") {
			es->setPrice(f.text().toInt());
			es->setBuyable(es->getPrice() > 0);
		}
		else if (f.nodeName() == "tax") es->setTax(f.text().toInt());
		else if (f.nodeName() == "taxpercentage") es->setTaxPercent(f.text().toInt());
		else if (f.nodeName() == "paytarget") es->setPayTarget(getServerEstate(f.text().toInt(), true));
		else if (f.nodeName() == "takecard") es->setTakeCard(f.text());
		else if (f.nodeName() == "passmoney") es->setPassmoney(f.text().toInt());
		else if (f.nodeName() == "tojail") es->setToJail(f.text() == "1");
		else if (f.nodeName() == "jail") es->setJail(f.text() == "1");
		else if (f.nodeName() == "payamount") es->setPayAmount(f.text().toInt());
		else if (f.nodeName() == "freeparking") es->setFreeParking(f.text() == "1");
		else if (f.nodeName() == "go") es->setGo(f.text() == "1");
		else if (f.nodeName() == "bgcolor") es->setBgColor(f.text());
		else if (f.nodeName() == "advancetoifowned") es->setAdvanceToIfOwned(f.text().toInt());
		else if (f.nodeName() == "rent0") es->setRent(0, f.text().toInt());
		else if (f.nodeName() == "rent1") es->setRent(1, f.text().toInt());
		else if (f.nodeName() == "rent2") es->setRent(2, f.text().toInt());
		else if (f.nodeName() == "rent3") es->setRent(3, f.text().toInt());
		else if (f.nodeName() == "rent4") es->setRent(4, f.text().toInt());
		else if (f.nodeName() == "rent5") es->setRent(5, f.text().toInt());
		else if (f.nodeName() == "name") es->setName(f.text(), f.attribute("lang"));
		else qDebug() << "Unknon Estate property: " << f.nodeName() << "\n";

		f = f.nextSiblingElement();
	}
}

QString CapiServerGame::getConfigUpdate(int cid, QString lang) {
	QString ret = "";
	QString base = "<configupdate configid=\"%1\" description=\"%4\" type=\"%2\" value=\"%3\" edit=\"1\" />";

	if  ((cid == 1) || (cid == -1))  ret += base.arg(1).arg("bool").arg(getCollectFine()).arg(translator->translate("Free Parking collects fine", lang));
	if  ((cid == 2) || (cid == -1))  ret += base.arg(2).arg("bool").arg(getShuffleCards()).arg(translator->translate("Shuffle Cards before taking", lang));
	if  ((cid == 3) || (cid == -1))  ret += base.arg(3).arg("bool").arg(getAuctions()).arg(translator->translate("Enable auctions", lang));
	if  ((cid == 4) || (cid == -1))  ret += base.arg(4).arg("bool").arg(getDoubleMoneyOnExactLanding()).arg(translator->translate("Double passmoney on exact landing", lang));
	if  ((cid == 5) || (cid == -1))  ret += base.arg(5).arg("bool").arg(getUnlimitedHouses()).arg(translator->translate("Unlimited houses/hotels", lang));
	if  ((cid == 6) || (cid == -1))  ret += base.arg(6).arg("bool").arg(getJailNoRent()).arg(translator->translate("Players in jail gets no rent", lang));
	if  ((cid == 7) || (cid == -1))  ret += base.arg(7).arg("bool").arg(getSellEnabled()).arg(translator->translate("Allow selling estates", lang));
	if  ((cid == 8) || (cid == -1))  ret += base.arg(8).arg("bool").arg(getAutoTax()).arg(translator->translate("Automate tax decisions", lang));
	if  ((cid == 9) || (cid == -1))  ret += base.arg(9).arg("bool").arg(getSpectatorsAllowed()).arg(translator->translate("Allow spectators", lang));
	if ((cid == 10) || (cid == -1))  ret += base.arg(10).arg("int").arg(getStartMoney()).arg(translator->translate("Starting Cash", lang));

	return ret;
}

void CapiServerGame::setConfig(int id, QString val) {
	int iVal = 0;
	bool ok;

	switch (id) {
	case 1:
		if (val == "1") collectFine = true;
		if (val == "0") collectFine = false;
		break;
	case 2:
		if (val == "1") shuffleCards = true;
		if (val == "0") shuffleCards = false;
		break;
	case 3:
		if (val == "1") auctions = true;
		if (val == "0") auctions = false;
		break;
	case 4:
		if (val == "1") doubleMoneyOnExactLanding = true;
		if (val == "0") doubleMoneyOnExactLanding = false;
		break;
	case 5:
		if (val == "1") unlimitedHouses = true;
		if (val == "0") unlimitedHouses = false;
		break;
	case 6:
		if (val == "1") jailNoRent = true;
		if (val == "0") jailNoRent = false;
		break;
	case 7:
		if (val == "1") sellEnabled = true;
		if (val == "0") sellEnabled = false;
		break;
	case 8:
		if (val == "1") autoTax = true;
		if (val == "0") autoTax = false;
		break;
	case 9:
		if (val == "1") spectatorsAllowed = true;
		if (val == "0") spectatorsAllowed = false;
		break;
	case 10:
		iVal = val.toInt(&ok);
		if (ok) {
			if (iVal > 0) startmoney = iVal;
		}
		break;
	default:
		qDebug(QString("No such config ID: "+QString::number(id)+"\n").toUtf8().data());
		break;
	}
}

QString CapiServerGame::removePlayerFromGame(CapiServerPlayer* p) {
	QString ret = "";

	//remove players estate owns
	foreach(CapiEstate* e, estates) {
		if (e->getOwner() == p) {
			CapiServerEstate* f = (CapiServerEstate*) e;
			f->setOwner(0);
			f->setHouses(0);
			f->setMortaged(false);
			setCanBuyOrSellHouses((CapiServerGroup*) f->getGroup());
			ret += f->getUpdate("owner houses mortgaged");
		}
	}

	//remove all player cards owns
	foreach(CapiCard* c, cards) {
		if (c->getOwner() == p) {
			CapiServerCard* d = (CapiServerCard*) c;
			d->setOwner(0);
			ret += d->getUpdate();
		}
	}

	p->setMoney(0);
	p->setBankrupt(false);
	p->setInJail(false);
	p->setCanRoll(false);
	p->setGame(0);
	ret += p->getUpdate("game money bankrupt jailed can_roll");
	removePlayer(p);

	if ((getStatus() != "end") && (p == playerOnTurn)) {
		endTurn();
	}

	//Set new Master, if needed
	if (getMaster() != 0) {
		if (getMaster() == p) {
			//Find non spectating player
			CapiPlayer* newMaster = 0;
			foreach (CapiPlayer* p, players) {
				if (!p->getSpectator()) {
					newMaster = p;
					break;
				}
			}
			setMaster(newMaster);
			ret += getUpdate("master");
			if ((getMaster() == 0) && (players.size() > 0)) {
				//There are spectators left
				foreach (CapiPlayer* p, players) {
					CapiServerPlayer* pl = (CapiServerPlayer*) p;
					pl->setGame(0);
					ret += pl->getUpdate("game");
				}
			}
		}
	} else {
		qDebug() << "Bug: Game has no master.";
	}

	if (getNumPlayingPlayers() == 1) {
		endGame();
	}

	return ret;
}

QString CapiServerGame::getFullStatus(QString lang) {
	QString ret = getUpdate(lang);

	//Collect full Player updates
	foreach (CapiPlayer* p, players) {
		ret += ((CapiServerPlayer* ) p)->getUpdate();
	}

	//collect full estategroup-updates
	foreach (CapiGroup* g, groups) {
		ret += QString("<estategroupupdate groupid=\"%1\" name=\"%2\" />").arg(g->getId()).arg(((CapiServerGroup*)g)->getName(lang));
	}

	//collect full estate updates
	foreach (CapiEstate* e, estates) {
		ret += ((CapiServerEstate* ) e)->getUpdate("", lang);
	}

	//collect full Card updates
	foreach (CapiCard* c, cards) {
		ret += ((CapiServerCard* ) c)->getUpdate();
	}

	return ret;
}

void CapiServerGame::startGame() {
	setStatus("init");
	foreach(CapiPlayer* p, players) {
		CapiServerPlayer* q = (CapiServerPlayer*) p;
		q->setMoney(getStartMoney());
		q->setMoved(true);
		q->setHasTurn(false);
		q->setCanRoll(false);
		q->setBankrupt(false);
		q->setInJail(false);
		q->setPos(goId);
	}

	//Mix up player order and cards
	for (int s = 0; s < 2000; s++) {
		//Players
		int i = rand()%players.size();
		int j = rand()%players.size();
		players.swapItemsAt(i,j);

		//Cards
		i = rand()%cards.size();
		j = rand()%cards.size();
		cards.swapItemsAt(i,j);
	}

	//Get the first non spectator
	for (int i = 0; i < players.size(); i++) {
		if (!players.value(i)->getSpectator()) {
			playerOnTurn = (CapiServerPlayer*) players.value(i);
			playerNum = i;
		}
	}

	playerOnTurn->setHasTurn(true);
	playerOnTurn->setCanRoll(true);
	if (unlimitedHouses) {
		maxHouses = 999999999;
		maxHotels = 999999999;
	}

	foreach (CapiPlayer* q, players) {
		CapiServerPlayer* pl = (CapiServerPlayer*) q;
		pl->send(getFullStatus(pl->getLang()));
	}
	setStatus("run");
	sendToAll(getUpdate("status"));

	QMap<QString, QString> vars;
	vars.insert("%{player}", playerOnTurn->getName());
	foreach (CapiPlayer* q, players) {
		CapiServerPlayer* p = (CapiServerPlayer*) q;
		p->send(createDisplayMsg(translator->translateMessage("Game started.", p->getLang(),0), -1, true, true));
		p->send(createDisplayMsg(translator->translateMessage("Turn goes to %{player}.", p->getLang(), &vars), -1));
		p->send(createDisplayMsg(translator->translateMessage("Warning: This Server is not marked as stable. It is possible that the server will crash, or do some silly things. Don't use it for official Tournaments. And NEVER let your Nuclear Weapons depend on this buggy server!", p->getLang(),0), -1));

	}

	playersMoved = true;
	turnState = 0;
	sendToAll(playerOnTurn->getUpdate("hasturn"));
}

void CapiServerGame::command(QString command, int pid) {
	CapiServerPlayer* p = (CapiServerPlayer*) getPlayer(pid);
	if (p == 0) return;
	if (command == ".r") {
		//Roll command
		if (p->getHasTurn() && p->getCanRoll() && (turnState == 0)) {

			p->setCanRoll(false);
			roll();

			QMap<QString, QString> vars;
			vars.insert("%{roll1}", QString::number(r1));
			vars.insert("%{roll2}", QString::number(r2));
			vars.insert("%{player}", p->getName());

			foreach (CapiPlayer* q, players) {
				CapiServerPlayer* pl = (CapiServerPlayer*) q;
				pl->send(createDisplayMsg(translator->translateMessage("%{player} rolls %{roll1} and %{roll2}.", pl->getLang(), &vars), p->getPos(), true));
			}

			//Caluclate new postion
			int newpos = (p->getPos()+r1+r2) % estates.count();

			//Check for doubles
			if (r1 == r2) p->incDoubles();

			if (p->getDoubles() == 3) {
				foreach(CapiPlayer* q, players) {
					CapiServerPlayer* pl = (CapiServerPlayer*) q;
					pl->send(createDisplayMsg(translator->translateMessage("%{player} has 3 doubles in row.", pl->getLang(), &vars), p->getPos()));

				}
				jailPlayer();
				endTurn();
			} else {
				//Notice all clients to move the player
				directmove = false;
				p->setDirectmove(false);
				oldPos = p->getPos();
				p->setPos(newpos);
				waitForMove();

				playersMoved = false;
			}
		} else {
			p->sendMsg(translator->translateMessage("You cannot roll at this moment.", p->getLang(),0), "error");
		}
	} else if (command.startsWith(".t")) {
		//Turn position command
		if (turnState == 3) {
			bool ok;
			int pos = command.remove(0,2).toInt(&ok);
			if (ok) {
				CapiServerEstate* e = getServerEstate(pos);

				//Send Client a notice that the player gets money
				if ((e != 0) && (e->getPassmoney() > 0)) {
				}

				//Notice Client that players lands
				if (pos == playerOnTurn->getPos()) {
					p->setMoved(true);
				}

				if (allPlayersMoved()) {
					handleMoved();
				}
			}
		}
	} else if (command == ".jr") {
		//Roll in jail
		if (p->getHasTurn() && p->getInJail() && (turnState == 1)) {
			roll();

			if (r1 == r2) {
				unjailPlayer();
			} else {
				p->incJailRolls();
				foreach (CapiPlayer* q, players) {
					CapiServerPlayer* pl = (CapiServerPlayer*) q;
					pl->send(createDisplayMsg(translator->translateMessage("No doubles. stay in jail.", pl->getLang(),0), p->getPos(), true));
				}
				if (p->getJailRolls() == 3) {
					payJail();
				} else {
					endTurn();
				}
			}
		}
	} else if (command == ".jp") {
		//pay for jail
		if (p->getHasTurn() && p->getInJail() && (turnState == 1)) {
			payJail();
		}
	} else if (command == ".jc") {
		//Use card
		if (p->getHasTurn() && p->getId() && (turnState == 1)) {
			//check if plyaer owns a jailcard
			//---search for one
			CapiServerCard* c = 0;
			foreach (CapiCard* d, cards) {
				CapiServerCard* e = (CapiServerCard*) d;
				if ((e->getOwner() == p) && e->getOutOfJail()) {
					c = e;
				}
			}
			if (c != 0) {
				//found one
				c->setOwner(0);
				p->setInJail(false);
				sendToAll(c->getUpdate()+p->getUpdate("jailed"));
				waitForRoll();
			}
		}
	} else if (command == ".eb") {
		//Buy estate
		if ((turnState == 4) && p->getHasTurn() && getEstate(p->getPos())->getBuyable() && (getEstate(p->getPos())->getOwner() == 0)) {
			CapiServerEstate* e = getServerEstate(p->getPos());
			if (p->getMoney() >= e->getPrice()) {
				e->setOwner(p);
				setCanBuyOrSellHouses((CapiServerGroup*) e->getGroup());
				e->setCanToggleMortage(true);
				p->setMoney(p->getMoney()-e->getPrice());
				p->setCanBuyEstate(false);
				p->setCanAuction(false);
				vars.insert("%{player}", p->getName());
				foreach (CapiPlayer* q, players) {
					CapiServerPlayer* pl = (CapiServerPlayer*) q;
					vars.insert("%{estate}", e->getName(pl->getLang()));
					pl->send(createDisplayMsg(translator->translateMessage("%{player} buys %{estate}.", pl->getLang(), &vars), e->getId()));
				}
				sendToAll(p->getUpdate("money can_buyestate canauction")+e->getUpdate("owner can_toggle_mortgage"));
				wasDouble();
			} else {
				QMap<QString, QString> vars;
				vars.insert("%{estate}", e->getName(p->getLang()));
				p->sendMsg(translator->translateMessage("You don't have enough money to buy %{estate}.", p->getLang(), &vars), "error");
			}
		}
	} else if (command == ".ea") {
		if ((turnState == 4) && p->getHasTurn() && getEstate(p->getPos())->getBuyable() && (getEstate(p->getPos())->getOwner() == 0)) {
			p->setCanBuyEstate(false);
			p->setCanAuction(false);
			sendToAll(p->getUpdate("can_buyestate canauction"));
			QMap<QString, QString> vars;
			vars.insert("%{player}", p->getName());
			foreach (CapiPlayer* q, players) {
				CapiServerPlayer* pl = (CapiServerPlayer*) q;
				vars.insert("%{estate}", getServerEstate(p->getPos())->getName(pl->getLang()));
				pl->send(createDisplayMsg(translator->translateMessage("%{player} chooses to auction %{estate}.", pl->getLang(), &vars), p->getPos()));
			}
			auction();
		}
	} else if (command.startsWith(".ab")) {
		if (turnState == 5) {
			QStringList list = command.remove(0,3).split(":");
			if (list.size() >= 2) {
				bool ok1, ok2;
				int aid = ((QString) list.value(0)).toInt(&ok1);
				int bid = ((QString) list.value(1)).toInt(&ok2);

				if (ok1 && ok2) {
					if (aid == auctionid) {
						auctionBid(p, bid);
					} else {
						p->sendMsg(translator->translateMessage("Invalid auctionid.", p->getLang(),0), "error");
					}
				} else {
					p->sendMsg(translator->translateMessage("Invalid parameters.", p->getLang(),0), "error");
				}
			} else {
				p->sendMsg(translator->translateMessage("You need two parameters.", p->getLang(),0), "error");
			}
		} else {
			p->sendMsg(translator->translateMessage("Currently there is no auction.", p->getLang(),0), "error");
		}
	} else if (command == ".E") {
		if ((turnState == 4) && p->getHasTurn() && getEstate(p->getPos())->getBuyable() && (getEstate(p->getPos())->getOwner() == 0)) {
			p->setCanBuyEstate(false);
			p->setCanAuction(false);
			sendToAll(p->getUpdate("canauction can_buyestate"));
			wasDouble();
		}
	} else if (command.startsWith(".es")) {
		if (sellEnabled) {
			bool ok = false;
			int eid = command.remove(0,3).toInt(&ok);
			if (ok) {
				CapiServerEstate* e = (CapiServerEstate*) getEstate(eid, false);
				if (e != 0) {
					if (e->getOwner() == p) {
						if (e->getGroup()->getNumHouses() == 0) {
							if (!e->getMortaged()) {
								e->setOwner(0);
								p->setMoney(p->getMoney()+e->getPrice()*0.75);
								sendToAll(e->getUpdate("owner")+p->getUpdate("money"));
								QMap<QString, QString> vars;
								vars.insert("%{player}", p->getName());
								foreach (CapiPlayer* q, players) {
									CapiServerPlayer* pl = (CapiServerPlayer*) q;
									vars.insert("%{estate}", getServerEstate(p->getPos())->getName(pl->getLang()));
									pl->send(createDisplayMsg(translator->translateMessage("%{player} selled %{estate}.", pl->getLang(), &vars), playerOnTurn->getPos()));
								}
								setCanBuyOrSellHouses((CapiServerGroup*) e->getGroup());
								checkForPayment();
							} else {
								p->sendMsg(translator->translateMessage("You cannot sell mortgaged estates.",p->getLang(),0), "error");
							}
						} else {
							p->sendMsg(translator->translateMessage("You cannot sell an estates with houses on its group.",p->getLang(),0), "error");
						}
					} else {
						QMap<QString, QString> vars;
						vars.insert("%{estate}", e->getName(p->getLang()));
						p->sendMsg(translator->translateMessage("You do not own %{estate}.", p->getLang(),0), "error");
					}
				} else {
					p->sendMsg(translator->translateMessage("Estate not found.",p->getLang(),0), "error");
				}
			} else {
				p->sendMsg(translator->translateMessage("Could not parse estateid.", p->getLang(),0), "error");
			}
		} else {
			p->sendMsg(translator->translateMessage("Selling is not allowed in this game.",p->getLang(),0), "error");
		}
	} else if (command == ".T$") {
		if (p->getHasTurn() && (turnState == 6) && (taxamount != 0)) {
			QMap<QString, QString> vars;
			vars.insert("%{player}", p->getName());
			vars.insert("%{tax}", QString:: number(taxamount));
			foreach (CapiPlayer* q, players) {
				CapiServerPlayer* pl = (CapiServerPlayer*) q;
				pl->send(createDisplayMsg(translator->translateMessage("%{player} chooses to pay %{tax}.", pl->getLang(), &vars), p->getPos()));
			}
			CapiEstate* targetEstate = getServerEstate(p->getPos())->getPayTarget();
			depts.append(new CapiDept(p, 0, targetEstate, taxamount));
			waitForDept();
		}
	} else if (command == ".T%") {
		if (p->getHasTurn() && (turnState == 6) && (taxrate != 0)) {
			QMap<QString, QString> vars;
			vars.insert("%{player}", p->getName());
			vars.insert("%{tax}", QString:: number(taxrate)+"%");
			foreach (CapiPlayer* q, players) {
				CapiServerPlayer* pl = (CapiServerPlayer*) q;
				pl->send(createDisplayMsg(translator->translateMessage("%{player} chooses to pay %{tax}.", pl->getLang(), &vars), p->getPos()));
			}
			int amount = (taxrate*assetsForPlayer(playerOnTurn))/100;
			CapiEstate* targetEstate = getServerEstate(p->getPos())->getPayTarget();
			depts.append(new CapiDept(p, 0, targetEstate, amount));
			waitForDept();
		}
	} else if (command.startsWith(".em")) {
		//(Un)Mortgage estate
		if (turnState != 3) {
			//Only allowed when players are not moving
			bool ok;

			int eid = command.remove(0,3).toInt(&ok);
			if (ok) {
				CapiServerEstate* e = getServerEstate(eid);

				//Only owner can toggle mortgage
				if (e->getOwner() == p) {
					int mprice = e->getUnmortageVal();
					if (!e->getMortaged()) mprice = (-1)*e->getMortageVal();

					if ((mprice < 0) || (p->getMoney() > mprice)) {
						if (e->getCanToggleMortage()) {
							e->setMortaged(!e->getMortaged());
							setCanBuyOrSellHouses((CapiServerGroup*) e->getGroup());
							p->setMoney(p->getMoney()-mprice);
							QString msg;
							if (e->getMortaged())
								msg = "%{player} mortgaged %{estate}.";
							else
								msg = "%{player} unmortgaged %{estate}.";

							vars.insert("%{player}", p->getName());
							foreach (CapiPlayer* q, players) {
								CapiServerPlayer* pl = (CapiServerPlayer*) q;
								vars.insert("%{estate}", e->getName(pl->getLang()));
								pl->send(createDisplayMsg(translator->translateMessage(msg, pl->getLang(),&vars),playerOnTurn->getPos()));
							}
							sendToAll(p->getUpdate("money")+e->getUpdate("mortgaged"));

							checkForPayment();
						} else {
							vars.insert("%{estate}", e->getName(p->getLang()));
							p->sendMsg(translator->translateMessage("You cannot toggle mortgage of %{estate}.", p->getLang(), &vars), "error");
						}
					} else {
						vars.insert("%{money}", QString::number(mprice));
						vars.insert("%{estate}", e->getName(p->getLang()));
						p->sendMsg(translator->translateMessage("You need %{money} to unmortgage %{estate}.", p->getLang(), &vars), "error");
					}
				} else {
					vars.insert("%{estate}", e->getName(p->getLang()));
					p->sendMsg(translator->translateMessage("You don't own %{estate}. It is unfair to mortgage other players estates.", p->getLang(), &vars), "error");
				}
			} else {
				p->sendMsg(translator->translateMessage("Invalid Estate-ID.", p->getLang(),0), "error");
			}
		} else {
			p->sendMsg(translator->translateMessage("You cannot toggle mortgage while players are moving.", p->getLang(),0), "error");
		}
	} else if (command.startsWith(".hb")) {
		//Buy house on estate
		if (turnState != 3) {
			bool ok;
			int eid = command.remove(0,3).toInt(&ok);
			if (ok) {
				CapiServerEstate* e = getServerEstate(eid);
				if ((e->getOwner() == p) && e->getCanBuyHouse()) {
					if (e->getHousePrice() <= p->getMoney()) {
						//Check if we may buy a house/hotel
						int houses = getNumHouses();
						int hotels = getNumHotels();
						if ((houses < maxHouses) && ((e->getHouses() != 4) || (hotels < maxHotels))) {
							e->setHouses(e->getHouses()+1);

							setCanBuyOrSellHouses((CapiServerGroup*) e->getGroup());
							p->setMoney(p->getMoney()-e->getHousePrice());
							sendToAll(p->getUpdate("money")+e->getUpdate("houses"));

							QString msg = "%{player} buys a house on %{estate}.";
							if (e->getHouses() > 4) msg = "%{player} buys a hotel on %{estate}.";
							vars.insert("%{player}", p->getName());
							foreach (CapiPlayer* q, players) {
								CapiServerPlayer* pl = (CapiServerPlayer*) q;
								vars.insert("%{estate}", e->getName(pl->getLang()));
								pl->send(createDisplayMsg(translator->translateMessage(msg, pl->getLang(),&vars), playerOnTurn->getPos()));
							}
							checkForPayment();
						} else {
							if (e->getHouses() < 4)
								p->sendMsg(translator->translateMessage("Cannot buy a house. All houses are selled.", p->getLang(),0), "error");
							else
								p->sendMsg(translator->translateMessage("Cannot buy a hotel. All hotels are selled.",p->getLang(),0), "error");
						}
					} else {
						vars.insert("%{money}", QString::number(e->getHousePrice()));
						vars.insert("%{estate}", e->getName(p->getLang()));
						p->sendMsg(translator->translateMessage("You need %{money} to buy a house or hotel on %{estate}.", p->getLang(), &vars), "error");
					}
				} else {
					vars.insert("%{estate}", e->getName(p->getLang()));
					p->sendMsg(translator->translateMessage("You cannot buy a house or hotel on %{estate}. Is it really your estate?", p->getLang(), &vars), "error");
				}
			} else {
				p->sendMsg(translator->translateMessage("Invalid Estate-ID.", p->getLang(),0), "error");
			}
		} else {
			p->sendMsg(translator->translateMessage("Wait until the movements are finished.", p->getLang(),0), "error");
		}
	} else if (command.startsWith(".hrb")) {
		//Buy a houserow on group
		if (turnState != 3) {
			bool ok;
			int gid = command.remove(0,4).toInt(&ok);
			if (ok) {
				CapiServerGroup* g = (CapiServerGroup*) getGroup(gid);
				int maxH = 0;
				for (int i = 0; i < g->getNumEstates(); i++)
					if (g->getEstate(i)->getHouses() > maxH) maxH = g->getEstate(i)->getHouses();

				int neededHouses = g->getNumEstates();
				int neededHotels = 0;
				for (int i = 0; i < g->getNumEstates(); i++) {
					CapiEstate* es = g->getEstate(i);
					if (es->getHouses() == 4) {
						neededHotels += 1;
					}
				}
				if ((getNumHouses()+neededHouses <= maxHouses) && (getNumHotels()+neededHotels <= maxHotels)) {
					if (g->getAllEstatesSameOwner() && (g->getEstate(0)->getOwner() == p) && (maxH < 5)) {
						int neededMoney = 0;
						for (int i = 0; i < g->getNumEstates(); i++)
							neededMoney += g->getEstate(i)->getHousePrice();
						if (neededMoney <= p->getMoney()) {
							for (int i = 0; i < g->getNumEstates(); i++) {
								CapiServerEstate* e = (CapiServerEstate*) g->getEstate(i);
								e->setHouses(e->getHouses()+1);
								p->setMoney(p->getMoney()-e->getHousePrice());
								sendToAll(p->getUpdate("money")+e->getUpdate("houses"));
							}
							setCanBuyOrSellHouses((CapiServerGroup*) g);
							vars.insert("%{player}", p->getName());
							foreach (CapiPlayer* q, players) {
								CapiServerPlayer* pl = (CapiServerPlayer*) q;
								vars.insert("%{group}", g->getName(pl->getLang()));
								pl->send(createDisplayMsg(translator->translateMessage("%{player} buys a houserow on group %{group}.", pl->getLang(), &vars), playerOnTurn->getPos()));
							}
							checkForPayment();
						} else {
							vars.insert("%{money}", QString::number(neededMoney));
							vars.insert("%{group}", g->getName(p->getLang()));
							p->sendMsg(translator->translateMessage("You need %{money} to buy a houserow on %{group}.", p->getLang(), &vars), "error");
						}
					} else {
						vars.insert("%{group}", g->getName(p->getLang()));
						p->sendMsg(translator->translateMessage("You cannot buy a houserow on %{group}.", p->getLang(), &vars), "error");
					}
				} else {
					p->sendMsg(translator->translateMessage("All houses or hotels are selled.", p->getLang(), 0), "error");
				}
			} else {
				p->sendMsg(translator->translateMessage("Invalid Group-ID.", p->getLang(),0), "error");
			}
		} else {
			p->sendMsg(translator->translateMessage("You cannot buy houserows players are moving.",p->getLang(),0), "error");
		}
	} else if (command.startsWith(".hs")) {
		//Sell house on estate
		if (turnState != 3) {
			bool ok;
			int eid = command.remove(0,3).toInt(&ok);
			if (ok) {
				CapiServerEstate* e = getServerEstate(eid);
				if ((e->getOwner() == p) && e->getCanSellHouse()) {
					e->setHouses(e->getHouses()-1);
					setCanBuyOrSellHouses((CapiServerGroup*) e->getGroup());
					p->setMoney(p->getMoney()+e->getSellHousePrice());
					sendToAll(p->getUpdate("money")+e->getUpdate("houses"));

					QString msg = "%{player} sells a house from %{estate}.";
					if (e->getHouses() == 5) msg = "%{player} sell a hotel from %{estate}.";
					vars.insert("%{player}", p->getName());
					foreach (CapiPlayer* q, players) {
						CapiServerPlayer* pl = (CapiServerPlayer*) q;
						vars.insert("%{estate}", e->getName(pl->getLang()));
						pl->send(createDisplayMsg(translator->translateMessage(msg, pl->getLang(),&vars), playerOnTurn->getPos()));
					}
					checkForPayment();
				} else {
					vars.insert("%{estate}", e->getName(p->getLang()));
					p->sendMsg(translator->translateMessage("You cannot sell a house or hotel from %{estate}.", p->getLang(),0), "error");
				}
			} else {
				p->sendMsg(translator->translateMessage("Invalid Estate-ID.", p->getLang(),0), "error");
			}
		} else {
			p->sendMsg(translator->translateMessage("You cannot sell houses or hotels while players are moving.", p->getLang(),0), "error");
		}
	} else if (command.startsWith(".Tn")) {
		bool ok;
		int tpid = command.remove(0,3).toInt(&ok);
		if (ok) {
			CapiServerPlayer* tradep = (CapiServerPlayer*) getPlayer(tpid);
			if (tradep != 0) {
				if (!tradep->getBankrupt()) {
					if (tradep != p) {
						CapiTrade* t = new CapiTrade(tradeid++, p, tradep);
						trades.append(t);
					} else {
						p->sendMsg(translator->translateMessage("Yes! Trading with yourself makes sense. But you don't need me to do this.", p->getLang(),0), "info");
					}
				} else {
					vars.insert("%{player}", tradep->getName());
					p->sendMsg(translator->translateMessage("%{player} is bankrupt.", p->getLang(), &vars), "error");
				}
			} else {
				vars.insert("%{playerid}", QString::number(tpid));
				p->sendMsg(translator->translateMessage("Cannont find player with ID %{playerid}.", p->getLang(),&vars), "error");
			}
		} else {
			p->sendMsg(translator->translateMessage("Invalid Player-ID.",p->getLang(),0), "error");
		}
	} else if (command.startsWith(".Tr")) {
		bool ok;
		int tid = command.remove(0,3).toInt(&ok);
		if (ok) {
			//Find the trade
			CapiTrade* t = 0;
			foreach(CapiTrade* u, trades) if (u->getId() == tid) t = u;

			if (t != 0) {
				if (t->playerInvolved(p)) {
					t->reject();
					trades.removeAll(t);
					delete t;
				} else {
					p->sendMsg(translator->translateMessage("Nice try. But you should not stop a trade, that you cannot know.",p->getLang(),0), "error");
				}
			} else {
				vars.insert("%{tradeid}", QString::number(tid));
				p->sendMsg(translator->translateMessage("Trade with ID %{tradeid} not found.", p->getLang(),&vars), "error");
			}
		} else {
			p->sendMsg(translator->translateMessage("Invalid Trade-ID.",p->getLang(),0), "error");
		}
	} else if (command.startsWith(".Ta")) {
		QStringList list = command.remove(0,3).split(":");
		if (list.size() == 2) {
			bool ok1, ok2;
			int tid = ((QString) list.value(0)).toInt(&ok1);
			int trev = ((QString) list.value(1)).toInt(&ok2);

			if (ok1 && ok2) {
				//Find the trade
				CapiTrade* t = 0;
				foreach(CapiTrade* u, trades) if (u->getId() == tid) t = u;

				if (t != 0) {
					if (t->playerInvolved(p)) {
						t->accept(p, trev);
						if (t->checkForAccepts()) {
							sendToAll(t->run());
							trades.removeAll(t);
							delete t;
							checkForPayment();
							foreach (CapiGroup* g, groups)
								setCanBuyOrSellHouses((CapiServerGroup*) g);
						}
					} else {
						p->sendMsg(translator->translateMessage("Nice try. But you should not accept trades that you cannot know.",p->getLang(),0), "error");
					}
				} else {
					p->sendMsg(translator->translateMessage("Could not find the trade.", p->getLang(),0), "error");
				}
			} else {
				p->sendMsg(translator->translateMessage("Invalid Trade-ID or Revision.",p->getLang(),0), "error");
			}
			}
	} else if (command.startsWith(".Te")) {
		QStringList list = command.remove(0,3).split(":");
		if (list.size() == 3) {
			bool ok1, ok2, ok3;
			int tid = ((QString) list.value(0)).toInt(&ok1);
			int teid = ((QString) list.value(1)).toInt(&ok2);
			int tpid = ((QString) list.value(2)).toInt(&ok3);

			if (ok1 && ok2 && ok3) {
				//Find the trade
				CapiTrade* t = 0;
				foreach(CapiTrade* u, trades) if (u->getId() == tid) t = u;
				if (t != 0) {
					//Found trade
					if (t->playerInvolved(p)) {
						//Player is involved
						CapiEstate* e = getEstate(teid, false);
						if (e != 0) {
							CapiPlayer* tp = getPlayer(tpid);
							if ((tp == 0) || !tp->getBankrupt()) {
								t->updateEstateTrade(tp, e);
							} else {
								vars.insert("%{player}", tp->getName());
								p->sendMsg(translator->translateMessage("%{player} is bankrupt.", p->getLang(), &vars), "error");
							}
						} else {
							p->sendMsg(translator->translateMessage("Unknown Estate-ID.", p->getLang(),0), "error");
						}
					} else {
						p->sendMsg(translator->translateMessage("You cannot change trade condition of trades that you cannot know.", p->getLang(),0), "error");
					}
				} else {
					p->sendMsg(translator->translateMessage("Could not find the trade.", p->getLang(),0), "error");
				}
			} else {
				p->sendMsg(translator->translateMessage("Invalid Trade-ID, Estate-ID or Player-ID.", p->getLang(),0), "error");
			}
		} else {
			p->sendMsg(translator->translateMessage("Incorrect number of parameters.", p->getLang(),0), "error");
		}
	} else if (command.startsWith(".Tc")) {
		QStringList list = command.remove(0,3).split(":");
		if (list.size() == 3) {
			bool ok1, ok2, ok3;
			int tid = ((QString) list.value(0)).toInt(&ok1);
			int tcid = ((QString) list.value(1)).toInt(&ok2);
			int tpid = ((QString) list.value(2)).toInt(&ok3);

			if (ok1 && ok2 && ok3) {
				//Find the trade
				CapiTrade* t = 0;
				foreach(CapiTrade* u, trades) if (u->getId() == tid) t = u;
				if (t != 0) {
					//Found trade
					if (t->playerInvolved(p)) {
						//Player is involved
						CapiCard* c = getCard(tcid, false);
						if (c != 0) {
							CapiPlayer* tp = getPlayer(tpid);
							if ((tp == 0) || !tp->getBankrupt()) {
								t->updateCardTrade(tp, c);
							} else {
								vars.insert("%{player}", tp->getName());
								p->sendMsg(translator->translateMessage("%{player} is bankrupt.", p->getLang(), &vars), "error");
							}
						} else {
							p->sendMsg(translator->translateMessage("Unknown Card-ID.",p->getLang(),0), "error");
						}
					} else {
						p->sendMsg(translator->translateMessage("You cannot change conditions of a trade that you cannot know.",p->getLang(),0), "error");
					}
				} else {
					p->sendMsg(translator->translateMessage("Could not find the trade.",p->getLang(),0), "error");
				}
			} else {
				p->sendMsg(translator->translateMessage("Invalid Trade-ID, Card-ID or Player-ID.", p->getLang(),0), "error");
			}
		} else {
			p->sendMsg(translator->translateMessage("Incorrect number of parameters.",p->getLang(),0), "error");
		}
	} else if (command.startsWith(".Tm")) {
		QStringList list = command.remove(0,3).split(":");
		if (list.size() == 4) {
			bool ok1, ok2, ok3, ok4;
			int tid =  ((QString) list.value(0)).toInt(&ok1);
			int tfid = ((QString) list.value(1)).toInt(&ok2);
			int ttid = ((QString) list.value(2)).toInt(&ok3);
			int tm =   ((QString) list.value(3)).toInt(&ok4);

			if (ok1 && ok2 && ok3 && ok4) {
				//Find the trade
				CapiTrade* t = 0;
				foreach(CapiTrade* u, trades) if (u->getId() == tid) t = u;
				if (t != 0) {
					//Found trade
					if (t->playerInvolved(p)) {
						//Player is involved
						CapiPlayer* tp = getPlayer(ttid);
						CapiPlayer* fp = getPlayer(tfid);

						if ((tp == 0) || (fp == 0) || ((!tp->getBankrupt()) && (!fp->getBankrupt()))) {
							t->updateMoneyTrade(fp, tp, tm);
						} else {
							vars.insert("%{player1}", fp->getName());
							vars.insert("%{player2}", tp->getName());

							p->sendMsg(translator->translateMessage("%{player1} or %{player2} is bankrupt.", p->getLang(), &vars), "error");
						}
					} else {
						p->sendMsg(translator->translateMessage("You cannot change trade condition of trades that you cannot know.",p->getLang(),0), "error");
					}
				} else {
					p->sendMsg(translator->translateMessage("Could not find the trade.",p->getLang(),0), "error");
				}
			} else {
				p->sendMsg(translator->translateMessage("Invalid Trade-ID, or Player-IDs.",p->getLang(),0), "error");
			}
		} else {
			p->sendMsg(translator->translateMessage("Incorrect number of parameters.",p->getLang(),0), "error");
		}
	} else if (command.startsWith(".gu")) {
		bool ok;
		int upid = command.remove(0,3).toInt(&ok);
		if (ok) {
			CapiServerPlayer* uppl = (CapiServerPlayer*) getPlayer(upid);
			if (uppl != 0) {
				if (getMaster() == p) {
					if (getStatus() == "config") {
						uppl->setSpectator(false);
						sendToAll(uppl->getUpdate("spectator"));
					} else {
						p->sendMsg(translator->translateMessage("You cannot upgrade a spectator in a running game.",p->getLang(),0));
					}
				} else {
					p->sendMsg(translator->translateMessage("You are not Master of the game.",p->getLang(),0), "error");
				}
			} else {
				p->sendMsg(translator->translateMessage("Invalid Player-ID.",p->getLang(),0), "error");
			}
		} else {
			p->sendMsg(translator->translateMessage("Could not parse Player-ID.",p->getLang(),0), "error");
		}
	} else if (command.startsWith(".gk")) {
		bool ok;
		int kid = command.remove(0,3).toInt(&ok);
		if (ok) {
			CapiServerPlayer* kpl = (CapiServerPlayer*) getPlayer(kid);
			if (kpl != 0) {
				if (getMaster() == p) {
					if (getStatus() == "config") {
						sendToAll(removePlayerFromGame(kpl));
						kpl->send(kpl->getUpdate());
					} else {
						p->sendMsg(translator->translateMessage("You cannot kick a player in a running game.",p->getLang(),0), "error");
					}
				} else {
					p->sendMsg(translator->translateMessage("You are not Master of the game.",p->getLang(),0), "error");
				}
			} else {
				p->sendMsg(translator->translateMessage("Invalid Player-ID.",p->getLang(),0), "error");
			}
		} else {
			p->sendMsg(translator->translateMessage("Could not parse Player-ID.",p->getLang(),0), "error");
		}
	} else if (command==".cheat") {
		/*
		//A small cheat for debugging
		p->setMoney(10000);
		sendToAll(p->getUpdate("money"));
		vars.insert("%{player}", p->getName());
		foreach (CapiPlayer* q, players) {
			CapiServerPlayer* pl =(CapiServerPlayer*) q;
			pl->send(createDisplayMsg(translator->translateMessage("%{player} cheated!!!",pl->getLang(), &vars), playerOnTurn->getPos()));
		}
		checkForPayment();
		*/
	} else {
			p->sendMsg(translator->translateMessage("Unknown command.",p->getLang(),0), "error");
	}
}

QString CapiServerGame::createButton(QString cmd, QString text) {
	return QString("<button command=\"%1\" caption=\"%2\" enabled=\"1\" />").arg(maskXML(cmd), maskXML(text));
}

QString CapiServerGame::createDisplayMsg(QString text, int estateid, bool clearText, bool clearButtons, QString buttons) {
	QString ret;
	ret = QString("<display estateid=\"%1\" text=\"%2\" cleartext=\"%3\" clearbuttons=\"%4\">").arg(estateid).arg(maskXML(text)).arg(clearText).arg(clearButtons);
	ret += buttons;
	ret += "</display>";
	
	return ret;
}

void CapiServerGame::sendToAll(QString msg, int exclude) {
	foreach(CapiPlayer* p, players) {
		CapiServerPlayer* q = (CapiServerPlayer*) p;
		if (q->getId() != exclude) q->send(msg);
	}
}

bool CapiServerGame::allPlayersMoved() {
	foreach(CapiPlayer* p, players) {
		CapiServerPlayer* q = (CapiServerPlayer*) p;
		if (!q->getMoved()) return false;
	}
	return true;
}

bool CapiServerGame::playerOwnsOutOfJailCard(CapiServerPlayer* p) {
	foreach (CapiCard* d, cards) {
		CapiServerCard* c = (CapiServerCard*) d;
		if (c->getOutOfJail() && (c->getOwner() == p)) return true;
	}
	return false;
}

void CapiServerGame::roll(bool forEstate) {
	if (forEstate) {
		payR1 = (rand() % 6) + 1;
		payR2 = (rand() % 6) + 1;
	} else {
		r1 = (rand() % 6) + 1;
		r2 = (rand() % 6) + 1;
		payR1 = r1;
		payR2 = r2;
	}
}

void CapiServerGame::waitForRoll() {
	turnState = 0;
	playerOnTurn->setCanRoll(true);
	sendToAll(playerOnTurn->getUpdate("hasturn can_roll"));
}

void CapiServerGame::waitForJail() {
	turnState = 1;
	playerOnTurn->setCanRoll(false);
	playerOnTurn->setCanUseCard(playerOwnsOutOfJailCard(playerOnTurn));
	QString buttons = createButton(".jr", translator->translateMessage("Roll",playerOnTurn->getLang(),0));
	if (playerOnTurn->getCanUseCard()) buttons += createButton(".jc", translator->translateMessage("Use card",playerOnTurn->getLang(),0));
	buttons += createButton(".jp", translator->translateMessage("Pay",playerOnTurn->getLang(),0));
	sendToAll(playerOnTurn->getUpdate("hasturn can_roll canusecard"));
	vars.insert("%{player}", playerOnTurn->getName());
	vars.insert("%{jailround}", QString::number(playerOnTurn->getJailRolls()+1));
	foreach (CapiPlayer* q, players) {
		CapiServerPlayer* pl = (CapiServerPlayer*) q;
		if (pl != playerOnTurn)
			pl->send(createDisplayMsg(translator->translateMessage("%{player} is in jail. Round: %{jailround}",pl->getLang(),&vars), playerOnTurn->getPos(), false, false));
		else
			pl->send(createDisplayMsg(translator->translateMessage("%{player} is in jail. Round: %{jailround}",pl->getLang(),&vars), playerOnTurn->getPos(), false, false, buttons));
	}
}

void CapiServerGame::payJail() {
	turnState = 2;
	//Pay the money
	CapiServerEstate* e = getServerEstate(playerOnTurn->getPos());
	if (playerOnTurn->getMoney() >= e->getPayAmount()) {
		playerOnTurn->setMoney(playerOnTurn->getMoney()-e->getPayAmount());
		playerOnTurn->setInJail(false);
		if (e->getPayTarget() != 0) {
			CapiServerEstate* t = (CapiServerEstate*) e->getPayTarget();
			t->setMoney(t->getMoney()+e->getPayAmount());
			sendToAll(t->getUpdate("money"));
		}
		vars.insert("%{player}", playerOnTurn->getName());
		vars.insert("%{money}", QString::number(e->getPayAmount()));
		foreach (CapiPlayer* q, players) {
			CapiServerPlayer* pl = (CapiServerPlayer*) q;
			pl->send(createDisplayMsg(translator->translateMessage("%{player} has payed %{money} and now leaved jail.", pl->getLang(), &vars), playerOnTurn->getPos()));
		}
		sendToAll(playerOnTurn->getUpdate("money jailed"));
		waitForRoll();
	} else {
		vars.insert("%{player}", playerOnTurn->getName());
		vars.insert("%{money}", QString::number(e->getPayAmount()));
		foreach (CapiPlayer* q, players) {
			CapiServerPlayer* pl = (CapiServerPlayer*) q;
			pl->send(createDisplayMsg(translator->translateMessage("%{player} must pay %{money}. He/She must raise his/her cash.", pl->getLang(), &vars), playerOnTurn->getPos()));
		}
	}
}

void CapiServerGame::waitForMove() {
	turnState = 3;
	playersMoved = false;
	//Notice all clients to move the player
	foreach (CapiPlayer* p, players) {
		CapiServerPlayer* pl = (CapiServerPlayer*) p;
		if (pl->getSocket() != 0)
			pl->setMoved(false);

		pl->setPosMoveTo(playerOnTurn->getPos());
		p->setDirectmove(directmove);
	}
	sendToAll(playerOnTurn->getUpdate("location can_roll directmove"));
}

void CapiServerGame::waitForChoice() {
	turnState = 4;
	CapiServerEstate* e = getServerEstate(playerOnTurn->getPos());
	QString buttons = createButton(".eb", "Buy");
	if (auctions) buttons += createButton(".ea", translator->translateMessage("Auction", playerOnTurn->getLang(), 0));
	else buttons += createButton(".E", translator->translateMessage("Do not Buy", playerOnTurn->getLang(),0));
	vars.insert("%{player}", playerOnTurn->getName());
	foreach (CapiPlayer* q, players) {
		CapiServerPlayer* pl = (CapiServerPlayer*) q;
		vars.insert("%{estate}", e->getName(pl->getLang()));
		if (pl != playerOnTurn)
			pl->send(createDisplayMsg(translator->translateMessage("%{player} can buy %{estate}.", pl->getLang(), &vars), e->getId(), false, false));
		else
			pl->send(createDisplayMsg(translator->translateMessage("%{player} can buy %{estate}.", pl->getLang(), &vars), e->getId(), false, false, buttons));
	}

	playerOnTurn->setCanAuction(auctions);
	playerOnTurn->setCanBuyEstate(true);

	sendToAll(playerOnTurn->getUpdate("canauction can_buyestate"));
}

void CapiServerGame::auction() {
	turnState = 5;
	//Begin a new auction
	auctionid = 1;
	auctionStatus = 0;
	auctionActor = playerOnTurn;
	auctionEstate = getServerEstate(playerOnTurn->getPos());
	auctionHighBidder = 0;
	auctionHighestBid = 0;
	statusCount = 0;

	sendToAll(auctionUpdate("actor, status, estateid"));
}

void CapiServerGame::auctionBid(CapiServerPlayer* bidder, int bid) {
	if (auctionid == -1) {
		bidder->sendMsg(translator->translateMessage("Currently there is no auction.", bidder->getLang(),0), "error");
		return;
	}

	if (bid <= auctionHighestBid) {
		vars.insert("%{money}", QString::number(auctionHighestBid));
		bidder->sendMsg(translator->translateMessage("Minimum bid is %{money}.", bidder->getLang(), &vars), "error");
		return;
	}

	if (bid > assetsForPlayer(bidder)) {
		bidder->sendMsg(translator->translateMessage("You can't bid more than you have.", bidder->getLang(),0), "error");
		return;
	}
	if (auctionStatus == 3) {
		bidder->sendMsg(translator->translateMessage("Too late for bids.", bidder->getLang(),0), "error");
		return;
	}

	auctionHighestBid = bid;
	auctionHighBidder = bidder;
	auctionStatus = 0;

	sendToAll(auctionUpdate("highbid highbidder status"));
}

void CapiServerGame::waitForAuctionPay() {

	if (auctionid == -1) return;
	if (auctionStatus != 3) return;

	if (auctionHighBidder->getMoney() >= auctionHighestBid) {
		//Highest Bidder can pay the bid
		auctionEstate->setOwner(auctionHighBidder);
		auctionHighBidder->setMoney(auctionHighBidder->getMoney()-auctionHighestBid);
		sendToAll(auctionEstate->getUpdate("owner")+auctionHighBidder->getUpdate("money"));
		resetAuction();
		wasDouble();
		return;
	}

	if (auctionHighestBid > assetsForPlayer(auctionHighBidder)) {
		//Highest bidder can't pay it. Stop auction
		vars.insert("%{player}", auctionHighBidder->getName());
		foreach (CapiPlayer* q, players) {
			CapiServerPlayer* pl = (CapiServerPlayer*) q;
			vars.insert("%{estate}", auctionEstate->getName(pl->getLang()));
			pl->send(createDisplayMsg(translator->translateMessage("%{player} was so stupid to waste enough money so that he/she can't pay %{estate} anymore.", pl->getLang(), &vars), playerOnTurn->getPos()));
		}
		vars.insert("%{estate}", auctionEstate->getName(auctionHighBidder->getLang()));
		auctionHighBidder->send(createDisplayMsg(translator->translateMessage("How the hell can you waste so much money?? If you don't want to have %{estate}, you should not bid for it. Next time i'll let you go bankrupt!", auctionHighBidder->getLang(), &vars), playerOnTurn->getPos()));
		resetAuction();
		wasDouble();
		return;
	}

	//We get here if player can pay hid bid, but must raise cash first
	vars.insert("%{player}", auctionHighBidder->getName());
	vars.insert("%{money}", QString::number(auctionHighestBid));
	foreach (CapiPlayer* q, players) {
		CapiServerPlayer* pl = (CapiServerPlayer*) q;
		vars.insert("%{estate}", auctionEstate->getName(pl->getLang()));
		pl->send(createDisplayMsg(translator->translateMessage("%{player} must raise his/her cash to %{money} to pay %{estate}.", pl->getLang(), &vars), playerOnTurn->getPos()));
	}
}

void CapiServerGame::resetAuction() {
		//Reset auction data
		auctionState = 0;
		auctionid = -1;
		auctionStatus = 0;
		auctionActor = 0;
		auctionEstate = 0;
		auctionHighBidder = 0;
		auctionHighestBid = 0;
		statusCount = 0;
}

QString CapiServerGame::auctionUpdate(QString type) {
	if (auctionid == -1) return "";

	QString ret = "<auctionupdate ";
	ret += QString("auctionid=\"%1\" ").arg(auctionid);
	if ((type == "") || type.contains("actor"))      ret += QString("actor=\"%1\" ").arg(playerOnTurn->getId());
	if ((type == "") || type.contains("estateid"))   ret += QString("estateid=\"%1\" ").arg(auctionEstate->getId());
	if ((type == "") || type.contains("highbid"))    ret += QString("highbid=\"%1\" ").arg(auctionHighestBid);
	if ((type == "") || type.contains("highbidder")) ret += QString("highbidder=\"%1\" ").arg(auctionHighBidder->getId());
	if ((type == "") || type.contains("status"))     ret += QString("status=\"%1\" ").arg(auctionStatus);

	ret += " />";
	return ret;
}

void CapiServerGame::waitForTax(int ta, int tr) {
	turnState = 6;

	taxamount = ta;
	taxrate = tr;
}

void CapiServerGame::waitForDept() {
	turnState = 7;

	//TODO
	//reduce deps
	//Use dept-graph

	bool allPayed = true;
	foreach (CapiDept* d, depts) {
		if (d->getPayed()) continue;

		//Check if ower can pay the dept
		if (d->getAmount() > assetsForPlayer(d->getFromPlayer())) {
			//ower is not able to raise his cash to the dept-amount
			bankruptPlayer(d);
		} else {
			//owner can raise his cash, or pay the dept
			//Depts was not payed yet try to pay
			bool payed = d->pay();

			QString target = "the bank";
			if (d->getToPlayer() != 0) target = d->getToPlayer()->getName();
			if (!payed) {
				//ower can not pay the dept
				vars.insert("%{playerfrom}", d->getFromPlayer()->getName());
				vars.insert("%{playerto}", target);
				vars.insert("%{money}", QString:: number(d->getAmount()));
				foreach (CapiPlayer* q, players) {
					CapiServerPlayer* pl = (CapiServerPlayer*) q;
					if (d->getToPlayer() == 0) vars.insert("%{playerto}", translator->translateMessage(target, pl->getLang(),0));
					pl->send(createDisplayMsg(translator->translateMessage("%{playerfrom} owes %{money} to %{playerto} but is not solvent. %{playerfrom} must raise his/her cash to %{money} frist.", pl->getLang(), &vars), playerOnTurn->getPos()));
				}
				allPayed = false;
			} else {
				//ower has paid the dept
				vars.insert("%{playerfrom}", d->getFromPlayer()->getName());
				vars.insert("%{playerto}", target);
				vars.insert("%{money}", QString:: number(d->getAmount()));
				foreach (CapiPlayer* q, players) {
					CapiServerPlayer* pl = (CapiServerPlayer*) q;
					if (d->getToPlayer() == 0) vars.insert("%{playerto}", translator->translateMessage(target, pl->getLang(),0));
					pl->send(createDisplayMsg(translator->translateMessage("%{playerfrom} pays %{money} to %{playerto}.", pl->getLang(), &vars), playerOnTurn->getPos()));
				}
				sendToAll(((CapiServerPlayer*) d->getFromPlayer())->getUpdate("money"));
				if (d->getToPlayer() != 0)
					sendToAll(((CapiServerPlayer*) d->getToPlayer())->getUpdate("money"));
				if (d->getToEstate() != 0)
					sendToAll(((CapiServerEstate*) d->getToEstate())->getUpdate("money"));
			}
		}
	}

	if (allPayed) {
		//If all depts payed delete all depts and continue game
		while (depts.size() > 0) {
			CapiDept* d = depts.value(0);
			depts.removeAll(d);
			delete d;
		}
		if (deptRound > 0) {
			foreach (CapiPlayer* q, players) {
				CapiServerPlayer* pl = (CapiServerPlayer*) q;
				pl->send(createDisplayMsg(translator->translateMessage("All depts settled. Game continues.", pl->getLang(), 0), playerOnTurn->getPos()));
			}
		}
		deptRound = 0;
		movedPlayer();
	} else {
		deptRound++;
	}
}

void CapiServerGame::handleMoved() {
	if (!handleMovedLock.tryLock()) return;

	if (playersMoved) return;
	playersMoved = true;

	CapiServerEstate* e = getServerEstate(playerOnTurn->getPos());
	//All clients have moved
	if (!directmove) {
		//Get all passmoneys
		int ps = oldPos;
		while (ps != playerOnTurn->getPos()) {
			ps = (ps+1)%estates.count();
			CapiServerEstate* es = getServerEstate(ps);
			if (es->getPassmoney() > 0) {
				playerOnTurn->setMoney(playerOnTurn->getMoney()+es->getPassmoney());
				QMap<QString, QString> vars;
				vars.insert("%{player}", playerOnTurn->getName());
				vars.insert("%{money}", QString::number(es->getPassmoney()));
				foreach (CapiPlayer* q, players) {
					CapiServerPlayer* pl = (CapiServerPlayer*) q;
					vars.insert("%{estate}", es->getName(pl->getLang()));
					pl->send(createDisplayMsg(translator->translateMessage("%{player} passed %{estate} and gets %{money}.", pl->getLang(), &vars), playerOnTurn->getPos()));
				}
				sendToAll(playerOnTurn->getUpdate("money"));
			}
		}

		//Player gets double money on exact landing, if enabled
		if (doubleMoneyOnExactLanding && (e->getPassmoney() > 0)) {
			playerOnTurn->setMoney(playerOnTurn->getMoney()+e->getPassmoney());
			QMap<QString, QString> vars;
			vars.insert("%{player}", playerOnTurn->getName());
			vars.insert("%{money}", QString::number(e->getPassmoney()));
			foreach (CapiPlayer* q, players) {
				CapiServerPlayer* pl = (CapiServerPlayer*) q;
				vars.insert("%{estate}", e->getName(pl->getLang()));
				pl->send(createDisplayMsg(translator->translateMessage("%{player} gets %{money} for landing on %{estate}.", pl->getLang(), &vars), playerOnTurn->getPos()));
			}
			sendToAll(playerOnTurn->getUpdate("money"));
		}
	}
	QMap<QString, QString> vars;
	vars.insert("%{player}", playerOnTurn->getName());
	foreach (CapiPlayer* q, players) {
		CapiServerPlayer* pl = (CapiServerPlayer*) q;
		vars.insert("%{estate}", e->getName(pl->getLang()));
		pl->send(createDisplayMsg(translator->translateMessage("%{player} lands on %{estate}.", pl->getLang(), &vars), playerOnTurn->getPos()));
	}

	if ((e->getOwner() == 0) && e->getBuyable()) {
		waitForChoice();
	} else {
		handleEstate();
	}

	handleMovedLock.unlock();
}

void CapiServerGame::checkForPayment() {
		if (turnState == 2)
			payJail();

		if (turnState == 5)
			waitForAuctionPay();

		if (turnState == 7)
			waitForDept();
}

void CapiServerGame::jailPlayer() {
	playerOnTurn->setDirectmove(true);
	playerOnTurn->resetJailRolls();

	//search jail
	for (int i = 1; i <= estates.count(); i++) {
		int eid = playerOnTurn->getPos()-i;
		while (eid < 0) eid += estates.count();
		while (eid >= estates.count()) eid -= estates.count();
		CapiServerEstate* e = getServerEstate(eid);
		if (e->getJail()) {
			playerOnTurn->setPos(eid);
			break;
		}
	}

	playerOnTurn->setInJail(true);
	sendToAll(playerOnTurn->getUpdate("can_roll location jailed directmove"));
}

void CapiServerGame::unjailPlayer() {
	playerOnTurn->setInJail(false);
	playerOnTurn->setDirectmove(false);

	//Caluclate new postion
	int newpos = (playerOnTurn->getPos()+r1+r2) % estates.count();
	QString txt = QString("%{player} rolls doubles and can leave jail.").arg(playerOnTurn->getName());
	vars.insert("%{player}", playerOnTurn->getName());
	foreach (CapiPlayer* q, players) {
		CapiServerPlayer* pl = (CapiServerPlayer*) q;
		pl->send(createDisplayMsg(translator->translateMessage(txt, pl->getLang(), &vars), playerOnTurn->getPos()));
	}
	oldPos = playerOnTurn->getPos();
	playerOnTurn->setPos(newpos);

	sendToAll(playerOnTurn->getUpdate("jailed directmove pos"));
	waitForMove();
}

void CapiServerGame::handleEstate() {
	bool wait = false;
	playerMoved = false;
	directmove = false;
	CapiServerEstate* e = getServerEstate(playerOnTurn->getPos());

	//GoToJailestate
	if (e->getToJail()) {
		jailPlayer();
	}

	//Free parking estate
	if (collectFine && e->getFreeParking() && (e->getMoney() > 0)) {
		vars.insert("%{player}", playerOnTurn->getName());
		vars.insert("%{money}", QString::number(e->getMoney()));
		foreach (CapiPlayer* q, players) {
			CapiServerPlayer* pl = (CapiServerPlayer*) q;
			pl->send(createDisplayMsg(translator->translateMessage("%{player} collects %{money} from fine.", pl->getLang(), &vars), e->getId()));
		}

		playerOnTurn->setMoney(playerOnTurn->getMoney()+e->getMoney());
		e->setMoney(0);
		sendToAll(playerOnTurn->getUpdate("money")+e->getUpdate("money"));
	}

	//Tax estate
	int ta = e->getTax();
	int tr = e->getTaxPercent();
	int options = 0;
	if (ta > 0) options++;
	if (tr > 0) options++;

	if (options == 2) {
		vars.insert("%{player}", playerOnTurn->getName());
		vars.insert("%{taxamount}", QString::number(ta));
		vars.insert("%{taxrate}", QString::number(tr));
		QString txt = QString("%{player} must pay %{taxamount} or %{taxrate}% of his/her assets.");
		QString buttons = createButton(".T$", translator->translateMessage("Pay %{taxamount}", playerOnTurn->getLang(), &vars));
		buttons += createButton(".T%", translator->translateMessage("Pay %{taxrate}%", playerOnTurn->getLang(), &vars));
		if (!autoTax) {
			foreach (CapiPlayer* q, players) {
				CapiServerPlayer* pl = (CapiServerPlayer*) q;
				if (pl != playerOnTurn) {
					pl->send(createDisplayMsg(translator->translateMessage(txt, pl->getLang(), &vars), e->getId()));
				} else {
					pl->send(createDisplayMsg(translator->translateMessage(txt, pl->getLang(), &vars), e->getId(), false, false, buttons));
				}
			}
			waitForTax(ta, tr);
			wait = true;
		} else {
			foreach (CapiPlayer* q, players) {
				CapiServerPlayer* pl = (CapiServerPlayer*) q;
				pl->send(createDisplayMsg(translator->translateMessage(txt, pl->getLang(), &vars), e->getId()));
			}
			//If automate tax decition is active, get min tax rate
			int trrate = (tr*assetsForPlayer(playerOnTurn))/100;
			int tarate = ta;
			int tax = tarate;
			if (trrate < tarate) tax = trrate;

			depts.append(new CapiDept(playerOnTurn, 0, e->getPayTarget(), tax));
			waitForDept();
			wait = true;
		}
	} else if (options == 1) {
		int taxrate = (tr*assetsForPlayer(playerOnTurn))/100;
		taxrate += ta;
		vars.insert("%{money}", QString::number(taxrate));
		QString txt = QString("%{player} must pay %{money}.");
		foreach (CapiPlayer* q, players) {
			CapiServerPlayer* pl = (CapiServerPlayer*) q;
			pl->send(createDisplayMsg(translator->translateMessage(txt, pl->getLang(), &vars), e->getId()));
		}

		depts.append(new CapiDept(playerOnTurn, 0, e->getPayTarget(), taxrate));
		waitForDept();
		wait = true;
	}

	//Handle rent
	if (e->getOwner() != 0) {
		CapiServerGroup* g = (CapiServerGroup*) e->getGroup();
		CapiServerPlayer* owner = (CapiServerPlayer*) e->getOwner();

		//Estate is owned by somebody
		if (owner == playerOnTurn) {
			//We land on our own estate
			vars.insert("%{player}", playerOnTurn->getName());
			foreach (CapiPlayer* q, players) {
				CapiServerPlayer* pl = (CapiServerPlayer*) q;
				pl->send(createDisplayMsg(translator->translateMessage("%{player} already owns it.", pl->getLang(), &vars), e->getId()));
			}

		} else {
			int rent = 0;
			if (!e->getMortaged()) {
				//calulate the rent for the estate in two steps:
				//first: normal rent
				//second: by rentmath

				//first normal rent
				rent = e->getRent(e->getHouses());
				if (e->getHouses() == 0) {
					//We must double rent for no houses, if owner own tha whole group
					CapiGroup* g = e->getGroup();
					if (g->getAllEstatesSameOwner()) {
						rent += rent;
					}
				}

				//second, calculate the rent my given math if needed. It will overwrite the normal-rent
				if (g->getRentMath() != "") {
					int dice = payR1+payR2;
					int groupown = 0;
					//Get the number of estates in the group, that is owned by owner
					for (int i = 0; i < g->getNumEstates(); i++)
						if (g->getEstate(i)->getOwner() == owner) groupown++;

					//Caluclate rent
					QString math = g->getRentMath();
					if (replaceMath != "") math = replaceMath;
					math = math.replace("${DICE}", QString::number(dice)).replace("${GROUPOWNED}", QString::number(groupown));
					QScriptEngine engine;
					rent = engine.evaluate(math).toInteger();
				}

				if (!(owner->getInJail() && jailNoRent)) {
					// playeronTurn has to pay the rent
					depts.append(new CapiDept(playerOnTurn, owner, 0, rent));
					wait = true;
					waitForDept();
				} else {
					vars.insert("%{player1}", playerOnTurn->getName());
					vars.insert("%{player2}", owner->getName());
					foreach (CapiPlayer* q, players) {
						CapiServerPlayer* pl = (CapiServerPlayer*) q;
						pl->send(createDisplayMsg(translator->translateMessage("%{player1} pays no rent, beacuse %{player2} is in jail.", pl->getLang(), &vars), e->getId()));
					}
				}
			} else {
				vars.insert("%{player}", playerOnTurn->getName());
				foreach (CapiPlayer* q, players) {
					CapiServerPlayer* pl = (CapiServerPlayer*) q;
					pl->send(createDisplayMsg(translator->translateMessage("%{player} pays no rent, because it is mortgaged.", pl->getLang(), &vars), e->getId()));
				}
			}
		}
	}
	replaceMath = "";

	//Handle takecard
	if (e->getTakeCard() != "") {
		qDebug() << "Needed cardgroup: " << e->getTakeCard();
		//Draw a card from stack
		//Find all cards that are not owned and are in the correct group
		QList<CapiCard*> cardList;
		for (int i = 0; i < cards.size(); i++) {
			CapiServerCard* d = (CapiServerCard*) cards.value(i);
			qDebug() << "Card #" << QString::number(d->getId()) << "is from group " << d->getGroup();
			if ((d->getGroup() == e->getTakeCard()) && (d->getOwner() == 0)) {
				cardList.append(d);
			}
		}

		if (cardList.size() == 0) {
			//We found to card
			foreach (CapiPlayer* q, players) {
				CapiServerPlayer* pl = (CapiServerPlayer*) q;
				pl->send(createDisplayMsg(translator->translateMessage("Found no card.", pl->getLang(), 0), e->getId()));
			}
		} else {
			//We found some cards. take one
			int cardNum = 0;
			if (shuffleCards) cardNum = (rand() % cardList.size());
			CapiServerCard* c = (CapiServerCard*) cardList.value(cardNum);

			//Bring it at the buttom from the stack
			cards.removeAll(c);
			cards.append(c);

			//Now handle the Card
			foreach (CapiPlayer* q, players) {
				CapiServerPlayer* pl = (CapiServerPlayer*) q;
				pl->send(createDisplayMsg(c->getText(pl->getLang()), e->getId()));
			}

			//Simple advance
			if (c->getAdvance() > 0) {
				int newPos = (playerOnTurn->getPos()+c->getAdvance())%estates.size();
				playerOnTurn->setPos(newPos);
				playerMoved = true;
			} else if (c->getAdvance() < 0) {
				int newPos = playerOnTurn->getPos()+c->getAdvance();
				while (newPos < 0) newPos += estates.size();
				playerOnTurn->setPos(newPos);
				playerMoved = true;
				directmove = true;
			}

			//Advance to
			if (c->getAdvanceTo() != -1) {
				playerOnTurn->setPos(c->getAdvanceTo());
				playerMoved = true;
			}

			//Advance to next of
			if (c->getAdvanceToNextOf() != "") {
				int pos = (playerOnTurn->getPos()+1)%estates.size();
				while (pos != playerOnTurn->getPos()) {
					if ((getEstate(pos)->getGroup() != 0) && (getEstate(pos)->getGroup()->getName() == c->getAdvanceToNextOf())) {
						break;
					}
					pos = (pos+1)%estates.size();
				}
				playerMoved = (pos != playerOnTurn->getPos());
				playerOnTurn->setPos(pos);
			}

			//Ownership
			if (c->getCanBeOwned()) {
				c->setOwner(playerOnTurn);
				sendToAll(c->getUpdate());
			}

			//Player has to pay
			if (c->getPay() > 0) {
				depts.append(new CapiDept(playerOnTurn, 0, e->getPayTarget(), c->getPay()));
				wait = true;
				waitForDept();
			}

			//Player gets money
			if (c->getPay() < 0) {
				playerOnTurn->setMoney(playerOnTurn->getMoney()-c->getPay());
				sendToAll(playerOnTurn->getUpdate("money"));
			}

			//Player has to pay to each player
			if (c->getPayEach() > 0) {
				foreach(CapiPlayer* q, players) {
					if ((q != playerOnTurn) && !q->getBankrupt()) {
						depts.append(new CapiDept(playerOnTurn, q, 0, c->getPayEach()));
					}
				}
				wait = true;
				waitForDept();
			}

			//Player gets money from each player
			if (c->getPayEach() < 0) {
				int amount = -1 * c->getPayEach();
				foreach(CapiPlayer* q, players) {
					if ((q != playerOnTurn) && !q->getBankrupt()) {
						depts.append(new CapiDept(q, playerOnTurn, 0, amount));
					}
				}
				wait = true;
				waitForDept();
			}

			//Player has to pay for his houses/hotels
			if ((c->getPayHotel() > 0) || (c->getPayHouse() > 0)) {
				int amount = 0;
				amount += c->getPayHouse() * getNumHouses(playerOnTurn);
				amount += c->getPayHotel() * getNumHotels(playerOnTurn);

				depts.append(new CapiDept(playerOnTurn, 0, e->getPayTarget(), amount));
				wait = true;
				waitForDept();
			}

			if (c->getRentMath() != "") {
				replaceMath = c->getRentMath();
			}

			//Roll if needed
			if (c->getRoll()) {
				roll(true);
			}

			//Jail player if needed
			if (c->getToJail()) {
				jailPlayer();
			}
		}
	}

	//Advance to, if owned (Property from atlantic)
	if ((e->getAdvanceToIfOwned() != -1) && e->getOwner() == playerOnTurn) {
		playerOnTurn->setPos(e->getAdvanceToIfOwned());
		playerMoved = true;
	}

	if (!wait) {
		movedPlayer();
	}
}

void CapiServerGame::movedPlayer() {
	if (depts.size() > 0) {
		qDebug() << "Bug: going on with non finished depts";
		return;
	}

	//After estate handling
	if (playerMoved) {
		waitForMove();
	} else {
		if (playerOnTurn->getInJail()) {
			endTurn();
		} else {
			if (!playerOnTurn->getBankrupt())
				wasDouble();
			else
				endTurn();
		}
	}
}

void CapiServerGame::endTurn() {
	if (playerOnTurn != 0) {
		//Turn endet. next player gets turn
		playerOnTurn->setHasTurn(false);
		sendToAll(playerOnTurn->getUpdate("hasturn"));
	}

	if (getNumPlayingPlayers() > 1) {
		//Begin of a new turn
		do {
			//Get a non bankrupt and non spectating player
			playerNum = (playerNum+1)%players.count();
			playerOnTurn = (CapiServerPlayer*) players.value(playerNum);
		} while (playerOnTurn->getBankrupt() || playerOnTurn->getSpectator());

		playerOnTurn->setHasTurn(true);
		playerOnTurn->resetDoubles();
		vars.insert("%{player}", playerOnTurn->getName());
		foreach (CapiPlayer* q, players) {
			CapiServerPlayer* pl = (CapiServerPlayer*) q;
			pl->send(createDisplayMsg(translator->translateMessage("Turn goes to %{player}.", pl->getLang(), &vars), -1));
		}
		if (playerOnTurn->getInJail()) {
			waitForJail();
		} else {
			waitForRoll();
		}
	} else {
		endGame();
	}
}

void CapiServerGame::endGame() {
	//Game ended
	turnState = -1; //Game over

	setStatus("end");
	sendToAll(getUpdate("status"));
	CapiServerPlayer* p = 0;
	foreach(CapiPlayer* q, players) {
		if (!q->getBankrupt()) p = (CapiServerPlayer*) q;
	}
	if (p == 0) {
		foreach (CapiPlayer* q, players) {
			CapiServerPlayer* pl = (CapiServerPlayer*) q;
			pl->send(createDisplayMsg(translator->translateMessage("No winner.", pl->getLang(), 0), -1));
		}
	} else {
		vars.insert("%{player}", p->getName());
		vars.insert("%{money}", QString::number(assetsForPlayer(p)));
		foreach (CapiPlayer* q, players) {
			CapiServerPlayer* pl = (CapiServerPlayer*) q;
			pl->send(createDisplayMsg(translator->translateMessage("%{player} wins with a furtune of %{money}.", pl->getLang(), &vars), -1, false, false, createButton(".gx", translator->translateMessage("Exit game", pl->getLang(), 0))));
		}

	}
}

void CapiServerGame::wasDouble() {
	if (r1 == r2) {
		turnState = 0;
		playerOnTurn->setCanRoll(true);
		vars.insert("%{player}", playerOnTurn->getName());
		foreach (CapiPlayer* q, players) {
			CapiServerPlayer* pl = (CapiServerPlayer*) q;
			pl->send(createDisplayMsg(translator->translateMessage("%{player} can roll again.", pl->getLang(), &vars), playerOnTurn->getPos()));
		}
		sendToAll(playerOnTurn->getUpdate("can_roll"));
	} else {
		endTurn();
	}
}

int CapiServerGame::assetsForPlayer(CapiPlayer* p) {
	int assets = p->getMoney();
	foreach (CapiEstate* e, estates) {
		if (e->getOwner() == p) {
			if (!e->getMortaged()) assets += e->getMortageVal();
			assets += e->getHouses()*e->getSellHousePrice();
		}
	}

	return assets;
}

void CapiServerGame::setCanBuyOrSellHouses(CapiServerGroup* g) {
	bool groupMortgaged = false;
	bool sameOwner = g->getAllEstatesSameOwner();
	int maxH = g->getEstate(0)->getHouses();
	int minH = g->getEstate(0)->getHouses();

	//Get the minimum and maximum number of houses on the group
	//noice if one estate is mortgaged
	for (int i = 0; i < g->getNumEstates(); i++) {
		CapiEstate* e = g->getEstate(i);
		if (e->getHouses() < minH) minH = e->getHouses();
		if (e->getHouses() > maxH) maxH = e->getHouses();
		if (e->getMortaged()) groupMortgaged = true;
	}

	//check for every estate in the group, if we can buy/sell houses
	for (int i = 0; i < g->getNumEstates(); i++) {
		CapiServerEstate* es = (CapiServerEstate*) g->getEstate(i);
		if (!sameOwner || groupMortgaged || (g->getHousePrice() == 0)) {
			es->setCanBuyHouse(false);
			es->setCanSellHouse(false);
		} else {
			es->setCanBuyHouse((es->getHouses() < 5) && (es->getHouses() == minH));
			es->setCanSellHouse((es->getHouses() > 0) && (es->getHouses() == maxH));
		}

		es->setCanToggleMortage(g->getNumHouses() == 0);
		sendToAll(es->getUpdate("can_buy_houses can_sell_houses can_toggle_mortgage"));
	}
}

//Gets the number of all houses owned by given player. if no player is given, it counts all houses
int CapiServerGame::getNumHouses(CapiPlayer* p) {
	bool countAll = (p == 0);
	int count = 0;

	foreach(CapiEstate* e, estates) {
		if (countAll || (e->getOwner() == p)) {
			if (e->getHouses() < 5)
				count += e->getHouses();
		}
	}

	return count;
}

//Gets the number of all hotels owned by given player. if no player is given, it counts all hotels
int CapiServerGame::getNumHotels(CapiPlayer* p) {
	bool countAll = (p == 0);
	int count = 0;

	foreach(CapiEstate* e, estates) {
		if (countAll || (e->getOwner()== p)) {
			if (e->getHouses() == 5) count += 1;
		}
	}

	return count;
}

//If a player has more depts then assets he is banrupt.
void CapiServerGame::bankruptPlayer(CapiDept* d) {
	CapiServerPlayer* p = (CapiServerPlayer*) d->getFromPlayer();
	p->setBankrupt(true);
	p->setInJail(false);
	CapiServerPlayer* tp = 0;

	if (d->getToPlayer() != 0) {
		tp = (CapiServerPlayer*) d->getToPlayer();
		tp->setMoney(tp->getMoney()+p->getMoney());
		sendToAll(tp->getUpdate("money"));
	}
	p->setMoney(0);

	QString update = p->getUpdate("bankrupt money jailed");
	if(tp != 0) update += tp->getUpdate("money");

	//All estate goes to the player that would get the money
	foreach (CapiEstate* f, estates) {
		CapiServerEstate* e = (CapiServerEstate*) f;
		if (e->getOwner() == p) {
			e->setOwner(tp);
			update += e->getUpdate("owner");
		}
	}

	//All cards goees to the player that would get the money
	foreach (CapiCard* d, cards) {
		CapiServerCard* c = (CapiServerCard*) d;
		if (c->getOwner() == p) {
			c->setOwner(tp);
			update += c->getUpdate();
		}
	}

	vars.insert("%{player}", p->getName());
	foreach (CapiPlayer* q, players) {
		CapiServerPlayer* pl = (CapiServerPlayer*) q;
		pl->send(createDisplayMsg(translator->translateMessage("%{player} is bankrupt.", pl->getLang(), &vars), playerOnTurn->getPos()));
	}

	sendToAll(update);
	d->setPayed(true);

	if (p->getId() == playerOnTurn->getId()) {
		//endTurn();
	}
}

int CapiServerGame::getNumPlayingPlayers() {
	int pl = 0;
	foreach(CapiPlayer* p, players)
		if (!p->getBankrupt() && !p->getSpectator()) pl++;

	return pl;
}
