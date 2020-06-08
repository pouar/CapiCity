#include "capiserverinfo.h"

#include <QDomDocument>

CapiServerInfo::CapiServerInfo(QString adress, int port) {
        this->adress = adress;
        this->port = port;

        sock = new QTcpSocket(0);
}

CapiServerInfo::~CapiServerInfo() {
        if (sock != 0)
                delete sock;
}

QString CapiServerInfo::getAdress() {
        return adress;
}

int CapiServerInfo::getPort() {
        return port;
}

QString CapiServerInfo::getVersion() {
        return version;
}

bool CapiServerInfo::isOnline() {
        //Connects to the server/port and looks what it says.
        //If it says the correct thing ("<monopd><server version="0.9.3"/></monopd>" or "<monopd><server version="* Capid"/></monopd>")
        //its ok. It will disconnect and return true

        bool res = false;
        version = "";
        sock->connectToHost(adress, port);
        if (sock->waitForConnected(400)) {
                if (sock->waitForReadyRead(400)) {
                        QByteArray bytes = sock->readLine();
                        QString line = QString::fromUtf8(bytes.data());
                        if (line.endsWith("\n")) line.chop(1);

                        QDomDocument greeting;
                        greeting.setContent(line);
                        QDomElement rootElement = greeting.documentElement();
                        if (rootElement.tagName() == "monopd") {
                                QDomElement serverElem = rootElement.firstChildElement();
                                if (serverElem.tagName() == "server") {
                                        if (serverElem.hasAttribute("version")) {
                                                version = serverElem.attribute("version", "");
                                                res = true;
                                        }
                                }
                        }
                }
        }

        sock->disconnectFromHost();

        return res;
}
