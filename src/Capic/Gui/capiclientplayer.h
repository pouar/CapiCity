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

#ifndef CAPICLIENTPLAYER_H
#define CAPICLIENTPLAYER_H

#include <QColor>
#include <QPixmap>
#include "playerinfo.h"
#include "playerauction.h"
#include "src/Capi/capiplayer.h"
#include "src/Capic/capiclientgame.h"

class CapiClientPlayer : public QObject, public CapiPlayer {
	Q_OBJECT
	Q_PROPERTY(int offset READ getOffset WRITE setOffset)

	public:
		CapiClientPlayer(int id);
		~CapiClientPlayer();
		QString getChatColor();

		void setGame(CapiClientGame* g);
		void setName(QString n);
		void setAvatar(QString a);
		void setMoney(int m);
		void setInJail(bool j);
		void setSpectator(bool spec);
		void setHasTurn(bool ht);
		void setPlayerWidgetVisible(bool vis);
		void setOffset(int o);
		int getOffset();

		PlayerInfo* playerWidget;
		PlayerAuction* auction;

		QImage avImage;
		QImage shadow;

	private:
		QString chatColor;
		int offset;
		bool posAc;
		void weakShadow();
		void blurShadow();

	signals:
		void positionChanged(int newPos);
};

#endif // CAPICLIENTPLAYER_H
