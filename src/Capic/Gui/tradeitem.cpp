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

#include "tradeitem.h"
#include "ui_tradeitem.h"

TradeItem::TradeItem(int tid, QString t, QWidget *parent) : QWidget(parent), m_ui(new Ui::TradeItem) {
	m_ui->setupUi(this);

	tradeId = tid;
	
	if (t == "card") type = "card";
	else if (t == "money") type = "money";
	else type = "estate";

	estateId = -1;
	cardId = -1;
	money = 0;
	fromPlayerId = -1;
	targetPlayerId = -1;

	txtTemplate = tr("<span style=\"font-weight:bold; color:#008000\">%1</span> gives <span style=\"font-weight:bold; color:#ff8000\">%2</span> to <span style=\"font-weight:bold; color:#000080\">%3</span>");
	connect(m_ui->bRemove, SIGNAL(clicked()), this, SLOT(emitTradeCommand()));
	
	//Under Win StyledPanel does not work
	#if defined(Q_WS_WIN)
	m_ui->frame->setFrameShape(QFrame::Panel);
	#endif

}

TradeItem::~TradeItem() {
	delete m_ui;
}

QString TradeItem::getType() {
	return type;
}

int TradeItem::getFromPlayerId() {
	return fromPlayerId;
}

int TradeItem::getEstateId() {
	return estateId;
}

int TradeItem::getCardId() {
	return cardId;
}

int TradeItem::getMoney() {
	return money;
}

int TradeItem::getTargetPlayerId() {
	return targetPlayerId;
}

void TradeItem::setFromPlayerId(int pid) {
	fromPlayerId = pid;
}

void TradeItem::setEstateId(int eid) {
	estateId = eid;
}

void TradeItem::setCardId(int cid) {
	cardId = cid;
}

void TradeItem::setMoney(int m) {
	money = m;
	tradeName = QString::number(money);
	setText();
}

void TradeItem::setTargetPlayerId(int pid) {
	targetPlayerId = pid;
}

void TradeItem::setFromName(QString name) {
	fromName = name;
	setText();
}

void TradeItem::setTradeName(QString name) {
	tradeName = name;
	setText();
}

void TradeItem::setTargetName(QString name) {
	targetName = name;
	setText();
}

void TradeItem::setText() {
	m_ui->lText->setText(txtTemplate.arg(fromName, tradeName, targetName));
}

void TradeItem::emitTradeCommand() {
	QString cmd = "";
	if (type == "estate") {
		cmd = ".Te"+QString::number(tradeId)+":"+QString::number(estateId)+":-1";
	} else if (type == "card") {
		cmd = ".Tc"+QString::number(tradeId)+":"+QString::number(cardId)+":-1";
	} else if (type == "money") {
		cmd = ".Tm"+QString::number(tradeId)+":"+QString::number(fromPlayerId)+":"+QString::number(targetPlayerId)+":0";
	}
	if (cmd != "") {
		emit(tradeCommand(cmd));
	}
}

void TradeItem::changeEvent(QEvent *e) {
	QWidget::changeEvent(e);
	switch (e->type()) {
		case QEvent::LanguageChange:
			m_ui->retranslateUi(this);
			break;
		default:
			break;
	}
}
