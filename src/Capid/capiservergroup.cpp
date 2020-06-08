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

#include "capiservergroup.h"

CapiServerGroup::CapiServerGroup(int id, QString n) : CapiGroup(id, n) {
        color = "";
        bgColor = "";
        housePrice = 0;
        price = -1;
        rentMath = "";
}

QString CapiServerGroup::maskXML(QString txt) {
        QString msg = txt.replace("&", "&#38;");
        msg = msg.replace("<", "&#60;");
        msg = msg.replace(">", "&#62;");
        msg = msg.replace("=", "&#61;");
        msg = msg.replace("\"", "&#34;");
        return msg;
}

QString CapiServerGroup::getName(QString lang) {
        if (names.keys().contains(lang)) return names.value(lang);
        return CapiGroup::getName();
}

QString CapiServerGroup::getColor() {
        return color;
}

QString CapiServerGroup::getBgColor() {
        return bgColor;
}

int CapiServerGroup::getHousePrice() {
        return housePrice;
}

int CapiServerGroup::getPrice() {
        return price;
}

QString CapiServerGroup::getRentMath() {
        return rentMath;
}

void CapiServerGroup::setName(QString name, QString lang) {
        if (lang.isNull()) CapiGroup::setName(name);
        else names.insert(lang, name);
}

void CapiServerGroup::setColor(QString col) {
        color = col;
}

void CapiServerGroup::setBgColor(QString bgcol) {
        bgColor = bgcol;
}

void CapiServerGroup::setHousePrice(int hp) {
        housePrice = hp;
}

void CapiServerGroup::setPrice(int p) {
        price = p;
}

void CapiServerGroup::setRentMath(QString rm) {
        rentMath = rm;
}
