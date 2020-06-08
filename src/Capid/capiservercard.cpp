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

#include "capiservercard.h"

CapiServerCard::CapiServerCard(int id) : CapiCard(id) {
        toJail = false;
        advanceTo = -1;
        advance = 0;
        pay = 0;
        rentMath = "";
        advanceToNextOf = "";
        canBeOwned = false;
        outOfJail = false;
        payHouse = 0;
        payHotel = 0;
        payEach = 0;
        roll = false;
        group = "";
        text = "";
}

QString CapiServerCard::maskXML(QString txt) {
        QString msg = txt.replace("&", "&#38;");
        msg = msg.replace("<", "&#60;");
        msg = msg.replace(">", "&#62;");
        msg = msg.replace("=", "&#61;");
        msg = msg.replace("\"", "&#34;");
        return msg;
}

bool CapiServerCard::getToJail() {
        return toJail;
}

int CapiServerCard::getAdvanceTo() {
        return advanceTo;
}

int CapiServerCard::getAdvance() {
        return advance;
}

int CapiServerCard::getPay() {
        return pay;
}

QString CapiServerCard::getRentMath() {
        return rentMath;
}

QString CapiServerCard::getAdvanceToNextOf() {
        return advanceToNextOf;
}

bool CapiServerCard::getCanBeOwned() {
        return canBeOwned;
}

bool CapiServerCard::getOutOfJail() {
        return outOfJail;
}

int CapiServerCard::getPayHouse() {
        return payHouse;
}

int CapiServerCard::getPayHotel() {
        return payHotel;
}

int CapiServerCard::getPayEach() {
        return payEach;
}

bool CapiServerCard::getRoll() {
        return roll;
}

QString CapiServerCard::getGroup() {
        return group;
}

QString CapiServerCard::getText(QString lang) {
        if (texts.keys().contains(lang)) return texts.value(lang);
        return text;
}

QString CapiServerCard::getUpdate() {
        int oid = -1;
        if (getOwner() != 0) oid = getOwner()->getId();

        QString ret = QString("<cardupdate ");
        ret += QString("cardid=\"%1\" ").arg(getId());
        ret += QString("title=\"%2\" ").arg(maskXML(getTitle()));
        ret += QString("owner=\"%3\" ").arg(oid);
        ret += "/>";

        return ret;
}

void CapiServerCard::setToJail(bool tj) {
        toJail = tj;
}

void CapiServerCard::setAdvanceTo(int at) {
        advanceTo = at;
}

void CapiServerCard::setAdvance(int a) {
        advance = a;
}

void CapiServerCard::setPay(int p) {
        pay = p;
}

void CapiServerCard::setRentMath(QString rm) {
        rentMath = rm;
}

void CapiServerCard::setAdvanceToNextOf(QString atn) {
        advanceToNextOf = atn;
}

void CapiServerCard::setCanBeOwned(bool cbo) {
        canBeOwned = cbo;
}

void CapiServerCard::setOutOfJail(bool ooj) {
        outOfJail = ooj;
}

void CapiServerCard::setPayHouse(int ph) {
        payHouse = ph;
}

void CapiServerCard::setPayHotel(int ph) {
        payHotel = ph;
}

void CapiServerCard::setPayEach(int pe) {
        payEach = pe;
}

void CapiServerCard::setRoll(bool r) {
        roll = r;
}

void CapiServerCard::setGroup(QString g) {
        group = g;
}

void CapiServerCard::setText(QString t, QString lang) {
        if (lang.isNull()) text = t;
        else texts.insert(lang, t);
}
