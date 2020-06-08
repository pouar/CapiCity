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

#include <QMessageBox>
#include <QDateTime>

#include "src/Capi/capigame.h"
#include "src/Capic/capiclientestate.h"
#include "capiclient.h"
#include "src/defines.h"

CapiClient::CapiClient(QWidget *parent) : QObject(parent) {

	soket = new QTcpSocket(this);

	connect(soket, SIGNAL(readyRead()), this, SLOT(readData()));
	connect(soket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(showError(QAbstractSocket::SocketError)));

	curGameId = -1;

	rolled = true;
	capid = false;

}

//Returns a Player. If no one is found, onbe will be created
CapiClientPlayer* CapiClient::getPlayer(int pid) {
	if (pid == -1) return 0;
	CapiClientPlayer* p = 0;
	for (int i = 0; i < players.size(); i++)
		if (players.value(i)->getId() == pid) return players.value(i);

	p = new CapiClientPlayer(pid);
	players.append(p);
	return p;
}

CapiClientPlayer* CapiClient::getPlayerFromList(int i) {
	if (i < 0) return 0;
	if (i >= players.size()) return 0;
	return players.value(i);
}

bool CapiClient::hasPlayer(int pid) {
	for (int i = 0; i < players.size(); i++)
		if (players.value(i)->getId() == pid) return true;
	return false;
}

//Returns a Game template. If no one is found, onbe will be created
CapiTemplate* CapiClient::getTemplate(QString t) {
	CapiTemplate* temp = 0;
	for (int i = 0; i < templates.size(); i++)
		if (templates.value(i)->getType() == t)
			return templates.value(i);
	temp = new CapiTemplate();
	temp->setType(t);
	templates.append(temp);
	return temp;
}

CapiTemplate* CapiClient::getTemplateFromList(int n) {
	if (n < 0) return 0;
	if (n >= templates.size()) return 0;
	return templates.value(n);
}

//Returns a Game. If no one is found, onbe will be created if type is given or created is true, otherwise 0 is returned
CapiClientGame* CapiClient::getGame(int gid) {
	if (gid == -1) return 0;
	CapiClientGame* g = 0;
	for (int i = 0; i < games.size(); i++)
		if (games.value(i)->getId() == gid) return games.value(i);

	g = new CapiClientGame(gid);
	games.append(g);
	return g;
}

CapiClientGame* CapiClient::getGameFromList(int n) {
	if (n < 0) return 0;
	if (n >= games.size()) return 0;
	return games.value(n);
}

int CapiClient::getNumPlayers() {
	return players.size();
}

int CapiClient::getNumTemplates() {
	return templates.size();
}

int CapiClient::getNumGames() {
	return games.size();
}

int CapiClient::getCurGameId() {
	return curGameId;
}

int CapiClient::getClientPlayerId() {
	return clientPlayerId;
}

QString CapiClient::getClientCookie() {
	return clientCookie;
}

//Deletes the player. It will be removed from all games
void CapiClient::deletePlayer(CapiClientPlayer* p) {
	if (p == 0) return;
	for (int i = 0; i < games.size(); i++)
		games.value(i)->removePlayer(p);
	players.removeAll(p);
	delete p;
}

void CapiClient::deleteGame(CapiClientGame* g) {
	if (g == 0) return;
	foreach (CapiClientPlayer* p, players)
		if (p->getGame() == g) p->setGame(0);
	games.removeAll(g);
	delete g;
}

void CapiClient::setCurGameId(int id) {
	curGameId = id;
}

void CapiClient::setClientPlayerId(int id) {
	clientPlayerId = id;
}

void CapiClient::setClientCookie(QString c) {
	clientCookie = c;
}

void CapiClient::leaveGame() {
	if ((getPlayer(this->clientPlayerId) != 0) && (this->getPlayer(this->clientPlayerId)->getGame() != 0))
		send(".gx");
	curGameId = -1;
}

