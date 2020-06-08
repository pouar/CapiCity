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

#ifndef PLAYERAUCTION_H
#define PLAYERAUCTION_H

#include <QtGui/QWidget>

namespace Ui {
	class PlayerAuction;
}

class PlayerAuction : public QWidget {
	Q_OBJECT

	public:
		PlayerAuction(QWidget *parent = 0, int avMinHeight= 0);
		~PlayerAuction();
		void setAvatar(QImage a);
		void setName(QString n);
		void setMoney(QString m);
		int playerId;

	protected:
		void changeEvent(QEvent* e);

	private:
		Ui::PlayerAuction *m_ui;
};

#endif // PLAYERINFO_H
