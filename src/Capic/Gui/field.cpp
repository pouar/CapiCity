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

#include <QLabel>
#include <QPainter>
#include <QStackedLayout>
#include <QMouseEvent>
#include <QAction>
#include <QMenu>
#include <QFile>
#include <QtDebug>
#include "field.h"
#include "ui_field.h"
#include "cmdtoolbutton.h"
#include "auction.h"
#include "src/Capi/capiestate.h"
#include "src/Capic/capiclientestate.h"

Field::Field(QWidget* parent) : QWidget(parent), ui(new Ui::Field) {
        ui->setupUi(this);
        text = new QTextBrowser(this);
        auction = new Auction();
        stack = new QStackedLayout(ui->center);
        stack->setObjectName("StackField");
        stack->addWidget(text);
        stack->addWidget(auction);
        stack->setCurrentIndex(0);

        ui->wEstate->setVisible(false);
        ui->fButtons->setVisible(false);
        selectedEstate = 0;
        setMouseTracking(true);

        txtColor = false;

        acSell = new QAction(QIcon(":/images/tools/sell.png"), tr("Sell"), this);
        acMortage = new QAction(QIcon(":/images/tools/mortage.png"), tr("Mortgage"), this);
        acUnmortage = new QAction(QIcon(":/images/tools/unmortage.png"), tr("Unmortgage"), this);
        acBuyHouse = new QAction(QIcon(":/images/tools/buyhouse.png"), tr("Buy house"), this);
        acSellHouse = new QAction(QIcon(":/images/tools/sellhouse.png"), tr("Sell house"), this);
        acBuyRow = new QAction(QIcon(":/images/tools/buyhouserow.png"), tr("Buy houserow"), this);
        acSellRow = new QAction(QIcon(":/images/tools/sellhouserow.png"), tr("Sell houserow"), this);
        menu = new QMenu(this);

        connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(menuClick(QAction*)));
        connect(ui->tfsearch, SIGNAL(textChanged(QString)), this, SLOT(setSearch(QString)));

        shownPlayer = 0;
        scroll = 0;

        search = "";
        ui->wSearch->hide();

        imgBoard = new QImage(width(), height(), QImage::Format_ARGB32);
        //imgBoard = new QPixmap(width(), height());
        //imgBoard->fill(this->palette().color(QPalette::Background).rgb());
}

Field::~Field() {
        delete ui;
}

CapiPlayer* Field::getShownPlayer() {
        return shownPlayer;
}

void Field::addTrade(Trade* t) {
        ui->tabWidget->addTab(t, tr("Trade %1").arg(t->getId()));
        ui->tabWidget->setCurrentWidget(t);
}

void Field::setCapid(bool c) {
        capid = c;
}

void Field::setShownPlayer(int pid) {
        bool up = false;
        if (shownPlayer != 0) {
                up = (pid != shownPlayer->getId());
        } else {
                up = (pid != -1);
        }

        if (pid != -1)
                shownPlayer = game->getPlayer(pid);
        else
                shownPlayer = 0;

        if (up) {
                updateBoard();
        }
}

void Field::setGame(CapiGame* g) {
        game = g;
        if (game == 0) ui->wSearch->hide();
}

void Field::setClientId(int id) {
        clientId = id;;
}

QRect Field::getAvatarRect(int pos, bool bigger) {
        QRect fRect = getEstateRect(pos);
        if (N == 0) return QRect(0,0,0,0);
        int side = 4*pos/N;

        int w;
        int h;

        if (side%2 == 0) {
                w = fRect.width()/2;
                h = fRect.height()/4;
        } else {
                w = fRect.width()/4;
                h = fRect.height()/2;
        }
        int s = w;
        if (h < w) s = h;

        if (bigger) s *= 1.5;
        return QRect(fRect.x()+(fRect.width()-s)/2, fRect.y()+(fRect.height()-s)/2, s, s);

        return QRect(0,0,0,0);
}

