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

#ifndef CAPISERVERGROUP_H
#define CAPISERVERGROUP_H

#include <QMap>

#include "src/Capi/capigroup.h"

class CapiServerGroup : public CapiGroup {

public:
        CapiServerGroup(int id, QString n);

        QString getName(QString lang=QString());
        QString getColor();
        QString getBgColor();
        int     getHousePrice();
        int     getPrice();
        QString getRentMath();
        QString maskXML(QString txt);

        void setName(QString name, QString lang=QString());
        void setColor(QString col);
        void setBgColor(QString bgcol);
        void setHousePrice(int hp);
        void setPrice(int p);
        void setRentMath(QString rm);

private:
        QMap<QString, QString> names;
        QString color;
        QString bgColor;
        int housePrice;
        int price;
        QString rentMath;

};

#endif // CAPISERVERGROUP_H
