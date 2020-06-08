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

#include "gameconfig.h"
#include "ui_gameconfig.h"
#include "gameoption.h"
#include "gamebooloption.h"
#include "gameintoption.h"

GameConfig::GameConfig(QWidget *parent) : QWidget(parent), m_ui(new Ui::GameConfig) {
        m_ui->setupUi(this);
        m_ui->bLeave->cmd = ".gx";
        m_ui->bStart->cmd = ".gs";
        enabled = false;
        m_ui->bStart->setEnabled(false);

        connect(m_ui->bLeave, SIGNAL(cmdClicked(QString)), this, SLOT(emitCommand(QString)));
        connect(m_ui->bStart, SIGNAL(cmdClicked(QString)), this, SLOT(emitCommand(QString)));
        connect(m_ui->bDescSet, SIGNAL(clicked()), this, SLOT(emitDescChange()));
}

GameConfig::~GameConfig() {
        delete m_ui;
}

void GameConfig::removeAllOptions() {
        while (gameOptions.size() > 0) {
                GameOption* o = gameOptions.value(0);
                gameOptions.removeAll(o);
                delete o;
        }
}

void GameConfig::setDesc(QString desc) {
        m_ui->tfDesc->setText(desc);
}

GameOption* GameConfig::getOption(QString cmd, QString type) {
        GameOption* go = 0;
        for (int i= 0; i < gameOptions.size(); i++)
                if (gameOptions.value(i)->getCommand() == cmd) return gameOptions.value(i);

        if (type != "") {
                if (type == "bool") go = new GameBoolOption(cmd, "", this);
                if (type == "int") go = new GameIntOption(cmd, "", this);

                if (go == 0) return 0;
                gameOptions.append(go);
                m_ui->options->addWidget(go);
                connect(go, SIGNAL(optionChanged(QString)), this, SLOT(emitCommand(QString)));
        }
        return go;
}

void GameConfig::emitCommand(QString cmd) {
        emit(optionChanged(cmd));
}

void GameConfig::emitDescChange() {
        if (m_ui->tfDesc->isEnabled())
                emit(optionChanged(".gd"+m_ui->tfDesc->text()));
}

void GameConfig::configUpdate(QString cmd, QString text, QString type, QString value) {
        GameOption* opt = getOption(cmd, type);
        if (opt == 0) return;

        disconnect(opt, SIGNAL(optionChanged(QString)), this, SLOT(emitCommand(QString)));
        if (text != "")
                opt->setText(text);
        opt->setEnabled(enabled);
        opt->setValue(value);
        connect(opt, SIGNAL(optionChanged(QString)), this, SLOT(emitCommand(QString)));
}

void GameConfig::setOptionsEnabled(bool e) {
        enabled = e;
        foreach(GameOption* o, gameOptions)
                o->setEnabled(enabled);
        m_ui->bStart->setEnabled(enabled);
        m_ui->bStart->setVisible(enabled);
        m_ui->tfDesc->setEnabled(enabled);
        m_ui->bDescSet->setVisible(enabled);
}

void GameConfig::changeEvent(QEvent *e) {
        QWidget::changeEvent(e);

        switch (e->type()) {
        case QEvent::LanguageChange:
                m_ui->retranslateUi(this);
                break;
        default:
                break;
        }
}