//calculates the rect for teh Field wiht id fid
QRect Field::getEstateRect(int fid) {

        if (N == 0) return QRect(0,0,0,0);
        int xOffset = (w-s)/2;
        int yOffset = (h-s)/2;

        int side = 4*fid/N;

        int fWidth = 0;
        int fHeight = 0;
        int fposX = 0;
        int fposY = 0;

        //get
        if (fid%(N/4) == 0) {
                //Edge
                fWidth = vertEstateWidth;
                fHeight = horEstateHeight;
        } else {
                if (side%2 == 0) {
                        //We are on the North or south side
                        fWidth = horEstateWidth;
                        fHeight = horEstateHeight;
                } else {
                        fWidth = vertEstateWidth;
                        fHeight = vertEstateHeight;
                }
        }

        if (side == 0) {
                //South side
                fposX = vertEstateWidth+(N/4-1-fid)*horEstateWidth;
                fposY = cornerSize+(N/4-1)*vertEstateHeight;
        }
        if (side == 1) {
                //West side
                fposX = 0;
                fposY = horEstateHeight+(N/2-1-fid)*vertEstateHeight;
        }
        if (side == 2) {
                //North side
                if (fid > N/2) fposX = vertEstateWidth+(fid-N/2-1)*horEstateWidth;
                else fposX = 0;
                fposY = 0;
        }
        if (side == 3) {
                //East side
                fposX = cornerSize+(N/4-1)*horEstateWidth;
                if (fid > 3*N/4) fposY = horEstateHeight+(fid-3*N/4-1)*vertEstateHeight;
                else fposY = 0;
        }
        
        return QRect(fposX+xOffset, fposY+yOffset, fWidth, fHeight);
}

QPoint Field::interpolate(QPoint p1, QPoint p2, int offset) {
        int x1 = p1.x();
        int x2 = p2.x();
        int y1 = p1.y();
        int y2 = p2.y();

        int tx = (offset*x2 + (100-offset)*x1)/100;
        int ty = (offset*y2 + (100-offset)*y1)/100;

        return QPoint(tx,ty);
}

CapiClientEstate* Field::getEstate(int x, int y) {
        for (int i = 0; i < game->getNumEstates(); i++) {
                CapiClientEstate* e = (CapiClientEstate*) game->getEstate(i);
                if (e->isInRect(x, y)) return e;
        }
        return 0;
}

void Field::deleteMenu() {
        menu->clear();
}

void Field::changeEvent(QEvent *e) {
        QWidget::changeEvent(e);
        switch (e->type()) {
        case QEvent::LanguageChange:
                ui->retranslateUi(this);
                break;
        default:
                break;
        }
}

void Field::mousePressEvent(QMouseEvent* event) {
        if (game == 0) return;
        selectedEstate = getEstate(event->x(), event->y());
        if ((event->button() == Qt::LeftButton) || (event->button() == Qt::RightButton)) {
                if (selectedEstate == 0) return;
                if (selectedEstate->getGroup() == 0) return;
                if ((selectedEstate->getOwner() == 0) || (selectedEstate->getOwner()->getId() != clientId)) return;

                deleteMenu();
                acSell->setData(QVariant(QString(".es%1").arg(selectedEstate->getId())));
                acMortage->setData(QVariant(QString(".em%1").arg(selectedEstate->getId())));
                acUnmortage->setData(QVariant(QString(".em%1").arg(selectedEstate->getId())));
                acBuyHouse->setData(QVariant(QString(".hb%1").arg(selectedEstate->getId())));
                acSellHouse->setData(QVariant(QString(".hs%1").arg(selectedEstate->getId())));
                if (selectedEstate->getGroup() != 0) {
                        acBuyRow->setData(QVariant(QString(".hrb%1").arg(selectedEstate->getGroup()->getId())));
                        acSellRow->setData(QVariant(QString(".hrs%1").arg(selectedEstate->getGroup()->getId())));
                }
                //Not shown with monopd, because it has problems with it.
                if (capid && game->getSellEnabled() && !selectedEstate->getMortaged() && (selectedEstate->getGroup()->getNumHouses() == 0)) {
                        menu->addAction(acSell);
                        menu->addSeparator();
                }

                if (selectedEstate->getCanToggleMortage()) {
                        if (selectedEstate->getMortaged()) menu->addAction(acUnmortage);
                        else menu->addAction(acMortage);
                        menu->addSeparator();
                }

                CapiGroup* g = selectedEstate->getGroup();
                if (selectedEstate->getCanBuyHouse()) menu->addAction(acBuyHouse);
                if ((g->getNumEstates() > 1) && g->canBuyHouseRow()) menu->addAction(acBuyRow);
                if (selectedEstate->getCanSellHouse()) menu->addAction(acSellHouse);
                if ((g->getNumEstates() > 1) && g->canSellHouseRow()) menu->addAction(acSellRow);

                menu->popup(QWidget::mapToGlobal(event->pos()));
        }
        event->accept();
}