void CapiClient::reconnect() {
	reconnected = false;
	host = s.value(CONFIG_CONNECTION_SERVER, QVariant("")).toString();

	if (host != "") {
		//Auto reconnect
		clientCookie = s.value(CONFIG_CONNECTION_COOKIE, QVariant("")).toString();
		int port = s.value(CONFIG_CONNECTION_PORT, QVariant(0)).toInt();
		connectToHost(host, port);
	}
}

void CapiClient::setAutoroll(bool ar) {
	s.setValue(CONFIG_GAME_AUTOROLL, ar);
}

bool CapiClient::getCapid() {
	return capid;
}

void CapiClient::parseDoc(QDomDocument d) {
	QDomElement root = d.documentElement();
	if (root.tagName() == "monopd") {

		//save the last time, we got a message by server
		s.setValue(CONFIG_CONNECTION_LASTMESSAGE, QVariant(QDateTime::currentDateTime().toTime_t()));
		bool update = false;
		bool gameListUpdated = false;
		QDomElement e = root.firstChildElement();
		while (!e.isNull()) {
			QString name = e.nodeName();
			if (name == "estateupdate") {
				parseEstate(e);
				update = true;
			}
			if (name == "server") {
				QString version = e.attribute("version", "0.0.0");
				capid = false;
				if (version.startsWith("1.0")) {
					capid = true;
				} else if (version != "0.9.3")
					QMessageBox::warning(0, tr("Wrong server Version"), tr("Capi City is only compatible to Monopd 0.9.3 or Capid. Perhaps the gaming experience is disturbed a bit."));
				send(".n"+s.value(CONFIG_PLAYER_NAME, CONFIG_PLAYER_NAME_DEFAULT).toString());
				QString image = s.value(CONFIG_PLAYER_IMAGE, CONFIG_PLAYER_IMAGE_DEFAULT).toString();
				send(".pi"+image);
				send(".l"+QLocale::system().name());
			}
			if (name == "cardupdate") {
				parseCard(e);
			}
			if (name == "estategroupupdate") {
				parseGroup(e);
			}
			if (name == "tradeupdate") {
				parseTrade(e);
			}
			if (name == "playerupdate") {
				parsePlayer(e);
			}
			if (name == "msg") {
				parseMsg(e);
			}
			if (name == "deleteplayer") {
				bool ok;
				int pid = e.attribute("playerid", "").toInt(&ok);
				if (ok) deletePlayer(getPlayer(pid));
			}
			if (name == "deletegame") {
				bool ok;
				int gid = e.attribute("gameid", "").toInt(&ok);
				if (ok) {
					if (gid == curGameId) {
						curGameId = -1;
						emit(curGameChanged());
					}
					deleteGame(getGame(gid));
				}
				gameListUpdated = true;
			}
			if (name == "client") {
				bool ok;
				int pid = e.attribute("playerid", "").toInt(&ok);
				if (ok) {
					clientPlayerId = pid;
				}

				if (!reconnected && (host == soket->peerName())) {
					//Here we reconnected
						send(".R"+clientCookie);
						reconnected = true;
				}
				host = soket->peerName();
				clientCookie = e.attribute("cookie", "");
				s.setValue(CONFIG_CONNECTION_SERVER, QVariant(host));
				s.setValue(CONFIG_CONNECTION_COOKIE, QVariant(clientCookie));
				s.setValue(CONFIG_CONNECTION_PORT, QVariant(soket->peerPort()));
				emit(gotHost(host));
			}
			if (name == "gameupdate") {
				parseGame(e);
				gameListUpdated = true;
			}
			if (name == "updategamelist") {
				QString type = e.attribute("type", "");
				//Only implementet for compatiblity with monopd 0.9.3
				QDomElement gu = e.firstChildElement("game");
				while (!gu.isNull()) {
					parseGamelistUpdate(gu, type);
					gu = e.nextSiblingElement("game");
				}
				gameListUpdated = true;
			}
			if (name == "configupdate") {
				parseConfig(e);
			}
			if (name == "display") {
				parseDisplay(e);
			}
			if (name == "auctionupdate") {
				parseAuction(e);
			}
			e = e.nextSiblingElement();
		}
		if (update) emit(boardUpdate());
		if (gameListUpdated) emit(newGameList());


		//Set visibility of all known players
		for (int i = 0; i < players.size(); i++) {
			CapiClientPlayer* p = players.value(i);
			if (p->getGame() == 0)
				p->setPlayerWidgetVisible(curGameId == -1);
			else
				p->setPlayerWidgetVisible(curGameId == p->getGame()->getId());
		}

		if (curGameId == -1) emit(newGameList());
	}
	bool autoroll = s.value(CONFIG_GAME_AUTOROLL, CONFIG_GAME_AUTOROLL_DEFAULT).toBool();
	if (autoroll && !rolled && hasPlayer(clientPlayerId) && getPlayer(this->clientPlayerId)->getCanRoll()) {
		rolled = true;
		send(".r");
	}
}

