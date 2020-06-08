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

#include "capigame.h"

#include "capiestate.h"
#include "capicard.h"

CapiGame::CapiGame(int i) : CapiTemplate() {
	status = "config";
	canBeJoined = true;
	numPlayer = 0;
	minPlayer = 0;
	maxPlayer = 0;
	master = 0;
	sellEnabled = false;
	setId(i);
}

CapiGame::~CapiGame() {
	while (estates.size() > 0) {
		CapiEstate* f = estates.value(0);
		estates.removeAll(f);
		delete f;
	}
	while (cards.size() > 0) {
		CapiCard* c = cards.value(0);
		cards.removeAll(c);
		delete c;
	}
	while (groups.size() > 0) {
		CapiGroup* g = groups.value(0);
		groups.removeAll(g);
		delete g;
	}

	while (players.size() > 0) {
		players.removeAll(players.value(0));
	}
}

//Returns the Estate with given id. If no such field exists, one wil be createed and returned
CapiEstate* CapiGame::getEstate(int id, bool create) {
	if (id < 0) return 0;
	foreach (CapiEstate* e, estates)
		if (e->getId() == id) return e;

	if (!create) return 0;

	CapiEstate* f = new CapiEstate(id);
	estates.append(f);
	return f;
}

//Returns the Estate with given id. If no such field exists, one wil be createed and returned
CapiGroup* CapiGame::getGroup(int id, QString name, bool create) {
	if (id < 0) return 0;
	foreach (CapiGroup* g, groups)
		if (g->getId() == id) return g;

	if (!create) return 0;

	CapiGroup* g = new CapiGroup(id, name);
	groups.append(g);
	return g;
}

//Returns the Player with given id. If no such Player exists 0 will be returned
CapiPlayer* CapiGame::getPlayer(int id, bool create) {
	if (id < 0) return 0;
	foreach (CapiPlayer* p, players)
		if (p->getId() == id) return p;

	if (!create) return 0;

	CapiPlayer* p = new CapiPlayer(id);
	players.append(p);
	return p;
}

CapiPlayer* CapiGame::getPlayerFromList(int num) {
	if (num < 0) return 0;
	if (num >= players.size()) return 0;
	return players.value(num);
}

bool CapiGame::hasPlayer(int id) {
	if (id == -1) return true;
	foreach (CapiPlayer* p, players)
		if (p->getId() == id) return true;
	return false;
}

CapiCard* CapiGame::getCard(int id, bool create) {
	foreach (CapiCard* c, cards) {
		if (c->getId() == id) return c;
	}
	if (!create) return 0;

	CapiCard* c = new CapiCard(id);
	cards.append(c);
	return c;
}

CapiCard* CapiGame::getCardFromList(int i) {
	if (i < 0) return 0;
	if (i >= cards.size()) return 0;
	return cards.value(i);
}

QString CapiGame::getStatus() {
	return status;
}

bool CapiGame::getCanBeJoined() {
	return canBeJoined;
}

int CapiGame::getNumPlayer() {
	return numPlayer;
}

int CapiGame::getMinPlayer() {
	return minPlayer;
}

int CapiGame::getMaxPlayer() {
	return maxPlayer;
}

CapiPlayer* CapiGame::getMaster() {
	return master;
}

bool CapiGame::getSellEnabled() {
	return sellEnabled;
}

int CapiGame::getNumEstates() {
	return estates.size();
}

int CapiGame::getNumCards() {
	return cards.size();
}

void CapiGame::addPlayer(CapiPlayer* p) {
	if (p == 0) return;
	players.append(p);
}

void CapiGame::removePlayer(CapiPlayer* p) {
	if (p == 0) return;
	players.removeAll(p);
}

void CapiGame::setStatus(QString s) {
	status = s;
}

void CapiGame::setCanBeJoined(bool cbj) {
	canBeJoined = cbj;
}

void CapiGame::setNumPlayer(int np) {
	numPlayer = np;
}

void CapiGame::setMinPlayer(int mp) {
	minPlayer = mp;
}

void CapiGame::setMaxPlayer(int mp) {
	maxPlayer = mp;
}

void CapiGame::setMaster(CapiPlayer* m) {
	master = m;
}

void CapiGame::setSellEnabled(int se) {
	sellEnabled = se;
}