void Field::wheelEvent(QWheelEvent* event) {
        if (event->modifiers() != Qt::ControlModifier) return;

        if (event->orientation() == Qt::Vertical) {
                if (event->delta() > 0) {
                        scroll = (scroll+1)%game->getNumEstates();
                } else {
                        scroll --;
                        if (scroll < 0) scroll = game->getNumEstates()-1;
                }
        }
        event->accept();
        updateBoard();
}

void Field::mouseMoveEvent(QMouseEvent* event) {
        auction->setToolTip("");
        if (game == 0) return;
        CapiEstate* f = getEstate(event->x(), event->y());
        if (f == 0) {
                setToolTip("");
                return;
        }

        QString tooltip = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\"><html><head><style type=\"text/css\">th { font-weight:normal; } .property { text-align:right; font:blue; } .value { text-align:left; }</style><title>bla</title></head><body><div>";
        tooltip += "<table border=\"0\" style=\"font-weight:normal\">";
        tooltip += "<tr><td colspan=\"3\" align=\"center\">"+f->getName()+"</td></tr>";
        QString base = tr("<tr><th align=\"right\">%1</th><th>:&nbsp;</th><th align=\"left\">%2</th></tr>");
        if (f->getGroup() != 0) {
                CapiGroup* g = f->getGroup();
                tooltip += base.arg(tr("Group"), g->getName());
        }
        if (f->getBuyable()) tooltip += base.arg(tr("Price")).arg(f->getPrice());
        if (f->getOwner() != 0) {
                tooltip = tooltip+base.arg(tr("Owner"), f->getOwner()->getName());
                if (f->getCanToggleMortage() && !f->getMortaged()) tooltip += base.arg(tr("Mortgagevalue")).arg(f->getMortageVal());
                if (f->getCanToggleMortage() && f->getMortaged())  tooltip += base.arg(tr("Unmortgagevalue")).arg(f->getUnmortageVal());
        }
        if (f->getCanBuyHouse())  tooltip += base.arg(tr("Buy House")).arg(f->getHousePrice());
        if (f->getCanSellHouse()) tooltip += base.arg(tr("Sell House")).arg(f->getSellHousePrice());
        if (f->getMoney() > 0) tooltip += base.arg(tr("Fine")).arg(f->getMoney());
        if (f->getRent(0) + f->getRent(1) + f->getRent(2) + f->getRent(3) + f->getRent(4) + f->getRent(5) != 0) {
                QString txt[6];
                txt[0] = tr("Rent with no houses");
                txt[1] = tr("Rent with one house");
                txt[2] = tr("Rent with two houses");
                txt[3] = tr("Rent with three houses");
                txt[4] = tr("Rent with four houses");
                txt[5] = tr("Rent with a hotel");
                for (int i = 0; i < 6; i++) {
                        QString rentTxt = QString("%1");
                        if (f->getHouses() == i)
                                rentTxt = QString("<span style=\"font-weight:bold\">%1</span>");

                        //If a group is owned by one single player, the normal rent is doubled
                        int acRent = f->getRent(i);
                        if ((i == 0) && f->getGroup()->getAllEstatesSameOwner()) acRent *= 2;

                        tooltip += base.arg(txt[i]).arg(rentTxt.arg(acRent));
                }
        }
        tooltip += "</table></div></body></html>";
        setToolTip(tooltip);
}

void Field::emitButtonClick(QString cmd) {
        emit(buttonClicked(cmd));
        clearButtons();
}

