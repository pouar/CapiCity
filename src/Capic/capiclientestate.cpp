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

#include "capiclientestate.h"

CapiClientEstate::CapiClientEstate(int i) : CapiEstate(i) {
	rect = QRect();
}

QRect CapiClientEstate::getRect() {
	return rect;
}

bool CapiClientEstate::isInRect(int x, int y) {
	if (rect.isNull()) return false;

	return (x >= rect.x()) && (x <= rect.x()+rect.width()) &&
	       (y >= rect.y()) && (y <= rect.y()+rect.height());
}

QColor CapiClientEstate::getBrushColor() {
	if (getColor() != "") {
		bool ok;
		int r = getColor().remove(0,1).remove(2,4).toInt(&ok, 16);
		int g = getColor().remove(0,3).remove(2,2).toInt(&ok, 16);
		int b = getColor().remove(0,5).toInt(&ok, 16);

		return QColor(r,g,b);
	} else {
		return QColor(0,0,0,0);
	}
}

QColor CapiClientEstate::getBgBrushColor() {
	if (getBgColor() != "") {
		bool ok;
		int r = getBgColor().remove(0,1).remove(2,4).toInt(&ok, 16);
		int g = getBgColor().remove(0,3).remove(2,2).toInt(&ok, 16);
		int b = getBgColor().remove(0,5).toInt(&ok, 16);

		return QColor(r,g,b);
	} else {
		return QColor(0,0,0,0);
	}
}

void CapiClientEstate::setRect(QRect r) {
	rect = r;
}