void CapiClient::parseMsg(QDomElement m) {
	QString type = m.attribute("type", "NaN");
	QString msg = m.attribute("value", "");
	QString val;
	bool ok;
	if (type == "NaN") return;
	if (type == "chat") {
		//a simple chat message
		val = m.attribute("playerid", "");
		if (val == "") return;
		int pid = val.toInt(&ok);
		if (!ok) return;
		emit(chatMessage(pid, msg));
	} else {
		emit(serverMessage(type, msg));
	}
}

void CapiClient::parseDisplay(QDomElement d) {
	QString val;
	bool ok;
	int fid;

	val = d.attribute("estateid", "NaN");
	fid = val.toInt(&ok);
	if (!ok) return;

	//Check EstateID
	//if (fid == -1) emit(clearedText());

	//Check if text should be cleared
	//val = d.attribute("cleartext", "");
	//if (val == "1") {
	//	emit(clearedText());
	//}

	//Check if buttons should be cleared
	val = d.attribute("clearbuttons", "");
	if (val == "1") {
		emit(clearedButtons());
	}

	val = d.attribute("text", "");
	if (val != "") emit(gotText(val));

	emit(gotEstate(fid));

	//Parse Buttons
	QDomElement b = d.firstChildElement("button");
	while (!b.isNull()) {
		bool enabled = false;
		QString text;
		QString cmd;
		val = b.attribute("enabled", "");
		if (val == "1") enabled = true;

		text = b.attribute("caption", "");

		cmd = b.attribute("command", "");

		if (enabled && (cmd != "")) {
			//if (!cmd.startsWith(".e")) {
				emit(gotButton(text, cmd));
			//}
		}

		b = b.nextSiblingElement("button");
	}

}

void CapiClient::parseGamelistUpdate(QDomElement g, QString type) {
	QString val;
	int gameId;
	bool ok;

	gameId = g.attribute("id", "NaN").toInt(&ok);
	if (!ok) return;

	if ((type == "edit") || (type == "add") || (type == "full" )) {
		if (gameId == -1) {
			CapiTemplate* temp = getTemplate(g.attribute("gametype", ""));
			if (temp == 0) return;

			temp->setDesc(g.attribute("description", ""));

			temp->setName(g.attribute("name", ""));
		} else {
			CapiClientGame* game = getGame(gameId);
			game->setType(g.attribute("gametype", ""));

			int players = g.attribute("players", "").toInt(&ok);
			if (ok) game->setNumPlayer(players);

			val = g.attribute("description", "");
			if (val != "") game->setDesc(val);

			val = g.attribute("name", "");
			if (val != "") game->setName(val);

			bool cbj = false;
			if (g.attribute("canbejoined", "") == "1") cbj = true;
			game->setCanBeJoined(cbj);

			game->setListed(true);
		}
	} else if (type == "del") {
		CapiClientGame* game = getGame(gameId);
		if (game == 0) return;
		game->setListed(false);
	}
}