void Field::menuClick(QAction* a) {
        if (selectedEstate != 0) {
                if ((a->data().toString().startsWith(".hrb")) && !capid)
                        emit(rowBuyed(selectedEstate->getGroup()->getId()));
                else if (a->data().toString().startsWith(".hrs")) emit(rowSelled(selectedEstate->getGroup()->getId()));
                else {
                        emit(buttonClicked(a->data().toString()));
                }
        }
        emit(fieldSelected(0));
        selectedEstate = 0;
        deleteMenu();
}

void Field::showEstate(int fid) {
        if (game == 0) return;
        QString name = "";
        QString price = "";
        if (fid == -1) {
                //clear Estate
                ui->lEstate->setText("<span style=\"font-size:20pt; font-weight:bold\"> </span>");
                ui->lPrice->setText("");
        } else {
                CapiClientEstate* f = (CapiClientEstate*) game->getEstate(fid);
                name = f->getName();
                price = QString::number(f->getPrice());
                QString col = "#000000";
                if (f->getBrushColor().value() < 50) col = "#ffffff";

                if (f->getGroup() != 0) {
                        if (f->getColor() == "") {
                                ui->lEstate->setText("<span style=\"font-size:20pt; font-weight:bold\">"+name+"</span>");
                        } else {
                                ui->lEstate->setText("<span style=\"background:"+f->getColor()+"; font-size:20pt; font-weight:bold; color:"+col+"\">"+name+"</span>");
                        }
                        ui->lPrice->setText("<span style=\"font-size:20pt; font-weight:bold\">"+price+"</span>");
                } else {
                        ui->lEstate->setText("<span style=\"font-size:20pt; font-weight:bold\">"+name+"</span>");
                        ui->lPrice->setText("");
                }
                ui->lEstate->setToolTip(name);
                ui->lPrice->setToolTip(price);
                ui->wEstate->setVisible(true);
        }
}

void Field::appendLine(QString txt) {
        if (gameMessages.size() >= 15) gameMessages.removeAt(0);
        gameMessages.append(txt);

        QString txtHtml = "";
        QString bgNorm = palette().color(QPalette::Base).name();
        QString bgAlt = palette().color(QPalette::AlternateBase).name();

        for (int i = 0; i < gameMessages.size(); i++) {
                QString bg = (i%2==0)?bgNorm:bgAlt;
                txtHtml = txtHtml+"<p style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; background:"+bg+"\"><span>"+gameMessages.value(i)+"</span></p>";
        }
        text->setHtml(txtHtml+"<span>&nbsp;</span><a name=\"end\" style=\"font-size:10px\">&nbsp;</a>");
        text->scrollToAnchor("end");

        text->setToolTip("");
}

void Field::addButton(QString text, QString cmd) {
        CmdToolButton* b = new CmdToolButton();
        b->setText(text);
        b->cmd = cmd;
        if (cmd == ".eb") b->setIcon(QIcon(":/images/field/document.png"));
        if (cmd == ".ea") b->setIcon(QIcon(":/images/tools/auction.png"));
        if (cmd == ".E") b->setIcon(QIcon(":/images/tools/endturn.png"));

        if (cmd == ".jr") b->setIcon(QIcon(":/images/tools/jailroll.png"));
        if (cmd == ".jc") b->setIcon(QIcon(":/images/tools/jailcard.png"));
        if (cmd == ".jp") b->setIcon(QIcon(":/images/tools/jailpay.png"));

        if (cmd == ".T$") b->setIcon(QIcon(":/images/tools/jailpay.png"));
        if (cmd == ".T%") b->setIcon(QIcon(":/images/tools/paypercent.png"));

        b->setToolTip(b->text());
        b->setIconSize(QSize(24,24));
        connect(b, SIGNAL(cmdClicked(QString)), this, SLOT(emitButtonClick(QString)));
        if (cmd == ".gx") clearButtons();
        buttons.append(b);
        ui->buttons->insertWidget(buttons.size(), b);
        ui->fButtons->setVisible(true);
}

void Field::clearText() {
        text->setHtml("");
        gameMessages.clear();
        txtColor = false;
        text->setToolTip("");
        clearButtons();
}

