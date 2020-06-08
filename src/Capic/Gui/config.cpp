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

#include "config.h"
#include "ui_config.h"
#include "src/defines.h"

#include <QFileDialog>
#include <QBuffer>

Config::Config(QWidget *parent) : QDialog(parent), m_ui(new Ui::Config) {
        m_ui->setupUi(this);

        m_ui->cbImage->addItem(QIcon(":/images/avatars/badge.png"),         "", QVariant("badge.png"));
        m_ui->cbImage->addItem(QIcon(":/images/avatars/beachball.png"),     "", QVariant("beachball.png"));
        m_ui->cbImage->addItem(QIcon(":/images/avatars/bell.png"),          "", QVariant("bell.png"));
        m_ui->cbImage->addItem(QIcon(":/images/avatars/bomb.png"),          "", QVariant("bomb.png"));
        m_ui->cbImage->addItem(QIcon(":/images/avatars/cat.png"),           "", QVariant("cat.png"));
        m_ui->cbImage->addItem(QIcon(":/images/avatars/cookie.png"),        "", QVariant("cookie.png"));
        m_ui->cbImage->addItem(QIcon(":/images/avatars/cube.png"),          "", QVariant("cube.png"));
        m_ui->cbImage->addItem(QIcon(":/images/avatars/eyeball.png"),       "", QVariant("eyeball.png"));
        m_ui->cbImage->addItem(QIcon(":/images/avatars/flag.png"),          "", QVariant("flag.png"));
        m_ui->cbImage->addItem(QIcon(":/images/avatars/ghost.png"),         "", QVariant("ghost.png"));
        m_ui->cbImage->addItem(QIcon(":/images/avatars/globe.png"),         "", QVariant("globe.png"));
        m_ui->cbImage->addItem(QIcon(":/images/avatars/hamburger.png"),     "", QVariant("hamburger.png"));
        m_ui->cbImage->addItem(QIcon(":/images/avatars/lips.png"),          "", QVariant("lips.png"));
        m_ui->cbImage->addItem(QIcon(":/images/avatars/puzzle.png"),        "", QVariant("puzzle.png"));
        m_ui->cbImage->addItem(QIcon(":/images/avatars/pyramid.png"),       "", QVariant("pyramid.png"));
        m_ui->cbImage->addItem(QIcon(":/images/avatars/skull.png"),         "", QVariant("skull.png"));
        m_ui->cbImage->addItem(QIcon(":/images/avatars/traffic_light.png"), "", QVariant("traffic_light.png"));
        m_ui->cbImage->addItem(QIcon(":/images/avatars/wizard.png"),        "", QVariant("wizard.png"));

        //SEt notification-checkbox
        m_ui->cbNotify->setChecked(s.value(CONFIG_WINDOW_SHOWNOTIFICATION, CONFIG_WINDOW_SHOWNOTIFICATION_DEFAULT).toBool());

        //Set Player infos
        QString image = s.value(CONFIG_PLAYER_IMAGE, CONFIG_PLAYER_IMAGE_DEFAULT).toString();
        if (image.startsWith("DATA")) loadImageFromData(image);

        int i = m_ui->cbImage->findData(QVariant(image));
        if (i < 0) i = 0;
        m_ui->cbImage->setCurrentIndex(i);
        m_ui->cbImage->setIconSize(QSize(32,32));

        QString name = s.value(CONFIG_PLAYER_NAME, CONFIG_PLAYER_NAME_DEFAULT).toString();
        m_ui->tfName->setText(name);
        connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(save()));

        //Set appearance config
        m_ui->cbAnimate->setChecked(s.value(CONFIG_GAME_ANIMATE, CONFIG_GAME_ANIMATE_DEFAULT).toBool());
        m_ui->wAnimTime->setEnabled(m_ui->cbAnimate->isChecked());
        m_ui->cbEmos->setChecked(s.value(CONFIG_CHAT_SHOWEMOS, CONFIG_CHAT_SHOWEMOS_DEFAULT).toBool());

        connect(m_ui->bOpenFile, SIGNAL(clicked()), this, SLOT(openFile()));
}

Config::~Config() {
        delete m_ui;
}

void Config::save() {
        //Save name and widget
        QString oldName = s.value(CONFIG_PLAYER_NAME, CONFIG_PLAYER_NAME_DEFAULT).toString();
        QString newName = m_ui->tfName->text();
        s.setValue(CONFIG_PLAYER_NAME, newName);

        int i = m_ui->cbImage->currentIndex();
        QString image = m_ui->cbImage->itemData(i).toString();
        s.setValue(CONFIG_PLAYER_IMAGE, image);

        s.setValue(CONFIG_WINDOW_SHOWNOTIFICATION, m_ui->cbNotify->isChecked());

        if (newName != oldName)
                emit(saved("name", newName));

        emit(saved("image", image));

        s.setValue(CONFIG_GAME_ANIMATE, m_ui->cbAnimate->isChecked());
        s.setValue(CONFIG_GAME_ANIMATIONTIME, m_ui->slAnimTime->value());
        s.setValue(CONFIG_CHAT_SHOWEMOS, m_ui->cbEmos->isChecked());
}

void Config::openFile() {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Select image"), QDir::homePath(), "Image Files (*.png *.jpg *.gif *.bmp *.jpeg)");

        QImage image(fileName);
        image = image.scaled(64,64, Qt::KeepAspectRatio);
        //Get Data
        QByteArray data;
        QBuffer buffer(&data);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "PNG"); // writes image into ba in PNG format
        QByteArray hexData = data.toHex();

        QString imgStr = "DATA"+QString::fromUtf8(hexData);

        m_ui->cbImage->addItem(QIcon(QPixmap::fromImage(image)), "", QVariant(imgStr));

        int i = m_ui->cbImage->findData(QVariant(imgStr));
        if (i < 0) i = 0;
        m_ui->cbImage->setCurrentIndex(i);
}

void Config::loadImageFromData(QString data) {
        QString imgData = data.right(data.size()-4);
        QByteArray hexData = imgData.toUtf8();
        QByteArray dataArr = QByteArray::fromHex(hexData);
        QImage image = QImage::fromData(dataArr);
        image = image.scaled(64,64, Qt::KeepAspectRatio);

        m_ui->cbImage->addItem(QIcon(QPixmap::fromImage(image)), "", QVariant(data));
}

void Config::changeEvent(QEvent *e) {
        QDialog::changeEvent(e);

        switch (e->type()) {
        case QEvent::LanguageChange:
                m_ui->retranslateUi(this);
                break;
        default:
                break;
        }
}
