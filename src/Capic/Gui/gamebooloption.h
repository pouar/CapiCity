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

#ifndef GAMEBOOLOPTION_H
#define GAMEBOOLOPTION_H

#include "gameoption.h"

namespace Ui {
	class GameBoolOption;
}

class GameBoolOption : public GameOption {
	Q_OBJECT

	public:
		GameBoolOption(QString cmd, QString txt, QWidget *parent = 0);
		~GameBoolOption();
		void setText(QString txt);
		void setValue(QString val);

	protected:
		void changeEvent(QEvent *e);

	private:
		Ui::GameBoolOption *m_ui;

	private slots:
		void emitOptionChanged(bool ch);
};

#endif // GAMEBOOLOPTION_H
