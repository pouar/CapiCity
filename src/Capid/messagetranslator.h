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

#ifndef MESSAGETranslator_H
#define MESSAGETranslator_H

#include <QString>
#include <QMap>

//A Static Class to generate Display messages to the client
class MessageTranslator {

	public:
		MessageTranslator();
		~MessageTranslator();

		void loadLang(QString lang);
		QString translateMessage(QString text, QString lang, QMap<QString, QString>* args);
		QString translate(QString text, QString lang);
		QString replaceVars(QString text, QMap<QString, QString>* args);

	private:
		QMap<QString, QMap<QString,QString>*> translators;
};

#endif // MESSAGETranslator_H
