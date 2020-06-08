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

#include "capiclientplayer.h"
#include <QPainter>
#include <QDebug>

CapiClientPlayer::CapiClientPlayer(int id) : QObject(0), CapiPlayer(id) {
	playerWidget = new PlayerInfo(0, 32);
	playerWidget->setName(getName());
	playerWidget->setMoney(QString("0"));
	playerWidget->setAvatar(avImage);

	auction = new PlayerAuction();
	auction->setName(getName());
	auction->setMoney(QString("0"));
	auction->setAvatar(avImage);

	playerWidget->playerId = getId();
	auction->playerId = getId();

	offset = 0;
	posAc = false;

	//Set a random chat color for that player
	int baseColorVal = playerWidget->palette().color(QPalette::Base).value();
	QColor col;
	int h = (rand() % (360));
	int s = 192+(rand() % (64));

	int v = (baseColorVal+96+(rand() % (64)))%255;
	col.setHsv(h,s,v);
	chatColor = col.name();
}

CapiClientPlayer::~CapiClientPlayer() {
	delete playerWidget;
	delete auction;
}

QString CapiClientPlayer::getChatColor() {
	return chatColor;
}

void CapiClientPlayer::setPlayerWidgetVisible(bool vis) {
	playerWidget->setVisible(vis);
	auction->setVisible(vis);
}

void CapiClientPlayer::setGame(CapiClientGame* g) {
	CapiPlayer::setGame(g);
	playerWidget->setMoney(QString::number(getMoney()));
	if (g == 0) setInJail(false);
}

void CapiClientPlayer::setName(QString n) {
	CapiPlayer::setName(n);
	playerWidget->setName(n);
	auction->setName(n);
}

void CapiClientPlayer::setAvatar(QString a) {
	if (a.startsWith("DATA")) {
		QByteArray hexData = a.right(a.size()-4).toUtf8();
		QByteArray data = QByteArray::fromHex(hexData);
		avImage = QImage::fromData(data);
		avImage = avImage.scaled(64,64, Qt::KeepAspectRatio);
	} else {
		avImage = QImage(":/images/avatars/"+a);
	}
	if (avImage.isNull()) {
		avImage = QImage(":/images/avatars/none.png");
	}

	if (getInJail()) {
		QPainter p(&avImage);
		p.drawPixmap(0,0,avImage.width(), avImage.height(), QPixmap(":/images/field/jailed.png"));
		p.end();
	}

	CapiPlayer::setAvatar(a);
	playerWidget->setAvatar(avImage);
	auction->setAvatar(avImage);

	weakShadow();
	blurShadow();
}

void CapiClientPlayer::setMoney(int m) {
	CapiPlayer::setMoney(m);
	playerWidget->setMoney(QString::number(getMoney()));
}

void CapiClientPlayer::setInJail(bool j) {
	CapiPlayer::setInJail(j);
	playerWidget->setJailed(j);
	setAvatar(getAvatar());
}

void CapiClientPlayer::setHasTurn(bool ht) {
	CapiPlayer::setHasTurn(ht);
	if (getHasTurn()) playerWidget->setName("<b>"+getName()+"</b>");
	else playerWidget->setName(getName());
}

void CapiClientPlayer::setSpectator(bool spec) {
	CapiPlayer::setSpectator(spec);
	playerWidget->setSpectator(spec);
	setAvatar(getAvatar());
}

void CapiClientPlayer::setOffset(int o) {
	offset = o;
/*
	if ((offset == 100) && !posAc) {
		setPos((getPos()+1)%getGame()->getNumEstates());
		emit(positionChanged(getPos()));
		posAc = true;
		offset = 0;
	}
	*/
}

int CapiClientPlayer::getOffset() {
	return offset;
}

void CapiClientPlayer::weakShadow() {
	QImage shdw = avImage.alphaChannel();
	shadow = QImage(shdw.width(), shdw.height(), QImage::Format_ARGB32);
	for (int x = 0; x < shdw.width(); x++) {
		for (int y = 0; y < shdw.height(); y++) {
			QColor c = QColor(shdw.pixel(x,y));
			c.setRed(c.red()/2);
			c.setGreen(c.green()/2);
			c.setBlue(c.blue()/2);
			shadow.setPixel(x,y,c.rgb());
		}
	}
}

void CapiClientPlayer::blurShadow() {
	QImage blur(shadow.width()+10, shadow.height()+10, QImage::Format_ARGB32);
	QImage shdw(shadow.width()+10, shadow.height()+10, QImage::Format_ARGB32);

	//Copy shadow to shdw (centered)
	for (int x = 0; x < shdw.width(); x++) {
	for (int y = 0; y < shdw.height(); y++) {
		shdw.setPixel(x,y,qRgb(0,0,0));
		if ((x >= 5) && (y >= 5) && (x < shadow.width()+5) && (y < shadow.height()+5)) shdw.setPixel(x,y, shadow.pixel(x-5,y-5));
	}
	}

	//Create a blured copy of shadow
	for (int x = 0; x < blur.width(); x++) {
	for (int y = 0; y < blur.height(); y++) {
		int numPix = 0;
		int sum = 0;
		for (int z = x-5; z <= x+5; z++) {
		for (int w = y-5; w <= y+5; w++) {
			if ((z >= 0) && (z < blur.width()) && (w >= 0) && (w < blur.height())) {
				if ((z-x)*(z-x)+(w-y)*(w-y) <= 25) {
					numPix++;
					sum += QColor(shdw.pixel(z,w)).red();
				}
			}
		}
		}
		int col = sum/numPix;
		blur.setPixel(x,y, QColor(col, col, col).rgb());
	}
	}

	shadow = QImage(blur.width(), blur.height(), QImage::Format_ARGB32);
	//make shadow complety black
	for (int x = 0; x < shadow.width(); x++) {
	for (int y = 0; y < shadow.height(); y++) {
		shadow.setPixel(x,y, qRgb(0,0,0));
	}
	}

	shadow.setAlphaChannel(blur);
}