void CapiClient::parsePlayer(QDomElement f) {
	QString val;
	int id;
	bool ok;

	//Get the player that has changed attributes
	val = f.attribute("playerid", "NaN");
	if (val == "NaN") return;
	id = val.toInt(&ok);
	if (!ok) return;
	if (id < 0) return;
	bool toAdd = !hasPlayer(id);
	CapiClientPlayer* p = getPlayer(id);
	if (toAdd) emit(playerAdded(p));

	int newPos = p->getPos();
	//Now parse all (other) attributes
	QDomNamedNodeMap map = f.attributes();
	for (int i = 0; i < map.size(); i++) {
		QDomNode a = map.item(i);

		if (a.nodeName() == "game") {
			int gid = a.nodeValue().toInt(&ok);

			if (ok) {
				CapiClientGame* g = getGame(gid);
				if (p->getId() == clientPlayerId) {
					int oldGid = curGameId;
					curGameId = gid;
					if (oldGid != gid) emit(curGameChanged());
				}
				if (p->getGame() != g) {
					if (p->getGame() != 0) {
						CapiClientGame* game = (CapiClientGame*) p->getGame();
						if (game != 0) game->removePlayer(p);
					}
					if (g != 0) {
						g->addPlayer(p);
					}
				}
				p->setGame(g);
			}
		}
		else if (a.nodeName() == "name") p->setName(a.nodeValue());
		else if (a.nodeName() == "image") {
			p->setAvatar(a.nodeValue());
			emit(boardUpdate());
		} else if (a.nodeName() == "money") p->setMoney(a.nodeValue().toInt());
		else if (a.nodeName() == "location") {
			int pos = a.nodeValue().toInt(&ok);
			if (ok) {
				newPos = pos;
			}
		} else if (a.nodeName() == "jailed") {
			p->setInJail(a.nodeValue() == "1");
			emit(boardUpdate());
		} else if (a.nodeName() == "directmove") p->setDirectmove(a.nodeValue() == "1");
		else if (a.nodeName() == "bankrupt") p->setBankrupt(a.nodeValue() == "1");
		else if (a.nodeName() == "hasturn") p->setHasTurn(a.nodeValue() == "1");
		else if (a.nodeName() == "can_roll") {
			p->setCanRoll(a.nodeValue() == "1");
			rolled = !p->getCanRoll();
		} else if (a.nodeName() == "can_buyestate") p->setCanBuyEstate(a.nodeValue() == "1");
		//else if (a.nodeName() == "host") p->setHost(a.nodeValue() == "1");
		else if (a.nodeName() == "spectator") p->setSpectator(a.nodeValue() == "1");
		else if (a.nodeName() == "canauction") p->setCanAuction(a.nodeValue() == "1");
		else if (a.nodeName() == "canusecard") p->setCanUseCard(a.nodeValue() == "1");
		else if (a.nodeName() == "playerid") p->getId();
		else
			qDebug() << "Unknown Player Attribute: "+a.nodeName() + " = " + a.nodeValue();
	}

	if (p->getId() == clientPlayerId) emit(playerUpdated());

	if (newPos != p->getPos()) {
		emitPlayerMoved(p, newPos);
	}
}

