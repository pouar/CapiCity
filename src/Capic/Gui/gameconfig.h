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

#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <QtGui/QWidget>
#include "gameoption.h"

namespace Ui {
	class GameConfig;
}

class GameConfig : public QWidget {
	Q_OBJECT

	public:
		GameConfig(QWidget *parent = 0);
		~GameConfig();
		void removeAllOptions();
		void setDesc(QString desc);

	protected:
		void changeEvent(QEvent *e);

	private:
		Ui::GameConfig *m_ui;
		QList<GameOption*> gameOptions;
		GameOption* getOption(QString cmd, QString type="");
		bool enabled;

	private slots:
		void emitCommand(QString cmd);
		void emitDescChange();

	public slots:
		void configUpdate(QString cmd, QString text, QString type, QString value);
		void setOptionsEnabled(bool eenabled);

	signals:
		void optionChanged(QString cmd);
};

#endif // GAMECONFIG_H