void Field::clearButtons() {
        while (buttons.size() > 0) {
                CmdToolButton* b = buttons.value(0);
                buttons.removeAll(b);
                delete b;
        }
        ui->fButtons->setVisible(false);
}

void Field::addPlayerAuction(CapiClientPlayer* p) {
        auction->addPlayerWidget(p->auction);
}

void Field::showAuction() {
        if (game == 0) return;
        stack->setCurrentIndex(1);
        for (int i = 0; i < game->getNumPlayer(); i++) {
                CapiClientPlayer* pl = (CapiClientPlayer*) game->getPlayerFromList(i);
                pl->auction->setName(pl->getName());
                pl->auction->setMoney("0");
        }
        auction->resetSpinBox();
}

void Field::updatAuctionId(int aid) {
        auction->setId(aid);
}

void Field::updateAuctionField(int fid) {
        if (game == 0) return;
        auction->setEstateName(game->getEstate(fid)->getName());
}

void Field::bidUpdate(int pid, int bid) {
        if (game == 0) return;
        CapiClientPlayer* p = (CapiClientPlayer*) game->getPlayer(pid);
        if (p == 0) return;
        for (int i = 0; i < game->getNumPlayer(); i++) {
                CapiClientPlayer* pl = (CapiClientPlayer*) game->getPlayerFromList(i);
                pl->auction->setName(pl->getName());
        }
        p->auction->setName("<b>"+p->getName()+"</b>");
        p->auction->setMoney(QString::number(bid));
        auction->setMinBid(bid+1);
}

void Field::updateAuctionStatus(int status) {
        switch (status) {
        case 0:
                auction->setStatusText("");
                break;
        case 1:
                auction->setStatusText(tr("First..."));
                break;
        case 2:
                auction->setStatusText(tr("Second..."));
                break;
        case 3:
                stack->setCurrentIndex(0);
                break;
        default:
                break;
        }
}

void Field::paintEvent(QPaintEvent* pe) {
        QPainter p(this);

        p.drawImage(0,0, *imgBoard);

        //Set size of the display
        ui->centWidget->setMaximumWidth((N/4-1)*horEstateWidth-20);
        ui->centWidget->setMaximumHeight((N/4-1)*vertEstateHeight-20);
        ui->centWidget->setMinimumWidth((N/4-1)*horEstateWidth-20);
        ui->centWidget->setMinimumHeight((N/4-1)*vertEstateHeight-20);

        pe->accept();
}

void Field::setSearch(QString s) {
        search = s;
        updateBoard();
}

void Field::showSearch() {
        ui->wSearch->show();
        ui->tfsearch->setFocus(Qt::OtherFocusReason);
}

void Field::clearSearch() {
        ui->tfsearch->setText("");
}

void Field::resizeEvent(QResizeEvent* e) {
        if (imgBoard != 0) delete imgBoard;
        imgBoard = new QImage(rect().width(), rect().height(), QImage::Format_ARGB32);
        //imgBoard = new QPixmap(rect().width(), rect().height());
        if (game != 0) {
                N = game->getNumEstates();
                w = imgBoard->width()-1;
                h = imgBoard->height()-1;
                s = w;
                if (h < w) s = h;

                //Set size of widget in Estate-center
                cornerSize = 2*s/13;
                vertEstateHeight = (s-2*cornerSize)/((N/4)-1);
                horEstateWidth = vertEstateHeight;
                horEstateHeight = cornerSize;
                vertEstateWidth = cornerSize;

                s = ((N/4)-1)*horEstateWidth+2*cornerSize;

                updateBoard();
        }
        e->accept();
}

void Field::renderBoard() {
        if (game == 0) return;
        if (imgBoard == 0) return;

        imgBoard->fill(qRgba(0,0,0,0));
        //imgBoard->fill(this->palette().color(QPalette::Background).rgb());

        //Now draw the estates
        for (int i = 0; i < N; i++) {
                renderEstate(i);
        }
}