void CapiClient::parseEstate(QDomElement f) {
	QString val;
	int id;
	bool ok;

	//Get the estate
	val = f.attribute("estateid", "NaN");
	if (val == "NaN") return;
	id = val.toInt(&ok);
	if (!ok) return;
	if (id < 0) return;
	if (curGameId == -1) return;
	CapiClientEstate* est = getGame(curGameId)->getEstate(id);

	//Now parse all (other) attributes
	QDomNamedNodeMap map = f.attributes();
	for (int i = 0; i < map.size(); i++) {
		QDomNode a = map.item(i);

		if (a.nodeName() == "color") est->setColor(a.nodeValue());
		else if (a.nodeName() == "bgcolor") est->setBgColor(a.nodeValue());
		else if (a.nodeName() == "owner") est->setOwner(getPlayer(a.nodeValue().toInt()));
		else if (a.nodeName() == "houseprice") est->setHousePrice(a.nodeValue().toInt());
		else if (a.nodeName() == "group") {
			CapiGroup* g = getGame(curGameId)->getGroup(a.nodeValue().toInt());
			est->setGroup(g);
			if (g != 0) g->addEstate(est);
		} else if (a.nodeName() == "can_be_owned") est->setBuyable(a.nodeValue() == "1");
		else if (a.nodeName() == "can_toggle_mortgage") est->setCanToggleMortage(a.nodeValue() == "1");
		else if (a.nodeName() == "can_buy_houses") est->setCanBuyHouse(a.nodeValue() == "1");
		else if (a.nodeName() == "can_sell_houses") est->setCanSellHouse(a.nodeValue() == "1");
		else if (a.nodeName() == "price") est->setPrice(a.nodeValue().toInt());
		else if (a.nodeName() == "rent0") est->setRent(0, a.nodeValue().toInt());
		else if (a.nodeName() == "rent1") est->setRent(1, a.nodeValue().toInt());
		else if (a.nodeName() == "rent2") est->setRent(2, a.nodeValue().toInt());
		else if (a.nodeName() == "rent3") est->setRent(3, a.nodeValue().toInt());
		else if (a.nodeName() == "rent4") est->setRent(4, a.nodeValue().toInt());
		else if (a.nodeName() == "rent5") est->setRent(5, a.nodeValue().toInt());
		else if (a.nodeName() == "name") est->setName(a.nodeValue());
		else if (a.nodeName() == "icon") est->setPicture(a.nodeValue());
		else if (a.nodeName() == "houses") est->setHouses(a.nodeValue().toInt());
		else if (a.nodeName() == "money") est->setMoney(a.nodeValue().toInt());
		else if (a.nodeName() == "mortgageprice") est->setMortageVal(a.nodeValue().toInt());
		else if (a.nodeName() == "unmortgageprice") est->setUnmortageVal(a.nodeValue().toInt());
		else if (a.nodeName() == "sellhouseprice") est->setSellHousePrice(a.nodeValue().toInt());
		else if (a.nodeName() == "mortgaged") est->setMortaged(a.nodeValue() == "1");
		else if (a.nodeName() == "estateid") est->getId();
		else
			qDebug() << "Unknown Estate Attribute: "+a.nodeName() + " = " + a.nodeValue();
	}
	updateEstateLabels();
}

void CapiClient::parseCard(QDomElement c) {
	QString val;
	bool ok;

	//Get the card
	int id;
	val = c.attribute("cardid", "NaN");
	if (val == "NaN") return;
	id = val.toInt(&ok);
	if (!ok) return;
	if (curGameId == -1) return;
	CapiCard* card = getGame(curGameId)->getCard(id);

	QDomNamedNodeMap map = c.attributes();
	for (int i = 0; i < map.size(); i++) {
		QDomNode a = map.item(i);
		if (a.nodeName() == "title") card->setTitle(a.nodeValue());
		else if (a.nodeName() == "owner") card->setOwner(getPlayer(a.nodeValue().toInt()));
		else if (a.nodeName() == "cardid") card->getId();
		else
			qDebug() << "Unknown Card attribute: "+a.nodeName()+" = "+a.nodeValue();
	}

	//Get title
	if (card->getTitle() == "Noname") card->setTitle(tr("Get out of jail"));
}

