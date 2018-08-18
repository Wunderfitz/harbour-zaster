/*
    Copyright (C) 2018 Sebastian J. Wolf

    This file is part of Zaster.

    Zaster is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Zaster is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Zaster. If not, see <http://www.gnu.org/licenses/>.
*/

#include "zaster.h"
#include "fints/fintsdeserializer.h"
#include "fints/message.h"

Zaster::Zaster(QObject *parent) : QObject(parent), settings("harbour-zaster", "settings")
{
    this->networkAccessManager = new QNetworkAccessManager(this);
    wagnis = new Wagnis(this->networkAccessManager, "harbour-zaster", "0.1", this);
    doStupidTests();
}

Zaster::~Zaster()
{
}

Wagnis *Zaster::getWagnis()
{
    return this->wagnis;
}

void Zaster::doStupidTests()
{
    qDebug() << "Starting stupid tests...";
    FinTsDeserializer deserializer;
    QString rawMessage = "HNHBK:1:3+000000000125+300+0+1'HKIDN:2:2+280:67292200+9999999999+0+0'HKVVB:3:3+0+0+0+36792786FA12F235F04647689+3'HNHBS:4:1+1'";
    Message *testMessage = deserializer.decodeAndDeserialize(rawMessage.toLatin1());
    qDebug() << "Message decoded :)";
    testMessage->deleteLater();
}
