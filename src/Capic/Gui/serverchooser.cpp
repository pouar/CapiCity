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

#include "serverchooser.h"
#include "ui_serverchooser.h"

#include <QUrl>
#include <QNetworkRequest>
#include <QSettings>
#include <QDebug>

#include "src/defines.h"

ServerChooser::ServerChooser(QWidget* parent) : QWidget(parent), ui(new Ui::ServerChooser) {
        ui->setupUi(this);

        manager = new QNetworkAccessManager(this);

        QSettings s;
        //Get Serverinfos
        QString serverList = s.value(CONFIG_CONNECTION_SERVERLIST, QVariant("")).toString();
        if (serverList != "") {
                QStringList lines = serverList.split(";");
                foreach (QString line, lines) {
                        QStringList data = line.split(":");
                        QString host = data.value(0);
                        int port = data.value(1).toInt();

                        CapiServerInfo* csi = new CapiServerInfo(host, port);

                        serverInfos.append(csi);
                }

                //And load it
                loadServerList();
        }
        connect(ui->tServer, SIGNAL(cellClicked(int,int)), this, SLOT(setServer(int)));
        connect(ui->tServer, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(connectToServer()));
        connect(ui->bConnect, SIGNAL(clicked()), this, SLOT(connectToServer()));
        connect(ui->tfServer, SIGNAL(returnPressed()), this, SLOT(connectToServer()));
        connect(ui->spPort, SIGNAL(editingFinished()), this, SLOT(connectToServer()));

        connect(ui->bReload, SIGNAL(clicked()), this, SLOT(loadServerList()));

        connect(ui->bStartServer, SIGNAL(clicked()), this, SIGNAL(localServerChoosen()));
        connect(ui->bDelServInfo, SIGNAL(clicked()), this, SLOT(deleteServerInfo()));
}

ServerChooser::~ServerChooser() {
        //Save Server-infos
        QString serverList = "";
        foreach (CapiServerInfo* csi, serverInfos) {
                serverList += csi->getAdress()+":"+QString::number(csi->getPort())+";";
        }
        serverList.chop(1);

        QSettings s;
        s.setValue(CONFIG_CONNECTION_SERVERLIST, serverList);

        delete ui;

        while (serverInfos.size() > 0) {
                CapiServerInfo* csi = serverInfos.value(0);
                serverInfos.removeAll(csi);

                delete csi;
        }
}

void ServerChooser::parseDoc(QDomDocument d) {
        QDomElement root = d.documentElement();
        if (root.tagName() == "monopigator") {
                //We get a list of servers
                QDomElement e = root.firstChildElement("server");
                while (!e.isNull()) {
                        QString host = e.attribute("host", "");
                        QTableWidgetItem* tih = new QTableWidgetItem(host);
                        if (host != "") {
                                if (ips.contains(host)) continue;
                                QTableWidgetItem* tiport = new QTableWidgetItem(e.attribute("port", ""));
                                QTableWidgetItem* tip = new QTableWidgetItem(e.attribute("users", ""));
                                QTableWidgetItem* tiip = new QTableWidgetItem(e.attribute("ip", ""));
                                QTableWidgetItem* tiv = new QTableWidgetItem(e.attribute("version", ""));
                                int r = ui->tServer->rowCount();
                                tih->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                                tiport->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                                tip->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                                tiip->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                                tiv->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                                ui->tServer->insertRow(r);
                                ui->tServer->setItem(r, 0, tih);
                                ui->tServer->setItem(r, 1, tiport);
                                ui->tServer->setItem(r, 2, tip);
                                ui->tServer->setItem(r, 3, tiip);
                                ui->tServer->setItem(r, 4, tiv);
                                ips.append(host);
                        }
                        e = e.nextSiblingElement("server");
                }
        }
}

