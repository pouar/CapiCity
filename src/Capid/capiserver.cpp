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

#include <QTcpSocket>
#include <QNetworkAccessManager>
#include <QDir>
#include <QStringList>
#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include <QXmlStreamWriter>
#include <QMap>
#include <QHostInfo>
#include <QApplication>
#include "capiserver.h"
#include "capiserverplayer.h"

CapiServer::CapiServer(QObject* parent) : QObject(parent) {

        host             = QHostInfo::localDomainName();
        port             = "1234";
        version          = "1.0 Capid";
        registerAtGators = false;
        registerInterval = 180;

        //Get Server-data from config file
        QFile cfile(QDir::currentPath()+"/config");
        if (!cfile.exists()) qDebug() << "Cound not find config file! Using defaults";
        else {
                //Read configs
                cfile.open(QIODevice::ReadOnly);
                QMap<QString, QString> map;
                while (!cfile.atEnd()) {
                        QString line = cfile.readLine();
                        line = line.replace("\n", "");

                        //Ignore comments and empty lines
                        if (line == "") continue;
                        if (line.startsWith("#")) continue;

                        QStringList list = line.split("=");
                        map.insert((QString) list.value(0), (QString) list.value(1));
                }

                //Now apply them
                QList<QString> keys = map.keys();
                foreach (QString k, keys) {
                        QString val = map.value(k);
                        //if (k == "host") host = val;
                        if (k == "port") port = val;
                        if (k == "version") version = val;
                        //if (k == "register") registerAtGators = false; //Gator is down. so there it wont be used (val == "1");
                        //if (k == "registerInterval") registerInterval = val.toInt();
                }
        }

        init();
}

CapiServer::CapiServer(QString p, QObject* parent) : QObject(parent) {
        host             = "";
        port             = p;
        version          = "1.0 Capid";
        registerAtGators = false;
        registerInterval = 180;

        init();
}

CapiServer::~CapiServer() {
        timer->stop();
        delete timer;

        serv->close();
        delete serv;

        registerAtGators = false;
        delete this->man;
        man = 0;
        delete this->oldReply;
        oldReply = 0;

        while (templates.size() > 0) {
                CapiTemplate* t = templates.value(0);
                templates.removeAll(t);
                delete t;
        }
        while (players.size() > 0) {
                CapiServerPlayer* p = players.value(0);
                players.removeAll(p);
                delete p;
        }
        while (games.size() > 0) {
                CapiServerGame* g = games.value(0);
                games.removeAll(g);
                delete g;
        }
}

void CapiServer::init() {
        pID = 0;
        gID = 0;

        //Start timer to implement time dependend stuff (eg: auctions)
        timer = new QTimer(this);
        timer->setTimerType(Qt::PreciseTimer);
        timer->start(1000);
        connect(timer, SIGNAL(timeout()), this, SLOT(tick()));

        //get all gametemplates from the games/ dir
        QDir dir("games/", "*.xml");

        QStringList files = dir.entryList();
        foreach(QString f, files) {
                QFile file(QDir::currentPath()+"/games/"+f);
                if (!file.exists()) qDebug(QString("File: "+file.fileName()+" could not be found.").toUtf8().data());
                file.open(QIODevice::ReadOnly);

                QString name = "";
                QString desc = "";
                QString type = f.replace(".xml", "");

                QString errmsg = "";
                int errline;
                int errcol;
                QDomDocument doc;
                if (doc.setContent(&file, false, &errmsg, &errline, &errcol)) {
                        QDomElement cnt = doc.documentElement();
                        if (cnt.nodeName() == "capigame") {
                                QDomElement genData = cnt.firstChildElement("general");
                                if (!genData.isNull()) {
                                        QDomElement elemName = genData.firstChildElement("name");
                                        if (!elemName.isNull()) name = elemName.text();
                                        QDomElement elemDesc = genData.firstChildElement("description");
                                        if (!elemName.isNull()) desc = elemDesc.text();
                                }
                        }
                        CapiTemplate* tmpl = new CapiTemplate();
                        tmpl->setType(type);
                        tmpl->setName(name);
                        tmpl->setDesc(desc);
                        templates.append(tmpl);
                } else {
                        qDebug(QString("Could not parse: "+f+": '"+errmsg+"' at line "+QString::number(errline)+"\n").toUtf8().data());
                }
        }

        //Start server
        serv = new QTcpServer();
        serv->listen(QHostAddress::Any, 1234);
        if (!serv->isListening()) {
                qDebug(QString("Could not start server. Probaly the port %1 is used by an other service.").arg(port).toUtf8().data());
        }

        connect(serv, SIGNAL(newConnection()), this, SLOT(manageConnection()));

        translator = new MessageTranslator();
        translator->loadLang("de_DE");
        oldReply = 0;
        man = new QNetworkAccessManager(this);
        //registerGator();
}

