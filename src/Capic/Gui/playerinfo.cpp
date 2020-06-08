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
#include "playerinfo.h"
#include "ui_playerinfo.h"

PlayerInfo::PlayerInfo(QWidget *parent, int avMinHeight) : QWidget(parent), m_ui(new Ui::PlayerInfo) {
        m_ui->setupUi(this);
        playerId = -1;

        m_ui->avatar->setMinimumHeight(avMinHeight);
        connect(m_ui->bTrade, SIGNAL(clicked()), this, SLOT(beginTrade()));

        setMouseTracking(true);
        
        connect(m_ui->bShow, SIGNAL(clicked()), this, SLOT(emitShow()));
        connect(m_ui->bEstates, SIGNAL(clicked()), this, SLOT(showEstateLabel()));

        m_ui->bShow->setVisible(false);
        m_ui->bTrade->setVisible(false);
        m_ui->lEstates->setVisible(false);
        m_ui->bEstates->setVisible(false);
        m_ui->lMoney->setVisible(false);
        //Under Win StyledPanel does not work
#if defined(Q_WS_WIN)
        m_ui->frame->setFrameShape(QFrame::Panel);
#endif

        //Only, if we return to asingle label layout
        //htmlStart = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"> <html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"> p, li { white-space: pre-wrap; } </style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\"> <p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">";
        //htmlMid = "</p><p align=\"right\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">";
        //htmlEnd = "</p></body></html>";
}

PlayerInfo::~PlayerInfo() {
        delete m_ui;
}

void PlayerInfo::setAvatar(QImage a) {
        m_ui->avatar->setAvatar(a);
}

void PlayerInfo::setJailed(bool j) {
        m_ui->avatar->jailed = j;
        m_ui->avatar->update();
}

void PlayerInfo::setSpectator(bool spec) {
        m_ui->avatar->spectating = spec;
        m_ui->avatar->update();
}

void PlayerInfo::setName(QString n) {
        if (n.size() > 25) {
                n = n.left(11)+"..."+n.right(11);
        }
        m_ui->lPlayer->setText(" "+n);
}

void PlayerInfo::setMoney(QString m) {
        m_ui->lMoney->setText(m);
}

void PlayerInfo::changeEvent(QEvent *e) {
        QWidget::changeEvent(e);
        switch (e->type()) {
        case QEvent::LanguageChange:
                m_ui->retranslateUi(this);
                break;
        default:
                break;
        }
}

void PlayerInfo::beginTrade() {
        emit(command(".Tn"+QString::number(playerId)));
}

void PlayerInfo::emitShow() {
        int pid = -1;
        if (m_ui->bShow->isChecked()) pid = playerId;

        emit(showed(pid));
}

void PlayerInfo::showEstateLabel() {
        m_ui->lEstates->setVisible(!m_ui->lEstates->isVisible());
}

void PlayerInfo::resetButtons() {
        m_ui->bShow->setChecked(false);
        m_ui->bEstates->setChecked(false);
        m_ui->lEstates->setVisible(false);
}

void PlayerInfo::resetShowButton() {
        m_ui->bShow->setChecked(false);
}

void PlayerInfo::setTradeButtonVisible(bool vis) {
        m_ui->bTrade->setVisible(vis);
}

void PlayerInfo::setShowButtonVisible(bool vis) {
        m_ui->bShow->setVisible(vis);
}

void PlayerInfo::setEstateButtonVisible(bool vis) {
        m_ui->bEstates->setVisible(vis);
}

void PlayerInfo::setMoneyVisible(bool vis) {
        m_ui->lMoney->setVisible(vis);
}

void PlayerInfo::setEstateText(QString txt) {
        QString text = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">p, li { white-space: pre-wrap; }</style></head><body style=\" font-family:'Sans Serif'; font-size:11pt; font-weight:400; font-style:normal;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">";
        text += txt;
        text += "</p></body></html>";
        m_ui->lEstates->setText(text);
        m_ui->lEstates->setToolTip(text);
}
