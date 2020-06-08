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

#include "capigroup.h"

CapiGroup::CapiGroup(int id, QString n) : CapiObject(id) {
	setName(n);
}

int CapiGroup::getNumEstates() {
	return estates.size();
}

CapiEstate* CapiGroup::getEstate(int i) {
	if (i < 0) return 0;
	if (i >= getNumEstates()) return 0;

	return estates.value(i);
}

void CapiGroup::addEstate(CapiEstate* f) {
	estates.append(f);
}

bool CapiGroup::getAllEstatesSameOwner() {
	if (estates.size() == 0) return false;
	CapiPlayer* owner = estates.value(0)->getOwner();
	if (owner == 0) return false;

	foreach (CapiEstate* f, estates) {
		if ((f->getOwner() == 0) || (owner->getId() != f->getOwner()->getId())) return false;
	}

	return true;
}

int CapiGroup::getNumHouses() {
	int n = 0;

	foreach (CapiEstate* f, estates) {
		n += f->getHouses();
	}

	return n;
}

bool CapiGroup::canBuyHouseRow() {
	bool canBuyRow = true;
	foreach (CapiEstate* f, estates) {
		canBuyRow = canBuyRow && f->getCanBuyHouse();
	}
	return canBuyRow;
}

bool CapiGroup::canSellHouseRow() {
	bool canSellRow = true;
	foreach (CapiEstate* f, estates) {
		canSellRow = canSellRow && f->getCanSellHouse();
	}
	return canSellRow;
}

