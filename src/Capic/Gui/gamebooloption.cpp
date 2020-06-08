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

#include "gamebooloption.h"
#include "ui_gamebooloption.h"

GameBoolOption::GameBoolOption(QString cmd, QString txt, QWidget *parent) : GameOption(cmd, txt, parent), m_ui(new Ui::GameBoolOption) {
	m_ui->setupUi(this);

	m_ui->cbOption->setText(txt);

	connect(m_ui->cbOption, SIGNAL(toggled(bool)), this, SLOT(emitOptionChanged(bool)));
}

GameBoolOption::~GameBoolOption() {
	delete m_ui;
}

void GameBoolOption::setText(QString txt) {
	m_ui->cbOption->setText(txt);
}

void GameBoolOption::setValue(QString val) {
	bool value = false;
	if(val == "1") value = true;

	m_ui->cbOption->setChecked(value);
}

void GameBoolOption::emitOptionChanged(bool ch) {
	QString cmd = command;
	if (ch) cmd = cmd+"1";
	else cmd = cmd+"0";

	emit(optionChanged(cmd));
}

void GameBoolOption::changeEvent(QEvent *e) {
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
