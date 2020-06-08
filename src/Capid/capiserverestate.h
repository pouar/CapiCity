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

#ifndef CAPISERVERESTATE_H
#define CAPISERVERESTATE_H

#include <QMap>
#include "src/Capi/capiestate.h"
#include "capiserverestate.h"

class CapiServerEstate : public CapiEstate {

public:
        CapiServerEstate(int id);
        ~CapiServerEstate();

        QString           getName(QString lang=QString());
        int               getTax();
        int               getTaxPercent();
        QString           getTakeCard();
        int               getPassmoney();
        CapiServerEstate* getPayTarget();
        bool              getToJail();
        bool              getJail();
        int               getPayAmount();
        bool              getFreeParking();
        bool              getGo();
        int               getAdvanceToIfOwned();

        void setName(QString name, QString lang=QString());
        void setTax(int t);
        void setTaxPercent(int tp);
        void setPayTarget(CapiServerEstate* pt);
        void setTakeCard(QString tc);
        void setPassmoney(int pm);
        void setToJail(bool tj);
        void setJail(bool j);
        void setPayAmount(int pa);
        void setFreeParking(bool fp);
        void setGo(bool g);
        void setAdvanceToIfOwned(int atio);
        void setPrice(int p);

        QString getUpdate(QString type="", QString lang=QString());
        QString maskXML(QString txt);

private:
        QMap<QString, QString> names;
        int tax;
        int taxpercent;
        QString takeCard;
        int passmoney;
        CapiServerEstate* payTarget;
        bool toJail;
        bool jail;
        int payAmount;
        bool freeParking;
        bool go;
        int advanceToIfOwned;
};

#endif // CAPISERVERESTATE_H
