// Copyright 2009 Thomas Kamps anubis1@linux-ecke.de
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

#include <QPainter>
#include <QPaintEvent>
#include "playeravatar.h"

PlayerAvatar::PlayerAvatar(QWidget* parent) : QWidget(parent) {
        jailed = false;
        spectating = false;
}

void PlayerAvatar::setAvatar(QImage a) {
        avatar = a;
        update();
}

void PlayerAvatar::paintEvent(QPaintEvent* pe) {
        QPainter p(this);
        int px = (width()-height())/2;
        p.drawImage(QRect(px, 0, height(), height()), avatar);
        if (jailed)
                p.drawPixmap(px,0, height(), height(), QPixmap(":/images/field/jailed.png"));

        if (spectating)
                p.drawPixmap(px+height()/2, 3*height()/4, height()/2, height()/4, QPixmap(":/images/avatars/spectating.png"));

        pe->accept();
}
