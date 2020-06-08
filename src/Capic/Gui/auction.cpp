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

#include "auction.h"
#include "ui_auction.h"
#include "playerauction.h"

Auction::Auction(QWidget *parent) : QWidget(parent), m_ui(new Ui::Auction) {
	m_ui->setupUi(this);
	connect(m_ui->bBid, SIGNAL(clicked()), this, SLOT(emitBid()));
	connect(m_ui->spBid, SIGNAL(editingFinished()), this, SLOT(emitBidOnReturnPress()));
}

Auction::~Auction() {
	delete m_ui;
}

void Auction::addPlayerWidget(PlayerAuction* pi) {
	m_ui->bets->addWidget(pi);
}

void Auction::setId(int aid) {
	id = aid;
}

void Auction::setEstateName(QString name) {
	m_ui->title->setText(tr("Auction for: %1").arg(name));
}

void Auction::setMinBid(int minBid) {
	if (m_ui->spBid->value() < minBid)
		m_ui->spBid->setMinimum(minBid);
}

void Auction::setStatusText(QString text) {
	m_ui->text->setText(text);
}

void Auction::resetSpinBox() {
	m_ui->spBid->setMinimum(1);
	m_ui->spBid->setValue(1);
}

void Auction::emitBid() {
	emit(bid(".ab"+QString::number(id)+":"+QString::number(m_ui->spBid->value())));
}

void Auction::emitBidOnReturnPress() {
	if (m_ui->spBid->hasFocus()) emitBid();
}

void Auction::changeEvent(QEvent *e) {
	QWidget::changeEvent(e);
	switch (e->type()) {
		case QEvent::LanguageChange:
			m_ui->retranslateUi(this);
			break;
		default:
			break;
	}
}
