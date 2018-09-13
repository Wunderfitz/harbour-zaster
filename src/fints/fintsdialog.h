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

#ifndef FINTSDIALOG_H
#define FINTSDIALOG_H

#include <QObject>
#include <QDebug>
#include <QVariantMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include "fintsserializer.h"
#include "fintsdeserializer.h"
#include "dataelementgroup.h"
#include "dataelement.h"
#include "message.h"
#include "segment.h"
#include "fintsglobals.h"
#include "messageconstants.h"
#include "bpdconstants.h"
#include "updconstants.h"

// HBCI-Version - always fixed version 3.0, see Formals, page 15
const char FINTS_VERSION[] = "300";
// Customer system ID must be 0 for PIN/TAN, see Formals page 116
const char CUSTOMER_SYSTEM_ID[] = "0";
// Customer system status, needs to be "1", see Formals page 117
const char CUSTOMER_SYSTEM_STATUS[] = "1";

class FinTsDialog : public QObject
{
    Q_OBJECT
public:
    explicit FinTsDialog(QObject *parent = 0, QNetworkAccessManager *networkAccessManager = 0);

    Q_INVOKABLE void dialogInitialization();
    Q_INVOKABLE void closeDialog();

signals:
    void pinTanSupport(const bool &isSupported);

public slots:

private slots:
    void handleDialogInitializationError(QNetworkReply::NetworkError error);
    void handleDialogInitializationFinished();
    void handleDialogEndError(QNetworkReply::NetworkError error);
    void handleDialogEndFinished();

private:    

    Message *createMessageDialogInitialization();
    void parseReplyDialogInitialization(Message *replyMessage);
    Message *createMessageCloseDialog();
    void parseReplyCloseDialog(Message *replyMessage);

    Segment *createSegmentMessageHeader(FinTsElement *parentElement, int segmentNumber, QString dialogId, int messageNumber);
    Segment *createSegmentIdentification(FinTsElement *parentElement, int segmentNumber, const QString &blz);
    Segment *createSegmentProcessPreparation(FinTsElement *parentElement, int segmentNumber);
    Segment *createSegmentMessageTermination(FinTsElement *parentElement, int segmentNumber, int messageNumber);
    Segment *createSegmentDialogEnd(FinTsElement *parentElement, int segmentNumber);
    Segment *createSegmentSignatureHeader(FinTsElement *parentElement, int segmentNumber);
    Segment *createSegmentSignatureFooter(FinTsElement *parentElement, int segmentNumber);
    Segment *createSegmentEncryptionHeader(FinTsElement *parentElement, int segmentNumber);
    Segment *createSegmentEncryptedData(FinTsElement *parentElement, int segmentNumber, const QString &encryptedData);
    void parseSegmentMessageHeader(Segment *segmentMessageHeader);
    void parseSegmentMessageFeedback(Segment *segmentMessageFeedback);
    void parseSegmentSegmentFeedback(Segment *segmentSegmentFeedback);
    void parseSegmentBankParameter(Segment *segmentBankParameter);
    void parseSegmentSecurityProcedure(Segment *segmentSecurityProcedure);
    void parseSegmentUserParameterData(Segment *segmentUserParameterData);
    void parseSegmentAccountInformation(Segment *segmentaccountInformation);
    Message *parseSegmentEncryptedMessage(Segment *segmentEncryptedMessage);

    DataElementGroup *createDegSegmentHeader(FinTsElement *parentElement, const QString &segmentId, const QString &segmentNumber, const QString &segmentVersion);
    DataElementGroup *createDegBankId(FinTsElement *parentElement, const QString &blz);
    DataElementGroup *createDegSecurityProfile(FinTsElement *parentElement);
    DataElementGroup *createDegSecurityIdentificationDetails(FinTsElement *parentElement);
    DataElementGroup *createDegDateTime(FinTsElement *parentElement);
    DataElementGroup *createDegHashAlgorithm(FinTsElement *parentElement);
    DataElementGroup *createDegSignatureAlgorithm(FinTsElement *parentElement);
    DataElementGroup *createDegKeyName(FinTsElement *parentElement, const QString &keyType);
    DataElementGroup *createDegEncryptionAlgorithm(FinTsElement *parentElement);

    void insertMessageLength(Message *message);
    QString convertToBinaryFormat(const QString &originalString);
    Message *packageMessage(Message *originalMessage);

    QNetworkAccessManager *networkAccessManager;
    FinTsSerializer serializer;
    FinTsDeserializer deserializer;
    QString myDialogId;
    int myMessageNumber;
    bool anonymousDialog;
    QVariantMap bankParameterData;
    QVariantMap userParameterData;

    bool debugStop;
};

#endif // FINTSDIALOG_H
