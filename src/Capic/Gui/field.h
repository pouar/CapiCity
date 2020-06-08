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

#ifndef FIELD_H
#define FIELD_H

#include <QTextBrowser>
#include <QWidget>
#include <QStackedLayout>
#include <QAction>
#include <QMenu>
#include <QImage>
#include "src/Capi/capigame.h"
#include "src/Capi/capiestate.h"
#include "src/Capic/capiclientestate.h"
#include "cmdtoolbutton.h"
#include "auction.h"
#include "capiclientplayer.h"
#include "trade.h"

namespace Ui {
	class Field;
}

class Field : public QWidget {
	Q_OBJECT

	public:
		Field(QWidget* parent=0);
		void setGame(CapiGame* g);
		void setClientId(int id);
		~Field();
		Auction* auction;
		CapiPlayer* getShownPlayer();
		void addTrade(Trade* t);
		void setCapid(bool c);

	private:
		Ui::Field *ui;
		CapiGame* game;
		int clientId;
		QRect getAvatarRect(int pos, bool bigger=false);
		QRect getEstateRect(int pos);
		QPoint interpolate(QPoint p1, QPoint p2, int offset);
		QStackedLayout* stack;
		QTextBrowser* text;

		QList<QString> gameMessages;
		bool txtColor;
		QList<CmdToolButton*> buttons;
		CapiClientEstate* selectedEstate;
		CapiClientEstate* getEstate(int x, int y);
		bool capid;
		void deleteMenu();

		QAction* acSell;
		QAction* acMortage;
		QAction* acUnmortage;
		QAction* acBuyHouse;
		QAction* acSellHouse;
		QAction* acBuyRow;
		QAction* acSellRow;

		QMenu* menu;

		int scroll;
		int N;
		int w;
		int h;
		int s;
		int horEstateWidth;
		int vertEstateHeight;
		int horEstateHeight;
		int vertEstateWidth;
		int cornerSize;
		int min(int a, int b) {if (a < b) return a; return b; }
		CapiPlayer* shownPlayer;
		QString search;
		QImage* imgBoard;
	
	protected:
		void changeEvent(QEvent* e);
		void mousePressEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void wheelEvent(QWheelEvent* event);
		void paintEvent(QPaintEvent* pe);
		void resizeEvent(QResizeEvent* e);

	private slots:
		void emitButtonClick(QString cmd);
		void menuClick(QAction* a);

	public slots:
		void showEstate(int fid);
		void appendLine(QString txt);
		void addButton(QString text, QString cmd);
		void clearText();
		void clearButtons();
		void addPlayerAuction(CapiClientPlayer* p);

		void showAuction();
		void updatAuctionId(int aid);
		void updateAuctionField(int fid);
		void bidUpdate(int pid, int bid);
		void updateAuctionStatus(int status);
		
		void setShownPlayer(int pid);
		void setSearch(QString s);
		void showSearch();
		void clearSearch();
		void renderBoard();
		void renderEstate(int eid);
		void renderPlayer(int pid);
		void playerOffsetChanged(CapiClientPlayer* p);
		void updateBoard();

	signals:
		void buttonClicked(QString cmd);
		void fieldSelected(CapiEstate* f);
		void rowBuyed(int gid);
		void rowSelled(int gid);

};

#endif // FIELD_H