void CapiServer::tick() {
        foreach (CapiServerPlayer* p, players)
                p->tick();

        foreach (CapiServerGame* g, games)
                g->tick();

        //secsSinceLastRegister++;
        //if (secsSinceLastRegister >= registerInterval)
        //      registerGator();
}

void CapiServer::manageConnection() {
        QTcpSocket* sock = serv->nextPendingConnection();
        CapiServerPlayer* p = new CapiServerPlayer(++pID);

        players.append(p);
        p->setSocket(sock);

        p->send("<server version=\""+version+"\" name=\"Capid\" />");
        connect(p, SIGNAL(gotMessage(int,QString)), this, SLOT(handleMessage(int,QString)));
        sock->setObjectName(QString::number(pID));
        connect(sock, SIGNAL(disconnected()), sock, SLOT(deleteLater()));
}

CapiServerPlayer* CapiServer::getPlayerById(int pid) {
        foreach(CapiServerPlayer* p, players) {
                if (p->getId() == pid) return p;
        }
        return 0;
}

CapiServerGame* CapiServer::getGameById(int gid) {
        foreach(CapiServerGame* g, games) {
                if (g->getId() == gid) return g;
        }
        return 0;
}

QString CapiServer::maskXML(QString txt) {
        QString msg = txt;
        msg = msg.replace("&", "&#38;");
        msg = msg.replace("<", "&#60;");
        msg = msg.replace(">", "&#62;");
        msg = msg.replace("=", "&#61;");
        msg = msg.replace("\"", "&#34;");
        return msg;
}

bool CapiServer::hasSemicolon(QString txt, int* pos) {
        //Checks if the found & is valid eg: it has the &xxx; where xxx only consists of non XML chars.
        // if it is valid pos will be set to the position of the ; otherwise it will not ne changed.
        Q_ASSERT(txt.at(*pos) == QString("&").at(0));
        int p = *pos + 1;
        while (p < txt.size()) {
                if (txt.at(p) == QString("&").at(0)) return false;
                if (txt.at(p) == QString(";").at(0)) {
                        *pos = p;
                        return true;
                }
                p++;
        }
        return false;
}

bool CapiServer::nameUsed(QString name) {
        foreach(CapiServerPlayer* p, players) {
                if (p->getName() == name) return true;
        }
        return false;
}

void CapiServer::sendToAll(QString msg, int gid) {
        foreach(CapiServerPlayer* p, players) {
                int g = -1;
                if (p->getGame() != 0) g = p->getGame()->getId();
                if ((gid == -2) || (g == gid)) p->send(msg);
        }
}

void CapiServer::deletePlayer(int pid) {
        CapiServerPlayer* p = getPlayerById(pid);
        players.removeAll(p);
        disconnect(p, SIGNAL(gotMessage(int,QString)), this, SLOT(handleMessage(int,QString)));
        delete p;
}

