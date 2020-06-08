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

#ifndef TRADE_H
#define TRADE_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QHash>
#include "src/Capi/capigame.h"
#include "tradeitem.h"

namespace Ui {
	class Trade;
}

class Trade : public QWidget {
	Q_OBJECT

	public:
		Trade(int id, QWidget *parent = 0);
		~Trade();
		int getId();
		void setGame(CapiGame* g);
		void setRev(int r);

	private:
		Ui::Trade *m_ui;
		int rev;
		int tradeId;
		int actor; //Playerid oof initiator of the trade
		//QHash<QString, QString> estates;
		//QHash<QString, QString> cards;

		CapiGame* game;
		QList<TradeItem*> trades;
		QHash<QString, QString> accepts;

		TradeItem* getEstateItem(int estateId);
		TradeItem* getCardItem(int cardId);
		TradeItem* getMoneyItem(int fromPlayerId, int toPlayerId);
		void remove(TradeItem* ti);
		void resetAccepts();
		void updateAcceptList();

	public slots:
		void updateTradeActor(int pid);
		void updateEstateTrade(int estateId, int targetId);
		void updateCardTrade(int cardId, int targetId);
		void updateMoneyTrade(int fromId, int money, int targetId);
		void updateTradeAccept(int playerId, bool accept);
		void reject();

	private slots:
		void sendCommand(QString cmd);
		void updateCbEstate(int i);
		void updateCbCard(int i);
		void addEstate();
		void addCard();
		void addMoney();
		void accept();

	protected:
		void changeEvent(QEvent *e);
		void closeEvent(QCloseEvent* e);

	signals:
		void tradeCommand(QString cmd);
};

#endif // TRADE_H
