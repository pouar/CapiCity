// Copyright 2009,2011 Thomas Kamps anubis1@linux-ecke.de
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

#include <stdlib.h>
#include <time.h>

#include <QtWidgets/QApplication>
#include <QTcpSocket>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

#include "src/Capic/Gui/capicity.h"
#include "src/Capic/Gui/field.h"

int main(int argc, char *argv[]) {
	srand(time(NULL));
	QApplication a(argc, argv);

	QTranslator trans;
	QString filename = QString("ts/CapiCity_%1").arg(QLocale::system().name());
	trans.load(filename, qApp->applicationDirPath());
	a.installTranslator(&trans);

	QTranslator trans2;
	trans2.load(QLibraryInfo::location(QLibraryInfo::TranslationsPath)+QString("/qt_%1").arg(QLocale::system().name()), QCoreApplication::applicationDirPath());
	a.installTranslator(&trans2);

	QCoreApplication::setOrganizationName("CapiCity");
	QCoreApplication::setApplicationName("CapiCity");

	CapiCity w;

	w.show();
	//c.show();
	return a.exec();
}