void Field::renderEstate(int eid) {
        QPainter p(imgBoard);

        int side = 0;
        if (N > 0)
                side = 4*((eid+scroll)%game->getNumEstates())/N;

        //Get Data for drawing
        CapiClientEstate* f = (CapiClientEstate*) game->getEstate(eid);
        QRect fRect = getEstateRect((eid+scroll)%game->getNumEstates());
        f->setRect(fRect);

        int fposX = fRect.x();
        int fposY = fRect.y();
        int fWidth = fRect.width();
        int fHeight = fRect.height();

        QRect strRect; //Drawing rect for the street-color
        QRect fName; //Rect for the Estatename

        QRect docRect; //Rect for document or owner-avatar
        int houseStartX = 0; //  Position of the houses
        int houseStartY = 0; //
        int houseWidth = 0;  //   Sizes of a house
        int houseHeight = 0; //
        int houseOffsetX = 0; //  Offset for house drawing
        int houseOffsetY = 0; //
        if (side == 0) {
                //South side
                int docS =  min(fWidth,fHeight/8);
                strRect = QRect(fposX, fposY, fWidth, fHeight/4);
                fName = QRect(fposX, fposY+fHeight/4, fWidth, 5*fHeight/8);
                docRect = QRect(fposX+fWidth-docS, fposY+fHeight-docS, docS, docS);
                houseStartX = fposX,
                        houseStartY = fposY+2;
                houseWidth = fWidth/4;
                houseHeight = fHeight/4-4;
                houseOffsetX = fWidth/4;
                houseOffsetY = 0;
        } else if (side == 1) {
                //West side
                int docS =  min(fWidth/8,fHeight);
                strRect = QRect(fposX+3*fWidth/4, fposY, fWidth/4, fHeight);
                fName = QRect(fposX+fWidth/8, fposY, 5*fWidth/8, fHeight);
                docRect = QRect(fposX, fposY+fHeight-docS, docS, docS);
                houseStartX = fposX+3*fWidth/4+2,
                        houseStartY = fposY;
                houseWidth = fWidth/4-4;
                houseHeight = fHeight/4;
                houseOffsetX = 0;
                houseOffsetY = fHeight/4;
        } else if (side == 2) {
                //North side
                int docS =  min(fWidth,fHeight/8);
                strRect = QRect(fposX, fposY+3*fHeight/4, fWidth, fHeight/4);
                fName = QRect(fposX, fposY+fHeight/8, fWidth, 5*fHeight/8);
                docRect = QRect(fposX, fposY, docS, docS);
                houseStartX = fposX,
                        houseStartY = fposY+3*fHeight/4+2;
                houseWidth = fWidth/4;
                houseHeight = fHeight/4-4;
                houseOffsetX = fWidth/4;
                houseOffsetY = 0;
        } else if (side == 3) {
                //East side
                int docS =  min(fWidth/8,fHeight);
                strRect = QRect(fposX, fposY, fWidth/4, fHeight);
                fName = QRect(fposX+fWidth/4, fposY, 5*fWidth/8, fHeight);
                docRect = QRect(fposX+fWidth-docS, fposY, docS, docS);
                houseStartX = fposX+2,
                        houseStartY = fposY;
                houseWidth = fWidth/4-4;
                houseHeight = fHeight/4;
                houseOffsetX = 0;
                houseOffsetY = fHeight/4;
        }

        if (f != 0) {
                //Draw in Background-color
                QColor c(192,255,192);
                if (f->getBgColor() != "") c = f->getBgBrushColor();
                p.setBrush(c);
                p.drawRect(fposX, fposY, fWidth, fHeight);

                //Highlicht buyable and non buyed estates
                if ((f->getOwner() == 0) && f->getBuyable()) {
                        p.setBrush(QColor(255,255,255,128));
                        p.drawRect(fposX, fposY, fWidth, fHeight);
                }

                //Show mortgage or highlight
                if (f->getMortaged()) {
                        p.setBrush(QColor(0,0,0,128));
                        p.drawRect(fposX, fposY, fWidth, fHeight);
                }
                if ((shownPlayer != 0) && (f->getOwner() == shownPlayer)) {
                        p.setBrush(QColor(255,255,0,64));
                        p.drawRect(fposX, fposY, fWidth, fHeight);
                }

                if (f->getBuyable()) {
                        //Draw document or avatar of owner
                        QPixmap pix;
                        if (f->getOwner() == 0) {
                                pix = QPixmap(":/images/field/document.png");
                        } else {
                                CapiClientPlayer* pl = (CapiClientPlayer*) f->getOwner();

                                pix = QPixmap::fromImage(pl->avImage);
                        }
                        p.drawPixmap(docRect, pix);
                }

                //Draw field name or image
                p.setPen(Qt::black);
                int imSize = fName.width();
                if (fName.height() < imSize) imSize = fName.height();
                QPixmap estateImage(":/images/field/"+f->getPicture());
                if (f->getName().contains(search, Qt::CaseInsensitive)) {
                        if (!estateImage.isNull()) {
                                estateImage = estateImage.scaled(imSize, imSize);
                                QRect imRect(fName.x()+(fName.width()-imSize)/2, fName.y()+(fName.height()-imSize)/2, imSize, imSize);
                                p.drawPixmap(imRect, estateImage);
                        } else {
                                if (f->getPicture() != "") qDebug() << "Missing icon: "+f->getPicture();
                                p.drawText(fName, Qt::AlignCenter|Qt::TextWrapAnywhere, f->getName());
                        }
                }
                if (f->getGroup() != 0) {
                        //Draw streetColor
                        p.setPen(f->getBrushColor());
                        p.setBrush(f->getBrushColor());
                        p.drawRect(strRect);

                        //draw Houses or Hotel
                        p.setPen(Qt::black);
                        if (f->getHouses() < 5) {
                                p.setBrush(Qt::green);
                                for (int k = 0; k < f->getHouses(); k++) {
                                        p.drawRect(houseStartX+k*houseOffsetX, houseStartY+k*houseOffsetY, houseWidth, houseHeight);
                                }
                        } else {
                                p.setBrush(Qt::red);
                                p.drawRect(houseStartX, houseStartY, houseWidth+houseOffsetX, houseHeight+houseOffsetY);
                        }
                }
        }

        //Draw Estate Borders
        p.setPen(QColor(0,0,0));
        p.setBrush(Qt::NoBrush);
        p.drawRect(fposX, fposY, fWidth, fHeight);
}