void CapiClient::parseGroup(QDomElement g) {
	QString val;
	bool ok;

	int id;
	val = g.attribute("groupid", "NaN");
	if (val == "NaN") return;
	id = val.toInt(&ok);
	if (!ok) return;
	CapiGroup* gr = getGame(curGameId)->getGroup(id, "", true);
	QDomNamedNodeMap map = g.attributes();
	for (int i = 0; i < map.size(); i++) {
		QDomNode a = map.item(i);
		if (a.nodeName() == "name") gr->setName(a.nodeValue());
		else if (a.nodeName() == "groupid") gr->getId();
		else
			qDebug() << "Unknown Group attribute: "+a.nodeName()+" = "+a.nodeValue();
	}
}

void CapiClient::parseTrade(QDomElement t) {
	QString val;
	bool ok;

	QString type = "";
	int tradeId = 0;
	int actorId = -1;
	int rev = 0;

	//Get data about the trade
	type = t.attribute("type", "edit");

	tradeId = t.attribute("tradeid", "NaN").toInt(&ok);
	if (!ok) return;

	actorId = t.attribute("actor", "NaN").toInt(&ok);
	if (!ok) actorId = -1;

	rev = t.attribute("revision", "NaN").toInt(&ok);
	if (!ok) rev = 0;

	emit(tradeUpdate(type, tradeId, actorId, rev));

	//Parse now all items
	QDomElement ti = t.firstChildElement();
	while (!ti.isNull()) {
		QString name = ti.nodeName();
		if (name == "tradeplayer") {
			int playerId = ti.attribute("playerid", "NaN").toInt(&ok);
			if (!ok) continue;

			bool accept = false;
			if (ti.attribute("accept", "0") == "1") accept = true;

			emit(tradePlayerUpdate(tradeId, playerId, accept));
		}
		if (name == "tradeestate") {
			int targetPlayerId = ti.attribute("targetplayer", "NaN").toInt(&ok);
			if (!ok) continue;

			int estateId = ti.attribute("estateid", "NaN").toInt(&ok);
			if (!ok) continue;

			emit(tradeEstateUpdate(tradeId, estateId, targetPlayerId));
		}
		if (name == "tradecard") {
			int targetPlayerId = ti.attribute("targetplayer", "NaN").toInt(&ok);
			if (!ok) continue;

			int cardId = ti.attribute("cardid", "NaN").toInt(&ok);
			if (!ok) continue;

			emit(tradeCardUpdate(tradeId, cardId, targetPlayerId));
		}
		if (name == "trademoney") {
			int fromPlayerId = ti.attribute("playerfrom", "NaN").toInt(&ok);
			if (!ok) continue;

			int targetPlayerId = ti.attribute("playerto", "NaN").toInt(&ok);
			if (!ok) continue;

			int money = ti.attribute("money", "NaN").toInt(&ok);
			if (!ok) continue;

			emit(tradeMoneyUpdate(tradeId, fromPlayerId, money, targetPlayerId));
		}
		ti = ti.nextSiblingElement();
	}

}

void CapiClient::parseAuction(QDomElement a) {
	QString val;
	bool ok;

	//Check, if a auction has started
	int actor = -1;
	val = a.attribute("actor", "");
	if (val != "") {
		actor = val.toInt(&ok);
		if (ok && (actor > -1)) emit(auctionStarted());
	}

	int auctionid;
	val = a.attribute("auctionid", "");
	if (val != "") {
		auctionid = val.toInt(&ok);
		if (ok) emit(auctionIdUpdate(auctionid));
	}

	int fieldid;
	val = a.attribute("estateid", "");
	if (val != "") {
		fieldid = val.toInt(&ok);
		if (ok) emit(auctionEstateUpdate(fieldid));
	}

	int highbid;
	int highbidder;
	val = a.attribute("highbid", "");
	QString val2 = a.attribute("highbidder", "");
	if ((val != "") && (val2 != "")) {
		bool ok2;
		highbidder = val2.toInt(&ok2);
		highbid = val.toInt(&ok);
		if (ok && ok2) emit(auctionBidUpdate(highbidder, highbid));
	}

	int status;
	val = a.attribute("status", "");
	if (val != "") {
		status = val.toInt(&ok);
		if (ok) emit(auctionStatusUpdate(status));
	}
}

