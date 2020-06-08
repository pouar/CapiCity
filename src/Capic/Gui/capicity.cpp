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

#include <QDesktopWidget>
#include <QMessageBox>
#include <QTableWidget>
#include <QCloseEvent>
#include <QDateTime>
#include <QCompleter>
#include <QLabel>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>

#include "capicity.h"
#include "ui_capicity.h"
#include "about.h"
#include "src/Capi/capiplayer.h"
#include "src/defines.h"

CapiCity::CapiCity(QWidget *parent)
        :QMainWindow(parent), ui(new Ui::CapiCity) {
        ui->setupUi(this);

        serverChooser = new ServerChooser(this);
        setClient(new CapiClient(this));
        server = 0;

        resize(s.value(CONFIG_WINDOW_SIZE, QVariant(size())).toSize());
        restoreState(s.value(CONFIG_WINDOW_LAYOUT, "").toByteArray(), 3);
        locked = s.value(CONFIG_WINDOW_LOCKED, CONFIG_WINDOW_LOCKED_DEFAULT).toBool();
        ui->acAutoRoll->setChecked(s.value(CONFIG_GAME_AUTOROLL, CONFIG_GAME_AUTOROLL_DEFAULT).toBool());
        setAutoroll();

        bool vis = s.value(CONFIG_WINDOW_MENUBAR, CONFIG_WINDOW_MENUBAR_DEFAULT).toBool();
        setMenuBarVisible(vis);

        //Set Table with games as central widget
        newGames();
        newGameConfig();
        newField();
        setGame(0);

        layout = new QStackedLayout();
        layout->setObjectName("Stack");
        layout->addWidget(games);
        layout->addWidget(gameConfig);
        layout->addWidget(field);
        layout->addWidget(serverChooser);
        ui->center->addLayout(layout);
        layout->setCurrentIndex(3);

        eventVisible = false;
        chatColor = false;
        eventColor = false;

        this->textBackgr = palette().brush(QPalette::Base).color();
        this->textAltBackgr = palette().brush(QPalette::AlternateBase).color();

        config = new Config(this);

        //Connect signals

        //ServerChooser:
        connect(serverChooser, SIGNAL(serverChoosen(QString,int)), client, SLOT(connectToHost(QString,int)));
        connect(serverChooser, SIGNAL(localServerChoosen()), this, SLOT(startServer()));

        //Config
        connect(config, SIGNAL(saved(QString, QString)), this, SLOT(setData(QString, QString)));
        connect(config, SIGNAL(saved(QString, bool)), this, SLOT(setData(QString, bool)));

        //Menu
        ui->acViewPlayers->setData(QVariant("player"));
        ui->acViewPlayers->setChecked(ui->dockPlayers->isVisibleTo(this));
        ui->acViewTurn->setData(QVariant("turn"));
        ui->acViewTurn->setChecked(ui->dockTurn->isVisibleTo(this));
        ui->acViewMessages->setData(QVariant("messages"));
        ui->acViewMessages->setChecked(ui->dockEvents->isVisibleTo(this));
        ui->acViewChat->setData(QVariant("chat"));
        ui->acViewChat->setChecked(ui->dockChat->isVisibleTo(this));
        ui->acLockLayout->setData(QVariant("lock"));
        ui->acLockLayout->setChecked(locked);

        //Game
        connect(ui->acSearch, SIGNAL(triggered()), field, SLOT(showSearch()));
        connect(ui->acLeaveGame, SIGNAL(triggered()), this, SLOT(leaveGame()));
        connect(ui->acDisconnect, SIGNAL(triggered()), this, SLOT(disconnectFromServer()));
        connect(ui->acExitCapiCity, SIGNAL(triggered()), this, SLOT(exit()));

        //Turn
        connect(ui->acRoll,SIGNAL(triggered()), this, SLOT(roll()));
        connect(ui->acAutoRoll, SIGNAL(triggered()), this, SLOT(setAutoroll()));
        connect(ui->acBuy, SIGNAL(triggered()), this, SLOT(buy()));
        connect(ui->acAuction, SIGNAL(triggered()), this, SLOT(auction()));
        connect(ui->acEndTurn, SIGNAL(triggered()), this, SLOT(endTurn()));

        connect(ui->acJailRoll, SIGNAL(triggered()), this, SLOT(jailRoll()));
        connect(ui->acJailCard, SIGNAL(triggered()), this, SLOT(jailCard()));
        connect(ui->acJailPay, SIGNAL(triggered()), this, SLOT(jailPay()));

        //Config
        connect(ui->acConfig, SIGNAL(triggered()), config, SLOT(show()));
        connect(ui->menuView, SIGNAL(triggered(QAction*)), this, SLOT(changeView(QAction*)));
        connect(ui->acShowMenuBar, SIGNAL(triggered()), this, SLOT(showMenuBar()));

        //Help
        connect(ui->acAbout, SIGNAL(triggered()), this, SLOT(showAbout()));

        //Turn
        ui->bRoll->cmd = ".r";
        ui->bBuy->cmd = ".eb";
        ui->bAuction->cmd = ".ea";
        ui->bEndturn->cmd = ".E";
        connect(ui->bRoll, SIGNAL(cmdClicked(QString)), client, SLOT(send(QString)));

        connect(ui->bBuy, SIGNAL(cmdClicked(QString)), client, SLOT(send(QString)));
        connect(ui->bBuy, SIGNAL(clicked()), field, SLOT(clearButtons()));

        connect(ui->bAuction, SIGNAL(cmdClicked(QString)), client, SLOT(send(QString)));
        connect(ui->bAuction, SIGNAL(clicked()), field, SLOT(clearButtons()));

        connect(ui->bEndturn, SIGNAL(cmdClicked(QString)), client, SLOT(send(QString)));
        connect(ui->bEndturn, SIGNAL(clicked()), field, SLOT(clearButtons()));

        //Jail
        ui->bJailRoll->cmd = ".jr";
        ui->bCard->cmd = ".jc";
        ui->bPay->cmd = ".jp";
        connect(ui->bJailRoll, SIGNAL(cmdClicked(QString)), client, SLOT(send(QString)));
        connect(ui->bJailRoll, SIGNAL(clicked()), field, SLOT(clearButtons()));

        connect(ui->bCard, SIGNAL(cmdClicked(QString)), client, SLOT(send(QString)));
        connect(ui->bCard, SIGNAL(clicked()), field, SLOT(clearButtons()));

        connect(ui->bPay, SIGNAL(cmdClicked(QString)), client, SLOT(send(QString)));
        connect(ui->bPay, SIGNAL(clicked()), field, SLOT(clearButtons()));

        //chat
        connect(ui->bSend, SIGNAL(clicked()), this, SLOT(sendMsg()));
        connect(ui->tfMess, SIGNAL(returnPressed()), this, SLOT(sendMsg()));
        connect(ui->bEmos, SIGNAL(clicked()), this, SLOT(showEmoMenu()));

        //news
        connect(ui->dockChat, SIGNAL(visibilityChanged(bool)), this, SLOT(normalChatTitle(bool)));
        connect(ui->dockEvents, SIGNAL(visibilityChanged(bool)), this, SLOT(normalEventTitle(bool)));

        if (s.value(CONFIG_CONNECTION_SERVER, QVariant("")) != "") {
                if(s.value(CONFIG_CONNECTION_LASTMESSAGE, QVariant(0)).toUInt()+180 > QDateTime::currentDateTime().toTime_t()) {
                        if (QMessageBox::question(this, tr("Reconnect"), tr("Capi City seems to be chrashed recently. Should be tried to reconnect to the last game?"),
                                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                                client->reconnect();
                        }
                }
        }

        notifier = new Notifier();

        //Prepare Emoicons
        emos = new QMap<QString, QString>();
        emos->insert(":arrow:", "icon_arrow.gif");
        emos->insert(":D",      "icon_biggrin.gif");
        emos->insert(":~",      "icon_confused.gif");
        emos->insert("8)",      "icon_cool.gif");
        emos->insert(":cry:",   "icon_cry.gif");
        emos->insert(":shock:", "icon_eek.gif");
        emos->insert(":evil:",  "icon_evil.gif");
        emos->insert(":!",      "icon_exclaim.gif");
        emos->insert(":(",      "icon_frown.gif");
        emos->insert(":idea:",  "icon_idea.gif");
        emos->insert(":lol:",   "icon_lol.gif");
        emos->insert(":x",      "icon_mad.gif");
        emos->insert(":gD",     "icon_mrgreen.gif");
        emos->insert(":|",      "icon_neutral.gif");
        emos->insert(":?",      "icon_question.gif");
        emos->insert(":P",      "icon_razz.gif");
        emos->insert(":oops:",  "icon_redface.gif");
        emos->insert(":roll:",  "icon_rolleyes.gif");
        emos->insert(":sad:",   "icon_sad.gif");
        emos->insert(":)",      "icon_smile.gif");
        emos->insert(":0",      "icon_surprised.gif");
        emos->insert(":evil):", "icon_twisted.gif");
        emos->insert(";)",      "icon_wink.gif");

        //Create emoicon Menu
        emoMenu = new QMenu(this);
        foreach (QString s, emos->keys()) {
                QAction* emoAction = emoMenu->addAction(QIcon(QApplication::applicationDirPath()+"/images/smilies/"+emos->value(s)), s);
                emoAction->setData(QVariant(s));
        }
        connect(emoMenu, SIGNAL(triggered(QAction*)), this, SLOT(addEmo(QAction*)));

        //Create Dummy titlebar widgets for the docks
        this->dummyDockPlayersTitlebarWidget  = new QWidget();
        this->dummyDockTurnTitlebarWidget     = new QWidget();
        this->dummyDockMessagesTitlebarWidget = new QWidget();
        this->dummyDockChatTitlebarWidget     = new QWidget();

        titlebarWidgets.insert(ui->dockPlayers, this->dummyDockPlayersTitlebarWidget);
        titlebarWidgets.insert(ui->dockTurn, this->dummyDockTurnTitlebarWidget);
        titlebarWidgets.insert(ui->dockEvents, this->dummyDockMessagesTitlebarWidget);
        titlebarWidgets.insert(ui->dockChat, this->dummyDockChatTitlebarWidget);

        setLocked();
        //Menu Actions
        //Game-menu
        addAction(ui->acSearch);
        addAction(ui->acLeaveGame);
        addAction(ui->acDisconnect);
        addAction(ui->acExitCapiCity);

        //Turn-menu
        addAction(ui->acRoll);
        addAction(ui->acBuy);
        addAction(ui->acAuction);
        addAction(ui->acEndTurn);
        addAction(ui->acJailRoll);
        addAction(ui->acJailCard);
        addAction(ui->acJailPay);

        //Settings-menu
        addAction(ui->acViewPlayers);
        addAction(ui->acViewTurn);
        addAction(ui->acViewMessages);
        addAction(ui->acViewChat);
        addAction(ui->acLockLayout);
        addAction(ui->acConfig);
        addAction(ui->acShowMenuBar);

        //Help-menu
        addAction(ui->acAbout);
}

CapiCity::~CapiCity() {
        delete ui;
}

CapiGame* CapiCity::getGame() {
        return game;
}

void CapiCity::setGame(CapiGame* g) {
        game = g;
        if (field == 0) return;
        field->setGame(g);
        field->setClientId(client->getClientPlayerId());
        foreach(Trade* t, trades) {
                t->setGame(g);
        }
}

void CapiCity::roll() {
        if (client == 0) return;
        client->send(".r");
        notifier->hideNotify();
}

void CapiCity::buy() {
        if (client == 0) return;
        client->send(".eb");
        field->clearButtons();
}

void CapiCity::auction() {
        if (client == 0) return;
        client->send(".ea");
        field->clearButtons();
}

void CapiCity::endTurn() {
        if (client == 0) return;
        client->send(".E");
        field->clearButtons();
}

void CapiCity::jailRoll() {
        if (client == 0) return;
        client->send(".jr");
        field->clearButtons();
}

void CapiCity::jailCard() {
        if (client == 0) return;
        client->send(".jc");
        field->clearButtons();
}

void CapiCity::jailPay() {
        if (client == 0) return;
        client->send(".jp");
        field->clearButtons();
}

void CapiCity::buyRow(int gr) {
        CapiGroup* g = game->getGroup(gr);

        //Check if player can pay this
        int neededMoney = 0;
        for (int i = 0; i < g->getNumEstates(); i++) {
                neededMoney += g->getEstate(i)->getHousePrice();
        }
        CapiPlayer* pl = game->getPlayer(client->getClientPlayerId());
        if (neededMoney > pl->getMoney()) {
                QMessageBox::information(this, "Capi City", tr("Cannot buy a houserow, because you don't have enough money. You need a least %1 to buy one.").arg(QString::number(neededMoney)));
                return;
        }

        //Buy them, if possible
        for (int i = 0; i < g->getNumEstates(); i++) {
                send(".hb"+QString::number(g->getEstate(i)->getId()));
        }
}

void CapiCity::sellRow(int gr) {
        CapiGroup* g = game->getGroup(gr);
        for (int i = 0; i < g->getNumEstates(); i++) {
                send(".hs"+QString::number(g->getEstate(i)->getId()));
        }
}

void CapiCity::sendMsg() {
        if (client == 0) return;
        client->send(ui->tfMess->text());
        ui->tfMess->setText("");
}

void CapiCity::normalChatTitle(bool vis) {
        chatVisible = vis;
        if (!vis) return;
        ui->dockChat->setWindowTitle(tr("Chat"));
}

void CapiCity::normalEventTitle(bool vis) {
        eventVisible = vis;
        if (!vis) return;
        ui->dockEvents->setWindowTitle(tr("Messages"));
}

void CapiCity::setClient(CapiClient* c) {
        client = c;
        connect(client, SIGNAL(connected()), this, SLOT(setCurGame()));

        connect(client, SIGNAL(boardUpdate()), this, SLOT(drawField()));
        connect(client, SIGNAL(playerAdded(CapiClientPlayer*)), this, SLOT(addToList(CapiClientPlayer*)));
        connect(client, SIGNAL(chatMessage(int, QString)), this, SLOT(showChat(int, QString)));
        connect(client, SIGNAL(serverMessage(QString, QString)), this, SLOT(showEvent(QString, QString)));
        connect(client, SIGNAL(playerUpdated()), this, SLOT(playerUpdate()));
        connect(client, SIGNAL(newGameList()), this, SLOT(updateGameList()));
        connect(client, SIGNAL(curGameChanged()), this, SLOT(setCurGame()));
        connect(client, SIGNAL(statusChanged(QString)), this, SLOT(setCurGame()));
        connect(client, SIGNAL(gameMasterChanged(int,int)), this, SLOT(setOptionsEnabled(int,int)));

        connect(client, SIGNAL(tradeUpdate(QString,int,int,int)), this, SLOT(tradeUpdate(QString,int,int,int)));
        connect(client, SIGNAL(tradePlayerUpdate(int,int,bool)), this, SLOT(tradePlayerUpdate(int,int,bool)));
        connect(client, SIGNAL(tradeEstateUpdate(int,int,int)), this, SLOT(tradeEstateUpdate(int,int,int)));
        connect(client, SIGNAL(tradeCardUpdate(int,int,int)), this, SLOT(tradeCardUpdate(int,int,int)));
        connect(client, SIGNAL(tradeMoneyUpdate(int,int,int,int)), this, SLOT(tradeMoneyUpdate(int,int,int,int)));

        connect(client, SIGNAL(connectionLost()), this, SLOT(forceDisconnect()));
        connect(client, SIGNAL(gotHost(QString)), this, SLOT(showHost(QString)));

        connect(client, SIGNAL(playerMoved(CapiClientPlayer*,int)), this, SLOT(animateMovment(CapiClientPlayer*, int)));
}

void CapiCity::playerUpdate() {
        CapiPlayer* p = client->getPlayer(client->getClientPlayerId());
        if (p == 0) return;
        bool bRollActive = ui->bRoll->isEnabled();
        ui->bRoll->setEnabled((client->getCurGameId() != -1) && p->getHasTurn() && p->getCanRoll());
        ui->acRoll->setEnabled(ui->bRoll->isEnabled());
        if (!bRollActive &&
            ui->bRoll->isEnabled() &&
            !s.value(CONFIG_GAME_AUTOROLL, CONFIG_GAME_AUTOROLL_DEFAULT).toBool() &&
            s.value(CONFIG_WINDOW_SHOWNOTIFICATION, CONFIG_WINDOW_SHOWNOTIFICATION_DEFAULT).toBool()) notifier->showNotify(6);

        ui->bAuction->setEnabled((client->getCurGameId() != -1) && p->getHasTurn() && p->getCanAuction());
        ui->acAuction->setEnabled(ui->bAuction->isEnabled());

        ui->bBuy->setEnabled((client->getCurGameId() != -1) && p->getHasTurn() && p->getCanBuyEstate());
        ui->acBuy->setEnabled(ui->bBuy->isEnabled());

        ui->bEndturn->setEnabled(p->getHasTurn() && !p->getInJail() && p->getCanBuyEstate() && !p->getCanAuction());
        ui->acEndTurn->setEnabled(ui->bEndturn->isEnabled());

        //Jail action
        ui->bJailRoll->setEnabled(p->getHasTurn() && p->getInJail());
        ui->acJailRoll->setEnabled(ui->bJailRoll->isEnabled());

        ui->bCard->setEnabled(p->getHasTurn() && p->getInJail() && p->getCanUseCard());
        ui->acJailCard->setEnabled(ui->bCard->isEnabled());

        ui->bPay->setEnabled(p->getHasTurn() && p->getInJail());
        ui->acJailPay->setEnabled(ui->bPay->isEnabled());

        ui->menuJail->setEnabled(ui->acJailRoll->isEnabled() || ui->acJailCard->isEnabled() || ui->acJailPay->isEnabled());
}

void CapiCity::updateGameList() {
        if (games == 0) return;
        if (client == 0) return;
        if (client->getCurGameId() != -1) return;
        games->clearContents();
        games->setColumnCount(6);
        games->setHorizontalHeaderLabels(QStringList(tr("Name")) << "" <<tr("Description") << tr("Players") << "joinCmd");
        games->setVerticalHeaderLabels(QStringList());

        games->setRowCount(0);
        int r = 0;
        for (int i = 0; i < client->getNumGames(); i++) {
                CapiClientGame* g = client->getGameFromList(i);
                if (!g->getListed()) continue;
                games->insertRow(r);

                CapiPlayer* p = g->getMaster();
                QString name = tr("Unknown");
                if (p != 0) name = p->getName();
                QTableWidgetItem* it = new QTableWidgetItem(tr("Join %1 from %2").arg(g->getName()).arg(name));
                bool cbj = g->getCanBeJoined();
                if (cbj) it->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                else it->setFlags(Qt::ItemIsEnabled);
                games->setItem(r, 0, it);

                it = new QTableWidgetItem(tr("Spectate"));
                it->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                games->setItem(r, 1, it);

                it = new QTableWidgetItem(g->getDesc());
                it->setFlags(Qt::ItemIsEnabled);
                games->setItem(r, 2, it);

                it = new QTableWidgetItem(QString::number(g->getNumPlayer()));
                it->setFlags(Qt::ItemIsEnabled);
                games->setItem(r, 3, it);

                it = new QTableWidgetItem(".gj"+QString::number(g->getId()));
                it->setFlags(Qt::ItemIsEnabled);
                games->setItem(r, 4, it);

                it = new QTableWidgetItem(".gS"+QString::number(g->getId()));
                it->setFlags(Qt::ItemIsEnabled);
                games->setItem(r, 5, it);

                r++;
        }

        for (int i = 0; i < client->getNumTemplates(); i++) {
                games->insertRow(r);

                QTableWidgetItem* it = new QTableWidgetItem(tr("Start new %1").arg(client->getTemplateFromList(i)->getName()));
                it->setForeground(QColor(0,0,0));
                it->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                it->setBackground(QBrush(QColor(192,255,192)));
                games->setItem(r, 0, it);

                it = new QTableWidgetItem(client->getTemplateFromList(i)->getDesc());
                it->setFlags(Qt::ItemIsEnabled);
                games->setItem(r, 2, it);

                it = new QTableWidgetItem("");
                it->setFlags(Qt::ItemIsEnabled);
                games->setItem(r, 3, it);

                it = new QTableWidgetItem(".gn"+client->getTemplateFromList(i)->getType());
                it->setFlags(Qt::ItemIsEnabled);
                games->setItem(r, 4, it);

                r++;
        }
        games->hideColumn(1); //Hide specation option, until there is a working specating mode by Monopd
        games->hideColumn(4);
        games->hideColumn(5);
        games->resizeColumnsToContents();
}

void CapiCity::showChat(int pid, QString msg) {
        CapiClientPlayer* p;
        if (client->getCurGameId() != -1) {
                p = (CapiClientPlayer*) game->getPlayer(pid);
        }
        QString bg = textBackgr.name();

        if (chatColor) {
                bg = textAltBackgr.name();
        }
        chatColor = !chatColor;

        QString name = tr("Unknown Player");
        QString pColor = palette().color(QPalette::Text).name();

        p = client->getPlayer(pid);
        if (p != 0) {
                name = p->getName();
                pColor = p->getChatColor();
        }
        if (name.size() > 15)
                name = name.left(6)+"..."+name.right(6);

        QString emocMsg = msg.replace("<", "&#60;");
        emocMsg = emocMsg.replace(">", "&#62;");

        if (s.value(CONFIG_CHAT_SHOWEMOS, CONFIG_CHAT_SHOWEMOS_DEFAULT).toBool()) emocMsg = emoicons(msg);

        QStringList words = emocMsg.split(" ");
        QString showMsg = "";
        foreach (QString s, words) {
                if (s.startsWith("www.")) {
                        showMsg += QString("<a href=\"http://%1\">%1</a> ").arg(s);
                } else if (s.startsWith("http://")) {
                        showMsg += QString("<a href=\"%1\">%1</a> ").arg(s);
                } else if (s.startsWith("https://")) {
                        showMsg += QString("<a href=\"%1\">%1</a> ").arg(s);
                } else if (s.startsWith("ftp://")) {
                        showMsg += QString("<a href=\"%1\">%1</a> ").arg(s);
                } else {
                        showMsg += s+" ";
                }
        }
        showMsg.remove(showMsg.size()-1,1);

        //Testing using tables to have a better structure - Does not work as hoped right now. ...
        chatHtml += QString("<tr style=\"background:%1\"><td style=\"text-align:right; vertical-align:top; color:%2; min-width:10%;\">%3:&nbsp;</td><td style=\"vertical-align:top; word-wrap=break-word;\">%4</td></tr>").arg(bg, pColor, name, showMsg);
        ui->chat->setHtml("<table border=\"0\" style=\"width=100%; table-layout: fixed;\" cellspacing=\"0\" cellpadding=\"0\">"+chatHtml+"</table><a name=\"end\" style=\"font-size:1px\">&nbsp;</a>");

        //... So still use "classic" style
        //chatHtml = chatHtml+"<p style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; background:"+bg+"\"><span style=\"color:"+pColor+"\">"+name+":</span> "+showMsg+"</p>";
        //ui->chat->setHtml(chatHtml+"<a name=\"end\" style=\"font-size:1px\">&nbsp;</a>");

        ui->chat->scrollToAnchor("end");

        if (!chatVisible) {
                ui->dockChat->setWindowTitle("*Chat*");
        }
}

void CapiCity::showEvent(QString type, QString msg) {
        //info, error, standby, and startgame
        QColor txtColor = palette().color(QPalette::Text);
        int maxVal = (txtColor.value()+64)%256;
        int minVal = (txtColor.value()+192)%256;
        int val = maxVal;
        if (minVal > val) val = minVal;

        if (type == "info"){}
        if (type == "error") {
                txtColor.setHsv(0,255,val);
        }
        if (type == "standby") {
                txtColor.setHsv(120,255,val);
        }
        if (type == "startgame") {
                txtColor.setHsv(240,255,val);
        }
        QString color = txtColor.name();
        QString bg = textBackgr.name();

        if (eventColor) {
                bg = textAltBackgr.name();
        }

        eventHtml = eventHtml+"<p style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; background:"+bg+"\"><span style=\"color:"+color+"\">"+msg+"</span></p>";
        ui->events->setHtml(eventHtml+"<a name=\"end\" style=\"font-size:1px\">&nbsp;</a>");
        ui->events->scrollToAnchor("end");

        eventColor = !eventColor;

        if (!eventVisible) {
                ui->dockEvents->setWindowTitle(tr("*Messages*"));
        }
}

void CapiCity::setCurGame() {
        int gid = client->getCurGameId();
        field->updateAuctionStatus(3);
        //field->clearText();
        //field->clearButtons();

        if (gid == -1) {
                setGame(0);
                field->clearSearch();
                ui->bRoll->setEnabled(false);
                ui->bBuy->setEnabled(false);
                ui->bAuction->setEnabled(false);
                ui->bEndturn->setEnabled(false);
                layout->setCurrentIndex(0);
                for (int i = 0; i < client->getNumPlayers(); i++) {
                        client->getPlayerFromList(i)->playerWidget->setShowButtonVisible(false);
                        client->getPlayerFromList(i)->playerWidget->resetButtons();
                        client->getPlayerFromList(i)->playerWidget->setTradeButtonVisible(false);
                        client->getPlayerFromList(i)->playerWidget->setEstateButtonVisible(false);
                        client->getPlayerFromList(i)->playerWidget->setMoneyVisible(false);
                        client->getPlayerFromList(i)->setHasTurn(false);
                }
                field->setShownPlayer(-1);
        } else {
                setGame(client->getGame(gid));
                //Set gameconfig as central widget
                if (game->getStatus() == "config") {
                        layout->setCurrentIndex(1);
                        gameConfig->setDesc(game->getDesc());
                        if (game->getMaster() != 0) gameConfig->setOptionsEnabled(game->getMaster()->getId() == client->getClientPlayerId());
                } else if (game->getStatus() == "run") {
                        gameConfig->removeAllOptions();
                        layout->setCurrentIndex(2);
                        field->clearText();
                        field->clearButtons();

                        /*//add a completer to tfMess
                          QStringList sl;
                          for (int i = 0; i < game->getNumPlayer(); i++)
                          sl.append(game->getPlayerFromList(i)->getName());
                          int n = game->getNumEstates();
                          for (int i = 0; i < n; i++) {
                          CapiEstate* e = game->getEstate(i);
                          if (e->getBuyable()) sl.append(e->getName());
                          }

                          QCompleter* c = new QCompleter(sl);
                          ui->tfMess->setCompleter(c);
                        */
                        for (int i = 0; i < client->getNumPlayers(); i++) {
                                CapiClientPlayer* pl = client->getPlayerFromList(i);
                                if (!pl->getSpectator()) {
                                        pl->playerWidget->setShowButtonVisible(true);
                                        if (pl->getId() != client->getClientPlayerId()) pl->playerWidget->setTradeButtonVisible(true);
                                        pl->playerWidget->setEstateButtonVisible(true);
                                        pl->playerWidget->setMoneyVisible(true);
                                }
                        }
                        field->setShownPlayer(-1);
                        field->updateBoard();
                }
        }
        ui->acSearch->setEnabled(layout->currentIndex() == 2);
}

void CapiCity::setOptionsEnabled(int gid, int newMaster) {
        if (gid == -1) return;
        if (client->getCurGameId() == -1) return;
        if (gid == client->getCurGameId()) gameConfig->setOptionsEnabled(newMaster == client->getClientPlayerId());
}

void CapiCity::startGame(int r, int c) {
        if (games == 0) return;
        QTableWidgetItem* it = 0;
        if (c == 0)
                it = games->item(r, 4);
        else if (c == 1)
                it = games->item(r, 5);

        if (it != 0) send(it->text());
}

void CapiCity::addToList(CapiClientPlayer* p) {
        if (p->getId() == client->getClientPlayerId()) {
                ui->playerInfos->insertWidget(0, p->playerWidget);
        } else {
                ui->playerInfos->insertWidget(ui->playerInfos->indexOf(ui->anchor), p->playerWidget);
        }
        ui->playerInfos->update();
        field->addPlayerAuction(p);

        connect(p->playerWidget, SIGNAL(command(QString)), client, SLOT(send(QString)));
        connect(p->playerWidget, SIGNAL(showed(int)), this, SLOT(setShownPlayer(int)));
}

void CapiCity::newGames() {
        games = new QTableWidget(this);
        games->setObjectName("Lobby");
        games->setHorizontalHeaderLabels(QStringList(tr("Name")) << tr("Description") << tr("Players"));

        connect(games, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(startGame(int, int)));
}

void CapiCity::newGameConfig() {
        gameConfig = new GameConfig(this);
        gameConfig->setObjectName("Config");
        if (client == 0) return;

        connect(client, SIGNAL(configUpdate(QString,QString,QString, QString)), gameConfig, SLOT(configUpdate(QString, QString, QString, QString)));

        connect(gameConfig, SIGNAL(optionChanged(QString)), client, SLOT(send(QString)));
}

void CapiCity::newField() {
        field = new Field(this);
        field->setMinimumWidth(500);
        field->setObjectName("Field");

        if (client == 0) return;
        connect(client, SIGNAL(gotEstate(int)), field, SLOT(showEstate(int)));
        connect(client, SIGNAL(gotText(QString)), field, SLOT(appendLine(QString)));
        //connect(client, SIGNAL(clearedText()), field, SLOT(clearText()));
        connect(field, SIGNAL(buttonClicked(QString)), client, SLOT(send(QString)));
        connect(client, SIGNAL(gotButton(QString,QString)), field, SLOT(addButton(QString,QString)));
        connect(client, SIGNAL(playerAdded(CapiClientPlayer*)), field, SLOT(addPlayerAuction(CapiClientPlayer*)));

        connect(client, SIGNAL(auctionStarted()), field, SLOT(showAuction()));
        connect(client, SIGNAL(auctionIdUpdate(int)), field, SLOT(updatAuctionId(int)));
        connect(client, SIGNAL(auctionEstateUpdate(int)), field, SLOT(updateAuctionField(int)));
        connect(client, SIGNAL(auctionBidUpdate(int,int)), field, SLOT(bidUpdate(int,int)));
        connect(client, SIGNAL(auctionStatusUpdate(int)), field, SLOT(updateAuctionStatus(int)));

        connect(field->auction, SIGNAL(bid(QString)), client, SLOT(send(QString)));

        connect(field, SIGNAL(rowBuyed(int)), this, SLOT(buyRow(int)));
        connect(field, SIGNAL(rowSelled(int)), this, SLOT(sellRow(int)));

}

Trade* CapiCity::getTrade(int tid) {
        Trade* t = 0;
        for (int i = 0; i < trades.size(); i++)
                if (trades.value(i)->getId() == tid) t = trades.value(i);
        return t;
}

void CapiCity::removeTrade(Trade* t) {
        trades.removeAll(t);
        delete t;
}

void CapiCity::setLocked() {

        foreach(QDockWidget* d, titlebarWidgets.keys()) {
                const QFlags<QDockWidget::DockWidgetFeature> lockfeatures = QDockWidget::NoDockWidgetFeatures;
                if (!locked) {
                        d->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
                        d->setTitleBarWidget(0);
                } else {
                        d->setFeatures(lockfeatures);
                        d->setTitleBarWidget(titlebarWidgets.value(d));
                }
        }
        QList<QAction*> actions;
        actions << ui->acViewPlayers << ui->acViewTurn << ui->acViewMessages << ui->acViewChat;// << ui->acViewSearch;
        foreach(QAction* a, actions)
                a->setEnabled(!locked);
}

QString CapiCity::emoicons(QString msg) {
        QString ret = msg;

        foreach(QString e, emos->keys())
                ret = ret.replace(e, "<img src=\"./images/smilies/"+emos->value(e)+"\">");

        return ret;
}

void CapiCity::send(QString msg) {
        if (client == 0) return;
        client->send(msg);
}

void CapiCity::drawField() {
        if (field == 0) return;
        if (client->getCurGameId() != -1) {
                CapiGame* g = client->getGame(client->getCurGameId());
                if (g->getStatus() == "run") {
                        field->updateBoard();
                }
        }
}

void CapiCity::tradeUpdate(QString type, int tradeId, int actor, int rev) {
        if (type == "new") {
                Trade* t = new Trade(tradeId);
                t->setRev(0);
                t->setGame(game);
                if (actor != -1) t->updateTradeActor(actor);
                trades.append(t);
                connect(t, SIGNAL(tradeCommand(QString)), client, SLOT(send(QString)));
                field->addTrade(t);
        } else if (type == "edit") {
                Trade* t = getTrade(tradeId);
                if (actor != -1) t->updateTradeActor(actor);
                if (t == 0) return;
                if (rev > 0) t->setRev(rev);
        } else if ((type == "rejected") || (type == "completed")) {
                Trade* t = getTrade(tradeId);
                if (t == 0) return;
                removeTrade(t);
        }
}

void CapiCity::tradePlayerUpdate(int tradeId, int playerid, bool accept) {
        Trade* t = getTrade(tradeId);
        if (t == 0) return;

        t->updateTradeAccept(playerid, accept);
}

void CapiCity::tradeEstateUpdate(int tradeId, int estateId, int targetPlayerId) {
        Trade* t = getTrade(tradeId);
        if (t == 0) return;

        t->updateEstateTrade(estateId, targetPlayerId);
}

void CapiCity::tradeCardUpdate(int tradeId, int cardId, int targetPlayerId) {
        Trade* t = getTrade(tradeId);
        if (t == 0) return;

        t->updateCardTrade(cardId, targetPlayerId);
}

void CapiCity::tradeMoneyUpdate(int tradeId, int fromPlayerId, int money, int targetPlayerId) {
        Trade* t = getTrade(tradeId);
        if (t == 0) return;

        t->updateMoneyTrade(fromPlayerId, money, targetPlayerId);
}

void CapiCity::changeView(QAction* a) {
        QString s = a->data().toString();
        if (s == "player")
                ui->dockPlayers->setVisible(a->isChecked());
        if (s == "turn")
                ui->dockTurn->setVisible(a->isChecked());
        if (s == "messages")
                ui->dockEvents->setVisible(a->isChecked());
        if (s == "chat")
                ui->dockChat->setVisible(a->isChecked());
//      if (s == "search")
//              ui->dockSearch->setVisible(a->isChecked());
        if (s == "lock") {
                locked = a->isChecked();
                setLocked();
        }
}

void CapiCity::setData(QString type, QString data) {
        if (type == "name") send(".n"+data);
        if (type == "image") send(".pi"+data);
}

void CapiCity::setData(QString type, bool data) {
        if (type == "locked") {
                locked = data;
                setLocked();
        }
        if (type == "autoroll") {
                client->setAutoroll(data);
        }
}

void CapiCity::showHost(QString host) {
        if (host == "")
                this->setWindowTitle("Capi City");
        else
                this->setWindowTitle(QString("Capi City - %1").arg(host));
        field->setCapid(client->getCapid());
}

void CapiCity::setShownPlayer(int pid) {
        //First uncheck all showButtons, that does not belong to pid
        for (int i = 0; i < client->getNumPlayers(); i++) {
                if (client->getPlayerFromList(i)->getId() != pid)
                        ((CapiClientPlayer*) client->getPlayerFromList(i))->playerWidget->resetShowButton();
        }

        //Now set the shown player on field
        field->setShownPlayer(pid);
}

bool CapiCity::leaveGame() {
        bool leave = true;
        if ((game != 0) && (game->getStatus() == "run")) {
                leave = false;
                if (QMessageBox::question(this, tr("Close current game?"), tr("Do you really want to leave the current game?"),
                                          QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                        leave = true;
                }
        }

        if (leave) {
                while (trades.size() > 0) {
                        Trade* t = trades.value(0);
                        t->reject();
                        removeTrade(t);
                }
                gameConfig->removeAllOptions();
                client->leaveGame();
                setGame(0);
                setCurGame();

                for (int i = 0; i < client->getNumPlayers(); i++)
                        client->getPlayerFromList(i)->setHasTurn(false);
        }

        return leave;
}

bool CapiCity::disconnectFromServer() {
        if (leaveGame()) {
                games->setRowCount(0);
                client->disconnectFromHost();
                serverChooser->loadServerList();
                layout->setCurrentIndex(3);

                return true;
        }
        return false;
}

void CapiCity::forceDisconnect() {
        while (trades.size() > 0) {
                Trade* t = trades.value(0);
                t->reject();
                removeTrade(t);
        }
        gameConfig->removeAllOptions();
        client->leaveGame();
        setGame(0);
        setCurGame();

        for (int i = 0; i < client->getNumPlayers(); i++)
                client->getPlayerFromList(i)->setHasTurn(false);

        client->disconnectFromHost();
        serverChooser->loadServerList();
        layout->setCurrentIndex(3);
}

void CapiCity::exit() {
        if (leaveGame()) {
                close();
                QApplication::exit();
        }
}

void CapiCity::closeEvent(QCloseEvent *event) {
        if (disconnectFromServer()) {
                s.setValue(CONFIG_WINDOW_SIZE, QVariant(size()));
                s.setValue(CONFIG_WINDOW_LAYOUT, QVariant(saveState(3)));
                s.setValue(CONFIG_WINDOW_LOCKED, QVariant(locked));
                s.setValue(CONFIG_WINDOW_MENUBAR, QVariant(ui->menubar->isVisible()));
                s.setValue(CONFIG_GAME_AUTOROLL, QVariant(ui->acAutoRoll->isChecked()));
                event->accept();
        } else {
                event->ignore();
        }
}

void CapiCity::showMenuBar() {
        setMenuBarVisible(!ui->menubar->isVisible());
}

void CapiCity::setMenuBarVisible(bool vis) {
        ui->menubar->setVisible(vis);
        ui->acShowMenuBar->setChecked(vis);
}

void CapiCity::showAbout() {
        About* a = new About();
        QRect screen = QApplication::desktop()->availableGeometry(this);
        int x = (screen.width() - a->width())/2;
        int y = (screen.height() - a->height())/2;

        a->move(x,y);
        a->show();
}

void CapiCity::showEmoMenu() {
        emoMenu->popup(ui->bEmos->mapToGlobal(QPoint(0,0)));
}

void CapiCity::addEmo(QAction *a) {
        if (a == 0) return;

        ui->tfMess->insert(a->data().toString());
        ui->tfMess->setFocus(Qt::OtherFocusReason);
}

void CapiCity::setAutoroll() {
        if (client == 0) return;
        client->setAutoroll(ui->acAutoRoll->isChecked());
}

void CapiCity::animateMovment(CapiClientPlayer *p, int newPos) {
        if (game == 0) return;
        if (game->getStatus() != "run" || game->getNumEstates() == 0) {
                p->setPos(newPos);
                return;
        }

        curMovingPlayer = p;
        int stPos = p->getPos();
        int time = s.value(CONFIG_GAME_ANIMATIONTIME, CONFIG_GAME_ANIMATIONTIME_DEFAULT).toInt();
        QSequentialAnimationGroup* anim = new QSequentialAnimationGroup(this);
        while (stPos != newPos) {
                QPropertyAnimation* propAnim = new QPropertyAnimation(p, "offset");
                propAnim->setStartValue(0);
                propAnim->setEndValue(100);
                propAnim->setDuration(time);
                anim->addAnimation(propAnim);
                connect(propAnim,SIGNAL(valueChanged(QVariant)), this, SLOT(updateMovment()));
                connect(propAnim, SIGNAL(finished()), this, SLOT(acPos()));
                stPos =(stPos+1)%game->getNumEstates();
        }
        ((QPropertyAnimation* )anim->animationAt(0))->setEasingCurve(QEasingCurve::InQuad);
        ((QPropertyAnimation* )anim->animationAt(0))->setDuration(2*time);;

        ((QPropertyAnimation* )anim->animationAt(anim->animationCount()-1))->setEasingCurve(QEasingCurve::OutQuad);
        ((QPropertyAnimation* )anim->animationAt(anim->animationCount()-1))->setDuration(2*time);
        connect(anim, SIGNAL(finished()), this, SLOT(stopAnim()));
        qDebug() << anim->duration();
        anim->start(QAbstractAnimation::DeleteWhenStopped);
        //disconnect(p, SIGNAL(positionChanged(int)), this, SLOT(acPos(int)));
}

void CapiCity::acPos() {
        if (client == 0) return;
        if (curMovingPlayer == 0) return;
        curMovingPlayer->setPos((curMovingPlayer->getPos()+1)%(game->getNumEstates()));
        client->send(".t"+QString::number(curMovingPlayer->getPos()));
        curMovingPlayer->setOffset(0);
}

void CapiCity::stopAnim() {
        curMovingPlayer = 0;
}

void CapiCity::updateMovment() {
        field->playerOffsetChanged(curMovingPlayer);
        update();
}

void CapiCity::startServer() {
        if (server != 0) delete server;
        server = new CapiServer(this);

        client->connectToHost("localhost", 1234);
}
