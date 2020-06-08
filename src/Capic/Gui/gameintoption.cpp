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

#include "gameintoption.h"
#include "ui_gameintoption.h"

GameIntOption::GameIntOption(QString cmd, QString txt, QWidget *parent) : GameOption(cmd, txt, parent), m_ui(new Ui::GameIntOption) {
        m_ui->setupUi(this);

        setText(text);

        connect(m_ui->bSet, SIGNAL(clicked()), this, SLOT(emitOptionChanged()));
        connect(m_ui->sbValue, SIGNAL(editingFinished()), this, SLOT(emitOptionChanged()));
}

GameIntOption::~GameIntOption() {
        delete m_ui;
}

void GameIntOption::setValue(QString value) {
        int val = value.toInt();
        m_ui->sbValue->setValue(val);
        m_ui->lText->setText(text+" ("+tr("Current")+": "+QString::number(val)+"):");
}

void GameIntOption::setText(QString txt) {
        text = txt;
        m_ui->lText->setText(txt+" ("+tr("Current")+": "+QString::number(m_ui->sbValue->value())+"):");
}

void GameIntOption::emitOptionChanged() {
        QString cmd = command+QString::number(m_ui->sbValue->value());
        emit(optionChanged(cmd));
}

void GameIntOption::changeEvent(QEvent *e) {
        QWidget::changeEvent(e);
        switch (e->type()) {
        case QEvent::LanguageChange:
                m_ui->retranslateUi(this);
                break;
        default:
                break;
        }
}
