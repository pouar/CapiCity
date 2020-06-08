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

#include "notifier.h"
#include "ui_notifier.h"
#include <QTimer>
#include <QDesktopWidget>
#include <QPainter>

Notifier::Notifier(QWidget *parent) : QWidget(parent), ui(new Ui::Notifier) {
	ui->setupUi(this);
	QPalette p = this->palette();
	c = p.brush(QPalette::Window).color();
	c.setAlpha(192);
	setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
	setAttribute(Qt::WA_TranslucentBackground);

	timer = new QTimer(this);
	timer->setSingleShot(true);

	connect(timer, SIGNAL(timeout()), this, SLOT(hideNotify()));
	connect(ui->bClose, SIGNAL(clicked()), this, SLOT(hideNotify()));

	QRect screen = QApplication::desktop()->availableGeometry(this);
	int x = (screen.width() - this->width());
	int y = (screen.height() - this->height());
	this->move(x,y);
}

Notifier::~Notifier() {
	delete ui;
	delete timer;
}

void Notifier::changeEvent(QEvent *e) {
	QWidget::changeEvent(e);

	switch (e->type()) {
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void Notifier::paintEvent(QPaintEvent *e) {
	QPainter p(this);
	p.fillRect(rect(), c);
	QWidget::paintEvent(e);
}

void Notifier::showNotify(int secs) {
	this->show();
	timer->setTimerType(Qt::PreciseTimer);
	timer->start(1000*secs);
}

void Notifier::hideNotify() {
	if (timer->isActive()) timer->stop();
	this->hide();
}