void ServerChooser::loadServerList() {
        //There is no known used gator out there.
        //So looking for servers is pointless :(


        ui->tServer->clearContents();
        ui->tServer->setRowCount(0);
        ui->tServer->setColumnCount(4);
        ui->tServer->setHorizontalHeaderLabels(QStringList(tr("Address")) << tr("Port") << tr("Online") << tr("Version"));

        /*
          ips.clear();

          //New code to get the server-list
          reply1 = manager->get(QNetworkRequest(QUrl("http://monopd-gator.kde.org")));
          connect(reply1, SIGNAL(finished()), this, SLOT(parseList1()));
          reply2 = manager->get(QNetworkRequest(QUrl("http://monopd-gator.not-another-server.org")));
          connect(reply2, SIGNAL(finished()), this, SLOT(parseList2()));
        */

        //ui->tServer->hideColumn(1);
        //ui->tServer->hideColumn(3);

        //New code for manually adding servers
        foreach (CapiServerInfo* si, serverInfos) {
                bool isOnline = si->isOnline();
                QString version = si->getVersion();
                QString adress = si->getAdress();
                int port = si->getPort();

                QTableWidgetItem* tiadr = new QTableWidgetItem(adress);
                QTableWidgetItem* tiport = new QTableWidgetItem(QString::number(port));
                QTableWidgetItem* tionline = new QTableWidgetItem(isOnline?tr("Yes"):tr("No"));
                QTableWidgetItem* tiv = new QTableWidgetItem(version);

                int r = ui->tServer->rowCount();
                tiadr->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                tiport->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                tionline->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                tiv->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

                ui->tServer->insertRow(r);
                ui->tServer->setItem(r, 0, tiadr);
                ui->tServer->setItem(r, 1, tiport);
                ui->tServer->setItem(r, 2, tionline);
                ui->tServer->setItem(r, 3, tiv);
        }

        ui->tServer->resizeColumnsToContents();
}

void ServerChooser::parseList1() {
        parseServerList(reply1);
        reply1->deleteLater();
}

void ServerChooser::parseList2() {
        parseServerList(reply2);
        reply2->deleteLater();
}

void ServerChooser::parseServerList(QNetworkReply* reply) {
        QByteArray bytes = reply->readAll();
        int size = bytes.size();
        QString xml = QString::fromUtf8(bytes.data(), size);
        qDebug() << xml;
        QDomDocument d;
        d.setContent(xml);
        parseDoc(d);
        ui->tServer->resizeColumnsToContents();

}

void ServerChooser::setServer(int r) {
        QTableWidgetItem* tis = ui->tServer->item(r,0);
        QTableWidgetItem* tip = ui->tServer->item(r,1);
        if (tis == 0) return;
        if (tip == 0) return;
        ui->tfServer->setText(tis->text());
        ui->spPort->setValue(tip->text().toInt());
}

void ServerChooser::connectToServer() {
        QString host = ui->tfServer->text();

        int port;
        port = ui->spPort->value();

        CapiServerInfo* csi = new CapiServerInfo(host, port);
        //Append if not there yet
        bool haveIt = false;
        foreach (CapiServerInfo* si, serverInfos) {
                if ((si->getAdress() == csi->getAdress()) &&
                    (si->getPort() == csi->getPort()))
                        haveIt = true;
        }

        if (!haveIt)
                serverInfos.append(csi);
        else
                delete csi;

        csi = 0;

        //Save Server-infos
        QString serverList = "";
        foreach (CapiServerInfo* csi, serverInfos) {
                serverList += csi->getAdress()+":"+QString::number(csi->getPort())+";";
        }
        serverList.chop(1);

        QSettings s;
        s.setValue(CONFIG_CONNECTION_SERVERLIST, serverList);

        emit(serverChoosen(host, port));
}

void ServerChooser::deleteServerInfo() {
        QString host = ui->tfServer->text();
        int port = ui->spPort->value();

        //search for SI
        foreach (CapiServerInfo* si, serverInfos) {
                if ((si->getAdress() == host) && (si->getPort() == port))
                        serverInfos.removeAll(si);
        }

        ui->tfServer->setText("");
        ui->spPort->setValue(1234);

        loadServerList();
}

void ServerChooser::changeEvent(QEvent *e) {
        QWidget::changeEvent(e);
        switch (e->type()) {
        case QEvent::LanguageChange:
                ui->retranslateUi(this);
                break;
        default:
                break;
        }
}
