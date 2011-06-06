/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of the QGROUNDCONTROL project

    QGROUNDCONTROL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QGROUNDCONTROL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
 * @file
 *   @brief Definition of class MeshStatus
 *   @author Matthias Egli <mavteam@student.ethz.ch>
 *
 */

#include "MeshStatus.h"
#include "ui_MeshStatus.h"

#include <QDebug>
#include <stdlib.h>
#include <string>
#include <iostream>



MeshStatus::MeshStatus(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MeshStatus)
{
    ui->setupUi(this);

    connect(ui->updatePushButton, SIGNAL(clicked()), this, SLOT(update()));

    telnetConnection = new QtTelnet();

    //Initialize AutoUpdate timer
    autoUpdateTimer = new QTimer(this);
    connect(autoUpdateTimer,SIGNAL(timeout()),this,SLOT(update()));
    if (ui->autoUpdateCheckBox->checkState() > 0) {
        autoUpdateTimer->start(1000);
    }

    connect(telnetConnection,SIGNAL(message(const QString &)),this,SLOT(telnetMessage(const QString &)));
    connect(ui->autoUpdateCheckBox, SIGNAL(stateChanged(int)),this,SLOT(autoUpdate(int)));

}

void MeshStatus::update()
{
    telnetConnection->connectToHost("localhost",2004);
}

void MeshStatus::autoUpdate(int state)
{
    if (state == 0) {
        autoUpdateTimer->stop();
    } else {
        autoUpdateTimer->start(1000);
    }
}

void MeshStatus::telnetMessage(const QString &msg)
{
    QStringList connectionList = msg.split("\n");

    //Regexp which matches this line: "82.130.102.52" -> "82.130.103.111"[label="1.094"];
    QString re2="\"";	// Any Single Character 2
    QString re3="((?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?))(?![\\d])";	// IPv4 IP Address 1
    QString re4=".*";	// Non-greedy match on filler
    QString re5="((?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?))(?![\\d])";	// IPv4 IP Address 2
    QString re6=".*";	// Non-greedy match on filler
    QString re7="([+-]?\\d*\\.\\d+)(?![-+0-9\\.])";	// Float 1
    QString re8="\"";	// Any Single Character 6
    QString re9="\\]";	// Any Single Character 7
    QString re10=";";	// Any Single Character 8

    QRegExp rx(re2+re3+re4+re5+re6+re7+re8+re9+re10);

    //Delete all rows in the TableWidget
    for (int i = 0; i<ui->connectionStatusTableWidget->rowCount(); i++)
    {
        ui->connectionStatusTableWidget->removeRow(0);
    }

    //Match Strings against RegExp, extract Source, Dest and Quality and display it inside the MeshStatus Widget
    foreach (QString str,connectionList) {
        if (rx.indexIn(str)==0)
        {
            QString source = rx.capturedTexts()[1];
            QString dest = rx.capturedTexts()[2];
            QString quality = rx.capturedTexts()[3];
            ui->connectionStatusTableWidget->insertRow(ui->connectionStatusTableWidget->rowCount());
            QTableWidgetItem *newItem1 = new QTableWidgetItem(source);
            ui->connectionStatusTableWidget->setItem(ui->connectionStatusTableWidget->rowCount()-1, 0, newItem1);
            QTableWidgetItem *newItem2 = new QTableWidgetItem(dest);
            ui->connectionStatusTableWidget->setItem(ui->connectionStatusTableWidget->rowCount()-1, 2, newItem2);
            QTableWidgetItem *newItem3 = new QTableWidgetItem(quality);
            ui->connectionStatusTableWidget->setItem(ui->connectionStatusTableWidget->rowCount()-1, 1, newItem3);
        }
    }



    //qDebug() << connectionList;
}

MeshStatus::~MeshStatus()
{
    delete ui;
}