void CapiClient::parseGame(QDomElement g) {
	bool ok;
	QString val = g.attribute("gameid", "NaN");
	if (val == "NaN") return;
	int gameId = val.toInt(&ok);
	if (!ok) return;

	if (gameId == -1) {
		CapiTemplate* temp = getTemplate(g.attribute("gametype", ""));
		temp->setName(g.attribute("name", ""));
		temp->setDesc(g.attribute("description", ""));
	} else {
		//Get Game
		CapiClientGame* game = getGame(gameId);

		//Parse Attributes
		QDomNamedNodeMap map = g.attributes();
		for (int i = 0; i < map.size(); i++) {
			QDomNode a =map.item(i);
			if (a.nodeName() == "gametype") game->setType(a.nodeValue());
			else if (a.nodeName() == "name") game->setName(a.nodeValue());
			else if (a.nodeName() == "description") game->setDesc(a.nodeValue());
			else if (a.nodeName() == "status") {
				game->setStatus(a.nodeValue());
				if (gameId == curGameId) emit(statusChanged(game->getStatus()));
				if (game->getStatus() == "end") game->setListed(false);
			} else if (a.nodeName() == "players") game->setNumPlayer(a.nodeValue().toInt());
			else if (a.nodeName() == "canbejoined") {
				game->setCanBeJoined(a.nodeValue() == "1");
				game->setListed(game->getCanBeJoined());
			} else if (a.nodeName() == "minplayers") game->setMinPlayer(a.nodeValue().toInt());
			else if (a.nodeName() == "maxplayers") game->setMaxPlayer(a.nodeValue().toInt());
			else if (a.nodeName() == "master") {
				game->setMaster(getPlayer(a.nodeValue().toInt()));
				if (game->getMaster() != 0)
					emit(gameMasterChanged(gameId, game->getMaster()->getId()));
				else
					emit(gameMasterChanged(gameId, -1));
			} else if (a.nodeName() == "allowestatesales") game->setSellEnabled(a.nodeValue() == "1");
			else if (a.nodeName() == "gameid") game->setId(a.nodeValue().toInt());
			else
				qDebug() << "Unknown Game Attribute: "+a.nodeName() + " = " + a.nodeValue();
		}
	}
}

void CapiClient::parseConfig(QDomElement c) {
	QString val;
	bool ok;
	val = c.attribute("gameid", "NaN");
	if (val != "NaN") {
		val.toInt(&ok);
		if (ok) {
			//We have old config updates. We have to parse them
			QDomElement e = c.firstChildElement("option");
			while (!e.isNull()) {
				parseConfigOption(e);
				e = e.nextSiblingElement("option");
			}
			return;
		}
	}

	//Here we have the new config updates
	QString cfgId;
	QString text;
	QString type;

	type = c.attribute("type", "");

	cfgId = c.attribute("configid", "");
	if (cfgId == "") return;

	text = c.attribute("description", "");

	val = c.attribute("value", "");

	emit(configUpdate(".gc"+cfgId+":", text, type, val));
}

//This method parses the old config updates
void CapiClient::parseConfigOption(QDomElement o) {
	//title="Free Parking collects fines" command=".gef" value="0" edit="1"
	QString val;

	QString cmd;
	QString title;

	cmd = o.attribute("command", "");
	if (cmd == "") return;

	title = o.attribute("title", "");

	val = o.attribute("value", "");

	emit(configUpdate(cmd, title, "bool", val));
}

