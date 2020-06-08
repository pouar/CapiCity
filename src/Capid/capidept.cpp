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

#include "capidept.h"

CapiDept::CapiDept(CapiPlayer* fPlayer, CapiPlayer* tPlayer, CapiEstate* tEstate, int a) {
        fromPlayer = fPlayer;
        toPlayer = tPlayer;
        toEstate = tEstate;
        amount = a;
        payed = false;
}

bool CapiDept::getPayed() {
        return payed;
}

void CapiDept::setPayed(bool  p) {
        payed = p;
}

CapiPlayer* CapiDept::getFromPlayer() {
        return fromPlayer;
}

CapiPlayer* CapiDept::getToPlayer() {
        return toPlayer;
}

CapiEstate* CapiDept::getToEstate() {
        return toEstate;
}

int CapiDept::getAmount() {
        return amount;
}

bool CapiDept::pay() {
        //Pays dept, if possible;
        if (fromPlayer->getMoney() < amount) return false;

        fromPlayer->setMoney(fromPlayer->getMoney()-amount);
        if (toPlayer != 0) {
                toPlayer->setMoney(toPlayer->getMoney()+amount);
        } else if (toEstate != 0) {
                toEstate->setMoney(toEstate->getMoney()+amount);
        }
        payed = true;
        return true;
}
