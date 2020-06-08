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

#ifndef PLAYERAVATAR_H
#define PLAYERAVATAR_H

#include <QWidget>

class PlayerAvatar : public QWidget {
	Q_OBJECT

	public:
		PlayerAvatar(QWidget* parent=0);
		void setAvatar(QImage a);
		bool jailed;
		bool spectating;

	protected:
		void paintEvent(QPaintEvent* pe);

	private:
		QImage avatar;

};

#endif // PLAYERAVATAR_H
