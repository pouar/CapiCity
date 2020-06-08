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

#ifndef CAPIGROUP_H
#define CAPIGROUP_H

#include <QList>
#include "capiestate.h"
#include "capiobject.h"

class CapiEstate;

class CapiGroup : public CapiObject {
public:
        CapiGroup(int id, QString n);

        int getNumEstates();
        CapiEstate* getEstate(int i);

        void addEstate(CapiEstate* f);

        bool getAllEstatesSameOwner(); //Checks, if all estates of the group have the same , not -1 owner
        int getNumHouses(); //Checks, if there is a house on any of the Estate of the groups
        bool canBuyHouseRow();
        bool canSellHouseRow();

protected:
        QList<CapiEstate*> estates;

};

#endif // CAPIGROUP_H
