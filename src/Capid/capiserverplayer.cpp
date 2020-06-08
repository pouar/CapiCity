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

#include "capiserverplayer.h"
#include <QList>
#include "capiserver.h"

CapiServerPlayer::CapiServerPlayer(int id) : QObject(), CapiPlayer(id) {
	state = 0; //Fresh generated.
	sock = 0;
	moved = false;
	doubles = 0;

	secondsDisconnected = 0;

	lang="en";
}

CapiServerPlayer::~CapiServerPlayer() {
	if (sock != 0) {
		sock->disconnectFromHost();
		//sock->deleteLater();
	}
}

QString CapiServerPlayer::getCookie() {
	return cookie;
}

void CapiServerPlayer::setCookie(QString c) {
	cookie = c;
}

void CapiServerPlayer::send(QString msg) {
	if (sock == 0) return;
	if (!(sock->state() == QAbstractSocket::ConnectedState)) return;
	sock->write(QString("<monopd>"+msg+"</monopd>\n").toUtf8());
	//sock->flush();
	qDebug() << QString::number(getId())+" <-- "+msg;
}

void CapiServerPlayer::sendMsg(QString msg, QString type, int pid) {
	if ((type == "chat") && (pid == -1)) return;

	QString snd = "<msg type=\""+type;
	snd += "\" value=\"";
	snd += CapiServer::maskXML(msg);
	snd += "\" ";

	if (pid != -1) snd += "playerid=\""+QString::number(pid)+"\" ";

	snd += " />";
	send(snd);
}

void CapiServerPlayer::sendClientInfo() {
	send(QString("<client playerid=\"%1\" cookie=\"%2\" />").arg(getId()).arg(getCookie()));
}

void CapiServerPlayer::setSocket(QTcpSocket* s) {
	sock = s;
	if (sock == 0) return;

	connect(sock, SIGNAL(readyRead()), this, SLOT(recieveMessage()));
	connect(sock, SIGNAL(disconnected()), this, SLOT(connectionLost()));
}

QTcpSocket* CapiServerPlayer::getSocket() {
	return sock;
}

void CapiServerPlayer::setState(int s) {
	state = s;
}

int CapiServerPlayer::getState() {
	return state;
}

bool CapiServerPlayer::getMoved() {
	return moved;
}

void CapiServerPlayer::setMoved(bool m) {
	moved = m;
}

void CapiServerPlayer::setPosMoveTo(int pos) {
	posMoveTo = pos;
}

void CapiServerPlayer::resetDoubles() {
	doubles = 0;
}

void CapiServerPlayer::incDoubles() {
	doubles++;
}

int CapiServerPlayer::getDoubles() {
	return doubles;
}

void CapiServerPlayer::resetJailRolls() {
	jailRolls = 0;
}

void CapiServerPlayer::incJailRolls() {
	jailRolls++;
}

int CapiServerPlayer::getJailRolls() {
	return jailRolls;
}

void CapiServerPlayer::tick() {
	if (state == 2) {
		secondsDisconnected++;
		if (secondsDisconnected == 180) {
			//Player did not reconnect within 180s
			//Let him disconnect from game
			state = 3;
			emit(gotMessage(getId(), ".d"));
		}
	}
}

void CapiServerPlayer::recieveMessage() {
	secondsDisconnected = 0;

	QList<QString> msgs;
	while (sock->canReadLine()) {
		QByteArray bytes = sock->readLine();
		QString msg = QString::fromUtf8(bytes);
		/*
		msg = msg.replace("&", "&#38;");
		msg = msg.replace("<", "&#60;");
		msg = msg.replace(">", "&#62;");
		msg = msg.replace("=", "&#61;");
		msg = msg.replace("\"", "&#34;");
		*/
		msg = msg.replace("\n","");
		msg = msg.replace("\r","");

		msgs.append(msg);
	}

	foreach (QString msg, msgs) {
		qDebug() << QString::number(getId())+" --> "+msg;
		emit(gotMessage(getId(), msg));
	}
}

void CapiServerPlayer::connectionLost() {
	if (state == 1) {
		state = 2;
		//sock->deleteLater();
		sock = 0;
		emit(gotMessage(getId(), QString(".t%1").arg(posMoveTo)));
	} else if (state == 0) {
		emit(gotMessage(getId(), ".d"));
	}

}

QString CapiServerPlayer::getLang() {
	return lang;
}

void CapiServerPlayer::setLang(QString lang) {
	this->lang = lang;
}

QString CapiServerPlayer::getUpdate(QString type) {
	QString ret = QString("<playerupdate playerid=\"%1\" ").arg(getId());
	int gid = -1;
	if (getGame() != 0) gid = getGame()->getId();

	if ((type == "") || type.contains("game"))          ret += QString("game=\"%1\" ").arg(gid);
	if ((type == "") || type.contains("name"))          ret += QString("name=\"%1\" ").arg(CapiServer::maskXML(getName()));
	if ((type == "") || type.contains("image"))         ret += QString("image=\"%1\" ").arg(CapiServer::maskXML(getAvatar()));
	if ((type == "") || type.contains("money"))         ret += QString("money=\"%1\" ").arg(getMoney());
	if ((type == "") || type.contains("location"))      ret += QString("location=\"%1\" ").arg(getPos());
	if ((type == "") || type.contains("jailed"))        ret += QString("jailed=\"%1\" ").arg(getInJail());
	if ((type == "") || type.contains("directmove"))    ret += QString("directmove=\"%1\" ").arg(toInt(getDirectmove()));
	if ((type == "") || type.contains("bankrupt"))      ret += QString("bankrupt=\"%1\" ").arg(getBankrupt());
	if ((type == "") || type.contains("hasturn"))       ret += QString("hasturn=\"%1\" ").arg(getHasTurn());
	if ((type == "") || type.contains("can_roll"))      ret += QString("can_roll=\"%1\" ").arg(getCanRoll());
	if ((type == "") || type.contains("can_buyestate")) ret += QString("can_buyestate=\"%1\" ").arg(getCanBuyEstate());
	if ((type == "") || type.contains("spectator"))     ret += QString("spectator=\"%1\" ").arg(getSpectator());
	if ((type == "") || type.contains("hasdept"))       ret += QString("hasdept=\"%1\" ").arg(getHasDept());
	if ((type == "") || type.contains("canauction"))    ret += QString("canauction=\"%1\" ").arg(getCanAuction());
	if ((type == "") || type.contains("canusecard"))    ret += QString("canusecard=\"%1\" ").arg(getCanUseCard());

	ret += "/>";
	return ret;
}
