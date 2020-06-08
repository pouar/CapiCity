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

#include "capiserverestate.h"

CapiServerEstate::CapiServerEstate(int id) : CapiEstate(id) {
	tax = 0;
	taxpercent = 0;
	takeCard = "";
	passmoney = 0;
	payTarget = 0;
	toJail = false;
	jail = false;
	payAmount = 0;
	freeParking = true;
	go = false;
	advanceToIfOwned = -1;
}

QString CapiServerEstate::maskXML(QString txt) {
	QString msg = txt.replace("&", "&#38;");
	msg = msg.replace("<", "&#60;");
	msg = msg.replace(">", "&#62;");
	msg = msg.replace("=", "&#61;");
	msg = msg.replace("\"", "&#34;");
	return msg;
}

QString CapiServerEstate::getName(QString lang) {
	if (names.keys().contains(lang)) return names.value(lang);
	return CapiEstate::getName();
}

int CapiServerEstate::getTax() {
	return tax;
}

int CapiServerEstate::getTaxPercent() {
	return taxpercent;
}

QString CapiServerEstate::getTakeCard() {
	return takeCard;
}

int CapiServerEstate::getPassmoney() {
	return passmoney;
}

CapiServerEstate* CapiServerEstate::getPayTarget() {
	return payTarget;
}

bool CapiServerEstate::getToJail() {
	return toJail;
}

bool CapiServerEstate::getJail() {
	return jail;
}

int CapiServerEstate::getPayAmount() {
	return payAmount;
}

bool CapiServerEstate::getFreeParking() {
	return freeParking;
}

bool CapiServerEstate::getGo() {
	return go;
}

int CapiServerEstate::getAdvanceToIfOwned() {
	return advanceToIfOwned;
}

void CapiServerEstate::setName(QString name, QString lang) {
	if (lang.isNull()) CapiEstate::setName(name);
	else names.insert(lang, name);
}

void CapiServerEstate::setTax(int t) {
	tax = t;
}

void CapiServerEstate::setTaxPercent(int tp) {
	taxpercent = tp;
}

void CapiServerEstate::setTakeCard(QString tc) {
	takeCard = tc;
}

void CapiServerEstate::setPassmoney(int pm) {
	passmoney = pm;
}

void CapiServerEstate::setPayTarget(CapiServerEstate* te) {
	payTarget = te;
}

void CapiServerEstate::setToJail(bool tj) {
	toJail = tj;
}

void CapiServerEstate::setJail(bool j) {
	jail = j;
}

void CapiServerEstate::setPayAmount(int pa) {
	payAmount = pa;
}

void CapiServerEstate::setFreeParking(bool fp) {
	freeParking = fp;
}

void CapiServerEstate::setGo(bool g) {
	go = g;
}

void CapiServerEstate::setAdvanceToIfOwned(int atio) {
	advanceToIfOwned = atio;
}

void CapiServerEstate::setPrice(int p) {
	CapiEstate::setPrice(p);
	setMortageVal(p/2);
	setUnmortageVal(11*(p/20));
}

QString CapiServerEstate::getUpdate(QString type, QString lang) {
	QString ret = "<estateupdate ";
	int oid = -1;
	if (getOwner() != 0) oid = getOwner()->getId();
	int gid = -1;
	if (getGroup() != 0) gid = getGroup()->getId();

	ret += "estateid=\""+QString::number(getId())+"\" ";
	if ((type == "") || (type.contains("name")))                ret += QString("name=\"%1\" ").arg(maskXML(getName(lang)));
	if ((type == "") || (type.contains("icon")))                ret += QString("icon=\"%1\" ").arg(maskXML(getPicture()));
	if ((type == "") || (type.contains("owner")))               ret += QString("owner=\"%1\" ").arg(oid);
	if ((type == "") || (type.contains("houses")))              ret += QString("houses=\"%1\" ").arg(getHouses());
	if ((type == "") || (type.contains("houseprice")))          ret += QString("houseprice=\"%1\" ").arg(getHousePrice());
	if ((type == "") || (type.contains("sellhouseprice")))      ret += QString("sellhouseprice=\"%1\" ").arg(getSellHousePrice());
	if ((type == "") || (type.contains("mortgaged")))           ret += QString("mortgaged=\"%1\" ").arg(getMortaged());
	if ((type == "") || (type.contains("group")))               ret += QString("group=\"%1\" ").arg(gid);
	if ((type == "") || (type.contains("can_be_owned")))        ret += QString("can_be_owned=\"%1\" ").arg(getBuyable());
	if ((type == "") || (type.contains("can_toggle_mortgage"))) ret += QString("can_toggle_mortgage=\"%1\" ").arg(getCanToggleMortage());
	if ((type == "") || (type.contains("can_buy_houses")))      ret += QString("can_buy_houses=\"%1\" ").arg(getCanBuyHouse());
	if ((type == "") || (type.contains("can_sell_houses")))     ret += QString("can_sell_houses=\"%1\" ").arg(getCanSellHouse());
	if ((type == "") || (type.contains("money")))               ret += QString("money=\"%1\" ").arg(getMoney());
	if ((type == "") || (type.contains("price")))               ret += QString("price=\"%1\" ").arg(getPrice());
	if ((type == "") || (type.contains("mortgageprice")))       ret += QString("mortgageprice=\"%1\" ").arg(getMortageVal());
	if ((type == "") || (type.contains("unmortgageprice")))     ret += QString("unmortgageprice=\"%1\" ").arg(getUnmortageVal());
	if (((type == "") || (type.contains("color"))) && (color != "")) ret += QString("color=\"%1\" ").arg(getColor());
	if (((type == "") || (type.contains("bgcolor"))) && (bgColor != "")) ret += QString("bgcolor=\"%1\" ").arg(getBgColor());
	if ((type == "") || (type.contains("rent"))) {
		for (int i = 0; i < 6; i++)
		ret += QString("rent%1=\"%2\" ").arg(i).arg(getRent(i));
	}
	ret += "/>";
	return ret;
}
