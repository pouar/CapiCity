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

#ifndef CAPISERVERCARD_H
#define CAPISERVERCARD_H

#include <QMap>

#include "src/Capi/capicard.h"

class CapiServerCard : public CapiCard {

public:
        CapiServerCard(int id);

        bool    getToJail();
        int     getAdvanceTo();
        int     getAdvance();
        int     getPay();
        QString getRentMath();
        QString getAdvanceToNextOf();
        bool    getCanBeOwned();
        bool    getOutOfJail();
        int     getPayHouse();
        int     getPayHotel();
        int     getPayEach();
        bool    getRoll();
        QString getGroup();
        QString getText(QString lang=QString());

        void setToJail(bool tj);
        void setAdvanceTo(int at);
        void setAdvance(int a);
        void setPay(int p);
        void setRentMath(QString rm);
        void setAdvanceToNextOf(QString atn);
        void setCanBeOwned(bool cbo);
        void setOutOfJail(bool ooj);
        void setPayHouse(int ph);
        void setPayHotel(int ph);
        void setPayEach(int pe);
        void setRoll(bool r);
        void setGroup(QString g);
        void setText(QString text, QString lang=QString());

        QString getUpdate();
        QString maskXML(QString txt);

private:
        QMap <QString, QString> texts;
        bool    toJail;
        int     advanceTo;
        int     advance;
        int     pay;
        QString rentMath;
        QString advanceToNextOf;
        bool    canBeOwned;
        bool    outOfJail;
        int     payHouse;
        int     payHotel;
        int     payEach;
        bool    roll;
        QString group;
        QString text;
};

#endif // CAPISERVERCARD_H
