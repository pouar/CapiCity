// Copyright 2010 Thomas Kamps anubis1@linux-ecke.de
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

#include "capiclientgame.h"

CapiClientGame::CapiClientGame(int id) : CapiGame(id) {
	listed = true;
}

CapiClientEstate* CapiClientGame::getEstate(int id) {
	if (id < 0) return 0;
	foreach (CapiEstate* e, estates)
		if (e->getId() == id) return (CapiClientEstate* ) e;

	CapiClientEstate* f = new CapiClientEstate(id);
	estates.append(f);
	return f;
}

CapiGroup* CapiClientGame::getGroupFromList(int i) {
	if (i < 0) return 0;
	if ( i >= groups.size()) return 0;
	return groups.value(i);
}

int CapiClientGame::getGroupListNumber(int gid) {
	for (int i = 0; i < groups.size(); i++) {
		CapiGroup* g = groups.value(i);
		if (g->getId() == gid) return i;
	}
	return -1;
}

int CapiClientGame::getNumGroups() {
	return groups.size();
}

bool CapiClientGame::getListed() {
	return listed;
}

void CapiClientGame::setListed(bool l) {
	listed = l;
}
