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

#include <QPixmap>
#include <QIcon>
#include <QCloseEvent>
#include "trade.h"
#include "ui_trade.h"
#include "src/Capi/capigame.h"
#include "src/Capi/capiestate.h"

Trade::Trade(int id, QWidget *parent) : QWidget(parent), m_ui(new Ui::Trade) {
        m_ui->setupUi(this);
        tradeId = id;

        connect(m_ui->cbEstate, SIGNAL(currentIndexChanged(int)), SLOT(updateCbEstate(int)));
        connect(m_ui->cbCard, SIGNAL(currentIndexChanged(int)), SLOT(updateCbCard(int)));

        connect(m_ui->bAddEstate, SIGNAL(clicked()), this, SLOT(addEstate()));
        connect(m_ui->bAddCard, SIGNAL(clicked()), this, SLOT(addCard()));
        connect(m_ui->bAddMoney, SIGNAL(clicked()), this, SLOT(addMoney()));

        connect(m_ui->bAccept, SIGNAL(clicked()), this, SLOT(accept()));
        connect(m_ui->bReject, SIGNAL(clicked()), this, SLOT(reject()));
}

Trade::~Trade() {
        delete m_ui;
}

int Trade::getId() {
        return tradeId;
}

void Trade::setGame(CapiGame* g) {
        game = g;
        if (game == 0) return;
        for (int i = 0; i < game->getNumEstates(); i++) {
                CapiEstate* f = game->getEstate(i);
                if (f->getOwner() != 0) {
                        QPixmap p(8, 8);
                        if (f->getColor() != "") {
                                p.fill(f->getColor());
                                m_ui->cbEstate->addItem(QIcon(p), f->getName(), QVariant(f->getId()));
                        } else {
                                m_ui->cbEstate->addItem(f->getName(), QVariant(f->getId()));
                        }
                }
        }

        for (int i = 0; i < game->getNumCards(); i++) {
                CapiCard* c = game->getCardFromList(i);
                if (c->getOwner() != 0) {
                        m_ui->cbCard->addItem(c->getTitle(), QVariant(c->getId()));
                }
        }

        for (int i = 0; i < game->getNumPlayer(); i++) {
                CapiPlayer* p = game->getPlayerFromList(i);
                m_ui->cbEstateFrom->addItem(p->getName(), QVariant(p->getId()));
                m_ui->cbCardFrom->addItem(p->getName(), QVariant(p->getId()));
                m_ui->cbMoneyFrom->addItem(p->getName(), QVariant(p->getId()));
                m_ui->cbEstateTo->addItem(p->getName(), QVariant(p->getId()));
                m_ui->cbCardTo->addItem(p->getName(), QVariant(p->getId()));
                m_ui->cbMoneyTo->addItem(p->getName(), QVariant(p->getId()));
        }

        updateCbEstate(0);
        updateCbCard(0);

}

void Trade::setRev(int r) {
        rev = r;
        resetAccepts();
}

TradeItem* Trade::getEstateItem(int estateId) {
        TradeItem* ti = 0;
        //search and return tradeItem
        for (int i = 0; i < trades.size(); i++) {
                ti = trades.value(i);
                if ((ti->getType() == "estate") && (ti->getEstateId() == estateId)) return ti;
        }

        //If not found: create one
        ti = new TradeItem(tradeId, "estate");
        ti->setEstateId(estateId);
        trades.append(ti);
        connect(ti, SIGNAL(tradeCommand(QString)), this, SLOT(sendCommand(QString)));
        m_ui->layout->insertWidget(m_ui->layout->indexOf(m_ui->anchor), ti);
        return ti;
}

TradeItem* Trade::getCardItem(int cardId) {
        TradeItem* ti = 0;
        //search and return tradeItem
        for (int i = 0; i < trades.size(); i++) {
                ti = trades.value(i);
                if ((ti->getType() == "card") && (ti->getCardId() == cardId)) return ti;
        }

        //If not found: create one
        ti = new TradeItem(tradeId, "card");
        ti->setCardId(cardId);
        trades.append(ti);
        connect(ti, SIGNAL(tradeCommand(QString)), this, SLOT(sendCommand(QString)));
        m_ui->layout->insertWidget(m_ui->layout->indexOf(m_ui->anchor), ti);
        return ti;
}

TradeItem* Trade::getMoneyItem(int fromPlayerId, int targetPlayerId) {
        TradeItem* ti = 0;
        //search and return tradeItem
        for (int i = 0; i < trades.size(); i++) {
                ti = trades.value(i);
                if ((ti->getType() == "money") &&
                    (ti->getFromPlayerId() == fromPlayerId) &&
                    (ti->getTargetPlayerId() == targetPlayerId)) return ti;
        }

        //If not found: create one
        ti = new TradeItem(tradeId, "money");
        ti->setFromPlayerId(fromPlayerId);
        ti->setTargetPlayerId(targetPlayerId);
        trades.append(ti);
        connect(ti, SIGNAL(tradeCommand(QString)), this, SLOT(sendCommand(QString)));
        m_ui->layout->insertWidget(m_ui->layout->indexOf(m_ui->anchor), ti);
        return ti;
}

void Trade::remove(TradeItem* ti) {
        trades.removeAll(ti);
        delete ti;
}

void Trade::resetAccepts() {
        QList<QString> keys = accepts.keys();
        foreach (QString k, keys) {
                accepts[k] = "0";
        }
        updateAcceptList();
}

