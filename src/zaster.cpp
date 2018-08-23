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
#include "fints/fintsserializer.h"
#include "fints/fintsdialog.h"
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

void Zaster::handleStupidTestsError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "Zaster::handleStupidTestsError:" << (int)error << reply->errorString() << reply->readAll();
}

void Zaster::handleStupidTestsFinished()
{
    qDebug() << "Zaster::handleStupidTestsFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    FinTsDeserializer deserializer;
    Message *replyMessage = deserializer.decodeAndDeserialize(reply->readAll());
    deserializer.debugOut(replyMessage);
    replyMessage->deleteLater();
}

void Zaster::doStupidTests()
{
    qDebug() << "Starting stupid tests...";
    FinTsDialog finTsDialog;
    Message *dialogInitializationMessage = finTsDialog.createDialogInitializationMessage();
    FinTsSerializer serializer;
    QByteArray serializedInitializationMessage = serializer.serializeAndEncode(dialogInitializationMessage);
    qDebug() << "INPUT" << serializedInitializationMessage;

    QUrl url = QUrl("https://hbci11.fiducia.de/cgi-bin/hbciservlet");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    QNetworkReply *reply = networkAccessManager->post(request, serializedInitializationMessage);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleStupidTestsError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleStupidTestsFinished()));

    dialogInitializationMessage->deleteLater();

}