//Updates the Estate label for the players
void CapiClient::updateEstateLabels() {
	CapiClientGame* g = getGame(curGameId);

	foreach (CapiClientPlayer* p, players) {
		if (p->getGame() == g) {
			QString txt = "";
			for(int gn = 0; gn < g->getNumGroups(); gn++) {
				CapiGroup* gr = g->getGroupFromList(gn);
				for (int en = 0; en < gr->getNumEstates(); en++) {
					QString color = "#808080";
					CapiEstate* e = gr->getEstate(en);
					if (e->getOwner() == p) color = e->getColor();
					if (color == "") color = "#000000";
					txt += QString("<span style=\"background:%1\">&nbsp;</span>").arg(color);
				}
				if (gn < g->getNumGroups()-1) txt += "&nbsp;";
			}
			p->playerWidget->setEstateText(txt);
		}
	}
}

void CapiClient::emitPlayerMoved(CapiClientPlayer *p, int oldPos) {
	if (p->getDirectmove() || !s.value(CONFIG_GAME_ANIMATE, CONFIG_GAME_ANIMATE_DEFAULT).toBool()) {
		p->setPos(oldPos);
		send(".t"+QString::number(p->getPos()));
		emit(boardUpdate());
	} else {
		emit(playerMoved(p, oldPos));
	}
}

void CapiClient::readData() {
	while (soket->canReadLine()) {
		QByteArray bytes = soket->readLine();
		int size = bytes.size();
		QString xml = QString::fromUtf8(bytes.data(), size);
		if (xml.startsWith("<monopd>")) cache = xml;
		else cache.append(xml);

		if (xml.endsWith("</monopd>\n")) {
			QDomDocument d;
			d.setContent(cache);
			parseDoc(d);
		}
	}
}

void CapiClient::showError(QAbstractSocket::SocketError e) {
	//QMessageBox::critical(0, "Error", soket->errorString());

	if (((e == QAbstractSocket::RemoteHostClosedError) ||
		 (e == QAbstractSocket::NetworkError)) && (this->getPlayer(this->clientPlayerId) != 0) && (this->getPlayer(this->clientPlayerId)->getGame() != 0) && (s.value(CONFIG_CONNECTION_SERVER, "") != "")) {
		qDebug() << "trying to reconnect...";
		soket->readAll();
		disconnect(soket, SIGNAL(readyRead()), this, SLOT(readData()));
		disconnect(soket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(showError(QAbstractSocket::SocketError)));
		soket->deleteLater();
		soket = new QTcpSocket(this);
		connect(soket, SIGNAL(readyRead()), this, SLOT(readData()));
		connect(soket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(showError(QAbstractSocket::SocketError)));
		reconnect();
	} else {
		qDebug() << "Disconnected";
		emit(connectionLost());
	}
}

void CapiClient::connectToHost(QString host, int port) {
	soket->connectToHost(host, port);
	soket->waitForConnected(2000);

	if (soket->state() == QAbstractSocket::ConnectedState) {
		emit(connected());
	}
}

void CapiClient::disconnectFromHost(bool deleteData) {
	if (deleteData) {
		//Disconnected sucessfully, remove the data, since no need to reconnect from now on
		s.remove(CONFIG_CONNECTION_SERVER);
		s.remove(CONFIG_CONNECTION_COOKIE);
		s.remove(CONFIG_CONNECTION_PORT);
		s.remove(CONFIG_CONNECTION_LASTMESSAGE);
		host = "";
	}

	send(".d");
	soket->disconnectFromHost();

	//delte all games, templates and players
	while (games.size() > 0) {
		CapiClientGame* g = games.value(0);
		games.removeAll(g);
		delete g;
	}
	while (templates.size() > 0) {
		CapiTemplate* t = templates.value(0);
		templates.removeAll(t);
		delete t;
	}
	while (players.size() > 0) {
		CapiClientPlayer* p = players.value(0);
		players.removeAll(p);
		delete p;
	}

	this->curGameId = -1;
	this->clientPlayerId = -1;

	emit(gotHost(""));
}

void CapiClient::send(QString msg) {
	if (soket->state() != QAbstractSocket::ConnectedState) return;

	soket->write(msg.toUtf8()+"\n");
	soket->flush();
}