void CapiServer::handleMessage(int pid, QString msg) {
        CapiServerPlayer* p = getPlayerById(pid);
        if (p == 0) {
                qDebug() << QString("Server has a bug! Cannot find Player with ID: %1").arg(pid);
                return;
        }

        //command from player
        if (msg.startsWith(".")) {

                if (msg.startsWith(".n")) {
                        //Ablity to change name
                        QString name = msg.remove(0,2).replace("\n", "");
                        //Check if name is used, and search a number to append, if needed
                        if (nameUsed(name)) {
                                int i = 2;
                                while (nameUsed(name+QString::number(i))) {i++; }
                                name = name+QString::number(i);
                        }
                        //inform about other players (handshake complete)
                        if (p->getState() == 0) {
                                QString cookie = QString::number(rand()%100000000);
                                cookie += "/"+QString::number(rand()%100000000);
                                p->setCookie(cookie);
                                playerMap.insert(cookie, p);
                                p->sendClientInfo();
                                QString msg = "";
                                foreach (CapiServerPlayer* pl, players) {
                                        if (pl->getId() != pid) msg += pl->getUpdate("name image game");
                                }
                                foreach (CapiTemplate* tmpl, templates) {
                                        msg += getTemplateUpdate(tmpl->getName(), tmpl->getDesc(), tmpl->getType());
                                }
                                foreach (CapiServerGame* g, games) {
                                        msg += g->getUpdate();
                                }
                                p->send(msg);
                        }
                        p->setName(name);
                        p->setState(1);

                        sendToAll(p->getUpdate("name host image game"));
                        registerGator();
                } else if ((p->getState() == 1) && msg.startsWith(".pi")) {
                        //Ability to chance image
                        QString av = msg.remove(0,3);
                        p->setAvatar(av);
                        p->setState(1);
                        sendToAll(p->getUpdate("image"));
                } else if ((p->getState() == 1) && (msg == ".gl")) {
                        //Ability to get a full list of games
                        QString ret = "";
                        foreach (CapiTemplate* templ, templates)
                                ret += getTemplateUpdate(templ->getName(), templ->getDesc(), templ->getType());
                        foreach (CapiServerGame* g, games)
                                ret += g->getUpdate();
                        p->send(ret);
                } else if ((p->getState() == 1) && msg.startsWith(".gn")) {
                        //Ability to create a game
                        if (p->getGame() == 0) {
                                QString type = msg.remove(0,3);

                                CapiServerGame* g = createGame(getTemplate(type));
                                if (g != 0) {
                                        g->setMaster(p);
                                        g->addPlayer(p);
                                        p->setGame(g);
                                        sendToAll(g->getUpdate());
                                        sendToAll(p->getUpdate("game"));
                                        p->send(g->getConfigUpdate(-1, p->getLang()));
                                } else {
                                        QMap<QString, QString> vars;
                                        vars.insert("%{type}", type);
                                        p->sendMsg(translator->translateMessage("Cannot create game '%{type}'", p->getLang(), &vars), "error");
                                }
                        } else {
                                p->sendMsg(translator->translateMessage("You cannot create a game, when you are within a game.", p->getLang(), 0), "error");
                        }
                } else if ((p->getState() == 1) && msg.startsWith(".gc")) {
                        //Ability to change config
                        bool ok;
                        QStringList parts = msg.split(QString(":"));
                        QString msgCid = parts.value(0);
                        msgCid = msgCid.remove(0,3);
                        QString msgVal = parts.value(1);
                        int cid = msgCid.toInt(&ok);
                        if (ok) {
                                CapiServerGame* g = (CapiServerGame*) p->getGame();
                                if (g != 0) {
                                        if (p == g->getMaster()) {
                                                g->setConfig(cid, msgVal);
                                                foreach (CapiServerPlayer* pl, players) {
                                                        if (pl->getGame() == g)
                                                                pl->send(g->getConfigUpdate(cid, pl->getLang()));
                                                }
                                        } else {
                                                p->sendMsg(translator->translateMessage("You are not Master of the game", p->getLang(), 0), "error");
                                        }
                                }
                        } else {
                                QMap<QString, QString> vars;
                                vars.insert("%{cid}", msgCid);
                                p->sendMsg(translator->translateMessage("Invalid configid: %{cid}", p->getLang(), &vars), "error");
                        }
                } else if ((p->getState() == 1) && msg.startsWith(".gd")) {
                        //Ability to change game description
                        QString desc = msg.remove(0,3);
                        CapiServerGame* g = (CapiServerGame*) p->getGame();
                        if (g != 0) {
                                if (p->getId() == g->getMaster()->getId()) {
                                        g->setDesc(desc);
                                        sendToAll(g->getUpdate("description"));
                                } else {
                                        p->sendMsg(translator->translateMessage("You are not Master of the game",p->getLang(), 0), "error");
                                }
                        }
                } else if ((p->getState() == 1) && msg.startsWith(".gj")) {
                        //Ability to join a game
                        int gid = 0;
                        bool ok;
                        gid = msg.remove(0,3).toInt(&ok);
                        if (ok) {
                                CapiServerGame* g = getGameById(gid);
                                if ((p->getGame() == 0) && (g != 0)) {
                                        if (g->getNumPlayer() < g->getMaxPlayer()) {
                                                if (g->getCanBeJoined()) {
                                                        QString msg = "";
                                                        g->addPlayer(p);
                                                        p->setGame(g);
                                                        p->setSpectator(false);
                                                        msg += p->getUpdate("game spectator");
                                                        msg += g->getUpdate("players");
                                                        sendToAll(msg);
                                                        p->send(g->getConfigUpdate());
                                                } else {
                                                        p->sendMsg(translator->translateMessage("You cannot join the game.", p->getLang(),0), "error");
                                                }
                                        } else {
                                                p->sendMsg(translator->translateMessage("Game is already full.", p->getLang(),0), "error");
                                        }
                                } else {
                                        p->sendMsg(translator->translateMessage("Invalid gameid or you are already in a game.",p->getLang(),0), "error");
                                }
                        } else {
                                p->sendMsg(translator->translateMessage("Invalid gameid.",p->getLang(),0), "error");
                        }
                } else if ((p->getState() == 1) && msg.startsWith(".gS")) {
                        //Ability to join a game as spectator

                        p->sendMsg(translator->translateMessage("Sorry, Capid does not support spectators at the moment.",p->getLang(),0), "error");
                        /*
                          int gid = 0;
                          bool ok;
                          gid = msg.remove(0,3).toInt(&ok);
                          if (ok) {
                          CapiServerGame* g = getGameById(gid);
                          if ((p->getGame() == 0) && (g != 0)) {
                          g->addPlayer(p);
                          p->setGame(g);
                          p->setSpectator(true);
                          sendToAll(p->getUpdate("game spectator"));
                          p->send(g->getConfigUpdate());
                          } else {
                          p->sendMsg("Invalid gameid or you arer already in a game.", "error");
                          }
                          } else {
                          p->sendMsg("Invalid gameid.", "error");
                          }
                        */
                } else if ((p->getState() == 1) && msg.startsWith(".gx")) {
                        //Ability to leave a game
                        CapiServerGame* g = (CapiServerGame*) p->getGame();
                        if (g != 0) {
                                QString updateStr = g->removePlayerFromGame(p);
                                p->setGame(0);
                                sendToAll(p->getUpdate("game")+g->getUpdate("players"));
                                if (g->getMaster() == 0) {
                                        //Is the last player hs gone, delete the game
                                        games.removeAll(g);
                                        sendToAll(QString("<deletegame gameid=\"%1\" />").arg(g->getId()));
                                        delete g;
                                } else {
                                        sendToAll(updateStr, g->getId());
                                        sendToAll(g->getUpdate());
                                }
                        } else {
                                p->sendMsg(translator->translateMessage("You are not in a game.",p->getLang(), 0), "error");
                        }
                } else if ((p->getState() == 1) && msg.startsWith(".gs")) {
                        //Ablility to start a game
                        CapiServerGame* g = (CapiServerGame*) p->getGame();
                        if (g != 0) {
                                if (g->getNumPlayer() >= g->getMinPlayer()) {
                                        g->startGame();
                                } else {
                                        QMap<QString, QString> vars;
                                        vars.insert("%{#players}", QString::number(g->getMinPlayer()));
                                        p->sendMsg(translator->translateMessage("You need at least %{#players} players to start the game.", p->getLang(), &vars), "error");
                                }
                        } else {
                                p->sendMsg(translator->translateMessage("You are not in a game.", p->getLang(), 0), "error");
                        }
                } else if (msg.startsWith(".d")) {
                        //Ability to disconnect
                        CapiServerGame* g = (CapiServerGame*) p->getGame();
                        if (g != 0) {
                                g->removePlayerFromGame(p);
                        }
                        deletePlayer(pid);
                        sendToAll(QString("<deleteplayer playerid=\"%1\" />").arg(pid));
                        registerGator();
                } else if (msg.startsWith(".R")) {
                        //Ability to reconnect, if needed
                        QString cookie = msg.remove(0,2);
                        CapiServerPlayer* player = playerMap.value(cookie);
                        if (player != 0) {
                                if (player->getState() == 2) {
                                        //make a reconnect
                                        player->setState(1);
                                        player->setSocket(p->getSocket());
                                        p->setSocket(0);
                                        players.removeAll(p);
                                        playerMap.remove(p->getCookie());
                                        sendToAll(QString("<deleteplayer playerid=\"%1\" />").arg(p->getId()));
                                        delete p;
                                        player->sendClientInfo();
                                        player->send(((CapiServerGame*) player->getGame())->getFullStatus(player->getLang()));
                                        player->sendMsg(translator->translateMessage("Reconnected", player->getLang(), 0), "info");

                                        //TODO: if players need to make Tax-decisojn, ask him/her here for it
                                }
                        }
                } else if (msg == ".f") {
                        QString msg = "";
                        foreach (CapiTemplate* tmpl, templates)
                                msg += getTemplateUpdate(tmpl->getName(), tmpl->getDesc(), tmpl->getType());
                        foreach (CapiServerGame* g, games)
                                msg += g->getUpdate();
                        foreach (CapiServerPlayer* p, players)
                                msg += p->getUpdate("name game image");
                        if (p->getGame() != 0) msg +=((CapiServerGame*) p->getGame())->getFullStatus(p->getLang());

                        p->send(msg);
                } else if (msg.startsWith(".l")) {
                        p->setLang(msg.remove(0,2));
                } else {
                        //Commands within a game
                        if (p->getGame() != 0) {
                                CapiServerGame* g = (CapiServerGame*) p->getGame();
                                g->command(msg, pid);
                        } else {
                                p->sendMsg(translator->translateMessage("Unknown command.", p->getLang(), 0), "error");
                        }
                }
        } else {
                if (p->getState() != 1) return;
                //chat message
                int gid = -1;
                if (p->getGame() != 0) gid = p->getGame()->getId();

                QString snd = "<msg type=\"chat\" value=\""+maskXML(msg)+"\" playerid=\""+QString::number(pid)+"\" />";
                sendToAll(snd, gid);
        }
}