void Trade::updateAcceptList() {
        if (game == 0) return;
        QString text = tr("Accepts: %1");
        QString accStr = "<span style=\"font-weight:bold\">";
        QList<QString> keys = accepts.keys();
        foreach (QString k, keys) {
                QString val = accepts.value(k);
                QString name = game->getPlayer(k.toInt())->getName();
                QString color = "#800000";
                if (val == "1") color = "#008000";

                accStr += QString("<span style=\"color: %1\">%2</span> ").arg(color, name);
        }
        accStr += "</span>";
        m_ui->lAccepts->setText(text.arg(accStr));
}

void Trade::updateTradeActor(int pid) {
        if (!accepts.contains(QString::number(pid))) accepts.insert(QString::number(pid), "0");
        updateAcceptList();
}

void Trade::updateEstateTrade(int estateId, int targetId) {
        if (game == 0) return;
        if (estateId == -1) return;
        TradeItem* ti = getEstateItem(estateId);
        if (ti == 0) return;

        if (targetId != -1) {
                ti->setTargetPlayerId(targetId);
                ti->setTargetName(game->getPlayer(targetId)->getName());;
                ti->setFromName(game->getEstate(estateId)->getOwner()->getName());
                ti->setTradeName(game->getEstate(estateId)->getName());
        } else {
                remove(ti);
        }
}

void Trade::updateCardTrade(int cardId, int targetId) {
        if (game == 0) return;
        if (cardId == -1) return;
        TradeItem* ti = getCardItem(cardId);
        if (ti == 0) return;

        if (targetId != -1) {
                ti->setTargetPlayerId(targetId);
                ti->setTargetName(game->getPlayer(targetId)->getName());
                ti->setFromName(game->getCard(cardId)->getOwner()->getName());
                ti->setTradeName(game->getCard(cardId)->getTitle());
        } else {
                remove(ti);
        }
}

void Trade::updateMoneyTrade(int fromId, int money, int targetId) {
        if (game == 0) return;
        if (fromId == -1) return;
        if (targetId == -1) return;

        TradeItem* ti = getMoneyItem(fromId, targetId);
        if (ti == 0) return;

        if (money > 0) {
                ti->setTargetName(game->getPlayer(targetId)->getName());
                ti->setFromName(game->getPlayer(fromId)->getName());
                ti->setMoney(money);
        } else {
                remove(ti);
        }
}

void Trade::updateTradeAccept(int playerId, bool accept) {
        CapiPlayer* p = game->getPlayer(playerId);
        if (p == 0) return;

        if (accepts.contains(QString::number(playerId))) {
                if (!accept) accepts[QString::number(playerId)] = "0";
                else accepts[QString::number(playerId)] = "1";
        } else {
                if (!accept) accepts.insert(QString::number(playerId), "0");
                else accepts.insert(QString::number(playerId), "1");
        }
        updateAcceptList();
}

void Trade::updateCbEstate(int i) {
        if (game == 0) return;
        int fid = m_ui->cbEstate->itemData(i).toInt();
        CapiEstate* f = game->getEstate(fid);
        int cbEstateInd = -1;
        if (f->getOwner() != 0) cbEstateInd = m_ui->cbEstateFrom->findData(QVariant(f->getOwner()->getId()));
        m_ui->cbEstateFrom->setCurrentIndex(cbEstateInd);
}

void Trade::updateCbCard(int i) {
        if (game == 0) return;
        int cid = m_ui->cbCard->itemData(i).toInt();
        CapiCard* c = game->getCard(cid);
        int cbCardInd = -1;
        if (c->getOwner() != 0) cbCardInd = m_ui->cbCardFrom->findData(QVariant(c->getOwner()->getId()));
        m_ui->cbCardFrom->setCurrentIndex(cbCardInd);
}

void Trade::addEstate() {
        bool ok;
        int fid = m_ui->cbEstate->itemData(m_ui->cbEstate->currentIndex()).toInt(&ok);
        if(!ok) return;

        int pid = m_ui->cbEstateTo->itemData(m_ui->cbEstateTo->currentIndex()).toInt(&ok);
        if (!ok) return;

        sendCommand(".Te"+QString::number(tradeId)+":"+QString::number(fid)+":"+QString::number(pid));
}

void Trade::addCard() {
        bool ok;
        int cid = m_ui->cbCard->itemData(m_ui->cbCard->currentIndex()).toInt(&ok);
        if(!ok) return;

        int pid = m_ui->cbCardTo->itemData(m_ui->cbCardTo->currentIndex()).toInt(&ok);
        if (!ok) return;

        sendCommand(".Tc"+QString::number(tradeId)+":"+QString::number(cid)+":"+QString::number(pid));
}

void Trade::addMoney() {
        bool ok;

        int pfid = m_ui->cbMoneyFrom->itemData(m_ui->cbMoneyFrom->currentIndex()).toInt(&ok);
        if (!ok) return;

        int ptid = m_ui->cbMoneyTo->itemData(m_ui->cbMoneyTo->currentIndex()).toInt(&ok);
        if(!ok) return;

        int money = m_ui->spMoney->value();

        sendCommand(".Tm"+QString::number(tradeId)+":"+QString::number(pfid)+":"+QString::number(ptid)+":"+QString::number(money));
}

void Trade::accept() {
        sendCommand(".Ta"+QString::number(tradeId)+":"+QString::number(rev));
}

void Trade::reject() {
        sendCommand(".Tr"+QString::number(tradeId));
}

void Trade::sendCommand(QString cmd) {
        emit(tradeCommand(cmd));
}

void Trade::changeEvent(QEvent *e) {
        QWidget::changeEvent(e);
        switch (e->type()) {
        case QEvent::LanguageChange:
                m_ui->retranslateUi(this);
                break;
        default:
                break;
        }
}

void Trade::closeEvent(QCloseEvent* e) {
        reject();
        e->accept();
}
