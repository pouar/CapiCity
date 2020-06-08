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

#ifndef TRADEITEM_H
#define TRADEITEM_H

#include <QtWidgets/QWidget>

namespace Ui {
        class TradeItem;
}

class TradeItem : public QWidget {
        Q_OBJECT

public:
        TradeItem(int tid, QString type="estate", QWidget *parent = 0);
        ~TradeItem();

        QString getType();
        int getFromPlayerId();
        int getEstateId();
        int getCardId();
        int getMoney();
        int getTargetPlayerId();

        void setFromPlayerId(int pid);
        void setEstateId(int eid);
        void setCardId(int cid);
        void setMoney(int m);
        void setTargetPlayerId(int pid);

        void setFromName(QString name);
        void setTradeName(QString name);
        void setTargetName(QString name);

private:
        Ui::TradeItem *m_ui;
        int tradeId;
        QString type; //Can be "estate", "card", "or money"
        int fromPlayerId;
        int cardId; //Only from interest, if type == "card"
        int estateId; //Only from interest, if type == "estate"
        int money; //Only from interest, if type == "money"

        QString fromName;
        QString tradeName;
        QString targetName;
        QString txtTemplate;
        int targetPlayerId;

        void setText();

private slots:
        void emitTradeCommand();

protected:
        void changeEvent(QEvent *e);

signals:
        void tradeCommand(QString cmd);
};

#endif // TRADEITEM_H
