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

#ifndef CAPIGAME_H
#define CAPIGAME_H

#include <QList>
#include "capiestate.h"
#include "capigroup.h"
#include "capitemplate.h"
#include "capicard.h"
#include "capiplayer.h"

class CapiEstate;
class CapiGroup;
class CapiTemplate;
class CapiCard;
class CapiPlayer;

class CapiGame : public CapiTemplate {

public:
        CapiGame(int id);
        ~CapiGame();

        CapiEstate* getEstate(int id, bool create=true);
        CapiGroup* getGroup(int id, QString name = "", bool create=true);
        CapiPlayer* getPlayer(int id, bool create=true);
        CapiPlayer* getPlayerFromList(int num);
        bool    hasPlayer(int id);
        CapiCard* getCard(int id, bool create=true);
        CapiCard* getCardFromList(int i);
        QString getStatus();
        bool    getCanBeJoined();
        int     getNumPlayer();
        int     getMinPlayer();
        int     getMaxPlayer();
        CapiPlayer* getMaster();
        bool getSellEnabled();

        int getNumEstates();
        int getNumCards();

        void addPlayer(CapiPlayer* p);
        void removePlayer(CapiPlayer* p);

        void setStatus(QString st);
        void setCanBeJoined(bool cbj);
        void setNumPlayer(int np);
        void setMinPlayer(int mp);
        void setMaxPlayer(int mp);
        void setMaster(CapiPlayer* m);
        void setSellEnabled(int se);

protected:
        QList<CapiEstate*> estates;
        QList<CapiGroup*> groups;
        QList<CapiPlayer*> players;
        QList<CapiCard*> cards;

        QString status;
        bool canBeJoined;
        int numPlayer;
        int minPlayer;
        int maxPlayer;
        CapiPlayer* master;
        bool sellEnabled;
};

#endif // CAPIGAME_H
