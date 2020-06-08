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

#include <QMouseEvent>
#include "playerauction.h"
#include "ui_playerauction.h"

PlayerAuction::PlayerAuction(QWidget *parent, int avMinHeight) : QWidget(parent), m_ui(new Ui::PlayerAuction) {
        m_ui->setupUi(this);
        playerId = -1;

        m_ui->avatar->setMinimumHeight(avMinHeight);

        //Under Win StyledPanel does not work
#if defined(Q_WS_WIN)
        m_ui->frame->setFrameShape(QFrame::Panel);
#endif
}

PlayerAuction::~PlayerAuction() {
        delete m_ui;
}

void PlayerAuction::setAvatar(QImage a) {
        m_ui->avatar->setAvatar(a);
}

void PlayerAuction::setName(QString n) {
        m_ui->lPlayer->setText(" "+n);
}

void PlayerAuction::setMoney(QString m) {
        m_ui->lMoney->setText("   "+m);
}

void PlayerAuction::changeEvent(QEvent *e) {
        QWidget::changeEvent(e);
        switch (e->type()) {
        case QEvent::LanguageChange:
                m_ui->retranslateUi(this);
                break;
        default:
                break;
        }
}
