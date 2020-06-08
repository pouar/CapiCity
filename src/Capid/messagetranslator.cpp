// Copyright 2010 Thomas Kamps anubis1@linux-ecke.de
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

#include "messagetranslator.h"

#include <QDir>
#include <QFile>
#include <QApplication>

MessageTranslator::MessageTranslator() {
}

MessageTranslator::~MessageTranslator() {
        while (translators.size() > 0) {
                QMap<QString, QString>* trans = translators.value(translators.keys().value(0));
                translators.remove(translators.keys().value(0));
                delete trans;
        }
}

void MessageTranslator::loadLang(QString lang) {
        //First check, if the lang exits. if not create it.
        QMap<QString,QString>* trans = translators.value(lang);
        if (trans == 0) {
                trans = new QMap<QString, QString>();
                translators.insert(lang, trans);
        }

        //Now load the Lang file
        //Get Server-data from config file
        QFile cfile(QApplication::applicationDirPath()+"/ts/Capid_"+lang+".trans");
        if (!cfile.exists()) return;

        //Read translations
        cfile.open(QIODevice::ReadOnly);
        while (!cfile.atEnd()) {
                QString line = cfile.readLine();
                line = line.replace("\n", "");

                //Ignore comments and empty lines
                if (line == "") continue;
                if (line.startsWith("#")) continue;

                QString line2 = cfile.readLine();
                line2 = line2.replace("\n", "");
                trans->insert(line, line2);
        }
}

QString MessageTranslator::translateMessage(QString text, QString lang, QMap<QString, QString>* args) {
        return replaceVars(translate(text, lang), args);
}

QString MessageTranslator::translate(QString text, QString lang) {
        QMap<QString, QString>* trans = translators.value(lang);
        if (trans != 0) {
                QString trText = trans->value(text);
                if (!trText.isNull()) return trText;
        }
        return text;
}

QString MessageTranslator::replaceVars(QString text, QMap<QString, QString>* args) {
        QString ret = text;

        if (args == 0) return ret;
        foreach (QString k, args->keys()) {
                ret = ret.replace(k, args->value(k));
        }
        return ret;
}
