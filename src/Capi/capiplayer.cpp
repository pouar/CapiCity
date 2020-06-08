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

#include <stdlib.h>
#include <time.h>

#include "capiplayer.h"

CapiPlayer::CapiPlayer(int i) : CapiObject(i) {
	game = 0;
	avatar = "bell.png";
	money = 0;
	pos = 0;
	inJail = false;
	hasTurn = false;
	canRoll = false;
	canBuyEstate = false;
	spectator = false;
	hasDept = false;
	canAuction = false;
	canUseCard = false;
}

CapiPlayer::~CapiPlayer() {
}

CapiGame* CapiPlayer::getGame() {
	return game;
}

QString CapiPlayer::getAvatar() {
	return avatar;
}

int CapiPlayer::getMoney() {
	return money;
}

int CapiPlayer::getPos() {
	return pos;
}

bool CapiPlayer::getInJail() {
	return inJail;
}

bool CapiPlayer::getHasTurn() {
	return hasTurn;
}

bool CapiPlayer::getCanRoll() {
	return canRoll;
}

bool CapiPlayer::getCanBuyEstate() {
	return canBuyEstate;
}

bool CapiPlayer::getSpectator() {
	return spectator;
}

bool CapiPlayer::getHasDept() {
	return hasDept;
}

bool CapiPlayer::getCanAuction() {
	return canAuction;
}

bool CapiPlayer::getCanUseCard() {
	return canUseCard;
}

bool CapiPlayer::getBankrupt() {
	return bankrupt;
}

bool  CapiPlayer::getDirectmove() {
	return directmove;
}

void CapiPlayer::setGame(CapiGame* g) {
	game = g;
}

void CapiPlayer::setAvatar(QString a) {
	avatar = a;
}

void CapiPlayer::setMoney(int m) {
	money = m;
}

void CapiPlayer::setPos(int p) {
	pos = p;
}

void CapiPlayer::setInJail(bool j) {
	inJail = j;
}

void CapiPlayer::setHasTurn(bool ht) {
	hasTurn = ht;
}

void CapiPlayer::setCanRoll(bool cr) {
	canRoll = cr;
}

void CapiPlayer::setCanBuyEstate(bool cbe) {
	canBuyEstate = cbe;
}

void CapiPlayer::setSpectator(bool s) {
	spectator = s;
}

void CapiPlayer::setHasDept(bool hd) {
	hasDept = hd;
}

void CapiPlayer::setCanAuction(bool ca) {
	canAuction = ca;
}

void CapiPlayer::setCanUseCard(bool cuc) {
	canUseCard = cuc;
}

void CapiPlayer::setBankrupt(bool b) {
	bankrupt = b;
}

void CapiPlayer::setDirectmove(bool dm) {
	directmove = dm;
}
