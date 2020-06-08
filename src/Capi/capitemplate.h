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

#ifndef CAPITEMPLATE_H
#define CAPITEMPLATE_H

#include <QString>
#include "capiobject.h"

class CapiTemplate : public CapiObject {

public:
        CapiTemplate();
        QString getType();
        QString getDesc();

        void setType(QString t);
        void setDesc(QString d);

protected:
        QString type;
        QString desc;
};
#endif // CAPITEMPLATE_H
