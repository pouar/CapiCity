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

#include "capiestate.h"

CapiEstate::CapiEstate(int i) : CapiObject(i) {
        picture = "";
        money = 0;
        buyable=false;
        price = 0;
        owner = 0;
        mortageVal = 0;
        unmortageVal = 0;
        mortaged=false;
        canToggleMortage=false;
        for (int i = 0; i < 6; i++) rent[i] = 0;
        group = 0;
        color = "";
        bgColor = "";
        housePrice = 0;
        sellHousePrice = 0;
        houses = 0;
        canBuyHouse = false;
        canSellHouse = false;
        rent[0] = 0;
}

CapiEstate::~CapiEstate() {
}

QString CapiEstate::getPicture() {
        return picture;
}

int CapiEstate::getMoney() {
        return money;
}

bool CapiEstate::getBuyable() {
        return buyable;
}

int CapiEstate::getPrice() {
        return price;
}

CapiPlayer* CapiEstate::getOwner() {
        return owner;
}

int CapiEstate::getMortageVal() {
        return mortageVal;
}

int CapiEstate::getUnmortageVal() {
        return unmortageVal;
}

bool CapiEstate::getMortaged() {
        return mortaged;
}

bool CapiEstate::getCanToggleMortage() {
        return canToggleMortage;
}

int CapiEstate::getRent(int i) {
        if (i < 0) return 0;
        if (i > 5) return 0;
        return rent[i];
}

CapiGroup* CapiEstate::getGroup() {
        return group;
}

QString CapiEstate::getColor() {
        return color;
}

QString CapiEstate::getBgColor() {
        return bgColor;
}

int CapiEstate::getHousePrice() {
        return housePrice;
}

int CapiEstate::getSellHousePrice() {
        return sellHousePrice;
}

int CapiEstate::getHouses() {
        return houses;
}

bool CapiEstate::getCanBuyHouse() {
        return canBuyHouse;
}

bool CapiEstate::getCanSellHouse() {
        return canSellHouse;
}

void CapiEstate::setPicture(QString p) {
        picture = p;
}

void CapiEstate::setMoney(int m) {
        money = m;
}

void CapiEstate::setBuyable(bool b) {
        buyable = b;
}

void CapiEstate::setPrice(int p) {
        price = p;
}

void CapiEstate::setOwner(CapiPlayer* p) {
        owner = p;
}

void CapiEstate::setMortageVal(int mv) {
        mortageVal=mv;
}

void CapiEstate::setUnmortageVal(int umv) {
        unmortageVal=umv;
}

void CapiEstate::setMortaged(bool m) {
        mortaged=m;
}

void CapiEstate::setCanToggleMortage(bool ctm) {
        canToggleMortage=ctm;
}

void CapiEstate::setRent(int i, int r) {
        if (i < 0) return;
        if (i > 5) return;
        rent[i] = r;
}

void CapiEstate::setGroup(CapiGroup* g) {
        group = g;
}

void CapiEstate::setColor(QString col) {
        color = col;
}

void CapiEstate::setBgColor(QString col) {
        bgColor = col;
}

void CapiEstate::setHousePrice(int hp) {
        housePrice = hp;
}

void CapiEstate::setSellHousePrice(int shp) {
        sellHousePrice = shp;
}

void CapiEstate::setHouses(int h) {
        houses = h;
}

void CapiEstate::setCanBuyHouse(bool cbh) {
        canBuyHouse = cbh;
}

void CapiEstate::setCanSellHouse(bool csh) {
        canSellHouse = csh;
}