void Field::renderPlayer(int i) {
        QPainter p(imgBoard);

        CapiPlayer* pl = game->getPlayerFromList(i);
        if (pl == 0) return;
        if (game->getNumEstates() == 0) return;
        if (!pl->getBankrupt() && !pl->getSpectator()) {
                //Get the position we draw the player
                //For that get the poistion on the players current position and the following position
                QRect avRect1 = getAvatarRect((pl->getPos()+scroll)%game->getNumEstates(), ((shownPlayer != 0) && (pl == shownPlayer)));
                QRect avRect2 = getAvatarRect((pl->getPos()+scroll+1)%game->getNumEstates(), ((shownPlayer != 0) && (pl == shownPlayer)));

                //Now interpolate it
                QPoint pos = interpolate(avRect1.topLeft(), avRect2.topLeft(), ((CapiClientPlayer*) pl)->getOffset());

                //Create the rects to draw
                QRect avRect(pos, avRect1.size());
                QRect shRect = avRect.translated(avRect.width()/4, avRect.height()/4);

                //Draw them
                p.drawImage(shRect, ((CapiClientPlayer*) pl)->shadow);
                p.drawImage(avRect, ((CapiClientPlayer*) pl)->avImage);
        }
}

void Field::updateBoard() {
        renderBoard();
        if (game != 0) {
                //Draw Players
                for (int i = 0; i < game->getNumPlayer(); i++) {
                        renderPlayer(i);
                }
        }
        update();
}

void Field::playerOffsetChanged(CapiClientPlayer* p) {
        int p1 = p->getPos();
        int p2 = 0;
        if (N > 0)
                p2 = (p->getPos()+1)%N;
        renderEstate(p1);
        renderEstate(p2);

        for (int i = 0; i < game->getNumPlayer(); i++) {
                if ((game->getPlayerFromList(i)->getPos() == p1) || (game->getPlayerFromList(i)->getPos() == p2))
                        renderPlayer(i);
        }
}