void CapiServer::registerGator() {
        return;

        //Atm there is no active gator, so code is diabled
        /*
          secsSinceLastRegister = 0;
          if (!registerAtGators) return;

          QString hostIP = QHostInfo::fromName(host).addresses().value(0).toString();
          qDebug() << hostIP;

          QString url = QString("http://monopd-gator.kde.org/register.php?ip=%1&host=%2&port=%3&version=%4&users=%5");
          url = url.arg(hostIP, host, port, version, QString::number(players.size()));

          QNetworkRequest req;
          req.setUrl(QUrl(url));
          QNetworkReply* reply = 0;
          if (man != 0)
          reply = man->get(req);

          if (oldReply != 0) delete oldReply;
          oldReply = reply;
          qDebug() << "Registered";
        */
}

CapiTemplate* CapiServer::getTemplate(QString type) {
        foreach(CapiTemplate* tmpl, templates) {
                if (tmpl->getType() == type) return tmpl;
        }
        return 0;
}

CapiServerGame* CapiServer::createGame(CapiTemplate* tmpl) {
        if (tmpl == 0) return 0;
        QFile file(QDir::currentPath()+"/games/"+tmpl->getType()+".xml");
        file.open(QIODevice::ReadOnly);

        CapiServerGame* g = new CapiServerGame(++gID, translator);
        g->setName(tmpl->getName());
        g->setDesc(tmpl->getDesc());
        g->setType(tmpl->getName());
        QDomDocument doc;
        doc.setContent(&file);

        g->parseGame(doc);

        games.append(g);

        return g;
}

bool CapiServer::isListening() {
        return serv->isListening();
}

QString CapiServer::getTemplateUpdate(QString name, QString desc, QString type) {
        return QString("<gameupdate gameid=\"-1\" name=\"%1\" description=\"%2\" gametype=\"%3\" />").arg(name, desc, type);
}
