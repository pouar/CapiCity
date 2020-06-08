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

#ifndef CAPICARD_H
#define CAPICARD_H

#include <QString>
#include "capiobject.h"
#include "capiplayer.h"

class CapiPlayer;

class CapiCard : public CapiObject {

public:
        CapiCard(int i);

        QString     getTitle(); //Adapter for get/(set name method
        CapiPlayer* getOwner();

        void setTitle(QString t);
        void setOwner(CapiPlayer* o);

protected:
        CapiPlayer* owner;
};

#endif // CAPICARD_H
