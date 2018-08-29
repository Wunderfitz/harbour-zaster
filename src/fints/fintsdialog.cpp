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

#include "fintsdialog.h"
#include <QListIterator>

FinTsDialog::FinTsDialog(QObject *parent, QNetworkAccessManager *networkAccessManager) : QObject(parent)
{
    this->networkAccessManager = networkAccessManager;

    // Dialog-ID - first message is always "0", see Formals, page 109, TODO: use the received ID from bank later!
    this->myDialogId = "0";
    // Message number - first message is always "1", but is increased at message creation, see Formals, page 120 // TODO: increment later!
    this->myMessageNumber = 0;
    // Dialog language, needs to be "0" for Standard, see Formals page 109
    this->bankParameterData.insert(BPD_KEY_SUPPORTED_LANGUAGE, "0");

    // Bank Parameter Data (BPD) version, needs to be "0" for the initial call, see Formals page 45, TODO: make it adopting to received BPD
    this->bankParameterData.insert(BPD_KEY_VERSION, "0");
    // Country code according to  ISO 3166-1, 280 is fixed for Germany (instead of 276), see Geschäftsvorfälle page 613
    this->bankParameterData.insert(BPD_KEY_COUNTRY_CODE, "280");
    // User Parameter Data (UPD) version, TODO: make it adopting to received UPD
    this->userParameterData.insert(UPD_KEY_VERSION, "0");
}

void FinTsDialog::dialogInitialization()
{
    qDebug() << "FinTsDialog::dialogInitialization";
    Message *dialogInitializationMessage = this->createMessageDialogInitialization();
    QByteArray serializedInitializationMessage = serializer.serializeAndEncode(dialogInitializationMessage);

    // TODO: Use automatially determined endpoint
    QUrl url = QUrl(FINTS_PLACEHOLDER_ENDPOINT);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    QNetworkReply *reply = networkAccessManager->post(request, serializedInitializationMessage);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleDialogInitializationError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleDialogInitializationFinished()));

    dialogInitializationMessage->deleteLater();
}

void FinTsDialog::closeDialog()
{
    qDebug() << "FinTsDialog::closeDialog";
    Message *closeDialogMessage = this->createMessageCloseDialog();
    QByteArray serializedCloseDialogMessage = serializer.serializeAndEncode(closeDialogMessage);

    // TODO: Use automatially determined endpoint
    QUrl url = QUrl(FINTS_PLACEHOLDER_ENDPOINT);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    QNetworkReply *reply = networkAccessManager->post(request, serializedCloseDialogMessage);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleDialogEndError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleDialogEndFinished()));

}

void FinTsDialog::handleDialogInitializationError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "FinTsDialog::handleDialogInitializationError:" << (int)error << reply->errorString() << reply->readAll();
}

void FinTsDialog::handleDialogInitializationFinished()
{
    qDebug() << "FinTsDialog::handleDialogInitializationFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    Message *replyMessage = deserializer.decodeAndDeserialize(reply->readAll());
    parseReplyDialogInitialization(replyMessage);
    closeDialog();
    replyMessage->deleteLater();
}

void FinTsDialog::handleDialogEndError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "FinTsDialog::handleDialogEndError:" << (int)error << reply->errorString() << reply->readAll();
}

void FinTsDialog::handleDialogEndFinished()
{
    qDebug() << "FinTsDialog::handleDialogEndFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    Message *replyMessage = deserializer.decodeAndDeserialize(reply->readAll());
    parseReplyCloseDialog(replyMessage);
    replyMessage->deleteLater();
}

Message *FinTsDialog::createMessageDialogInitialization()
{
    qDebug() << "FinTsDialog::createMessageDialogInitialization";
    this->myMessageNumber++;
    Message *dialogInitializationMessage = new Message();
    dialogInitializationMessage->addSegment(createSegmentMessageHeader(dialogInitializationMessage, dialogInitializationMessage->getNextSegmentNumber(), this->myDialogId, this->myMessageNumber));
    // Usually it's the German "Bankleitzahl" or BLZ, see Geschäftsvorfälle page 608, TODO: Don't use hard-coded BLZ :D
    dialogInitializationMessage->addSegment(createSegmentIdentification(dialogInitializationMessage, dialogInitializationMessage->getNextSegmentNumber(), FINTS_PLACEHOLDER_BLZ));
    dialogInitializationMessage->addSegment(createSegmentProcessPreparation(dialogInitializationMessage, dialogInitializationMessage->getNextSegmentNumber()));
    dialogInitializationMessage->addSegment(createSegmentMessageTermination(dialogInitializationMessage, dialogInitializationMessage->getNextSegmentNumber(), this->myMessageNumber));

    insertMessageLength(dialogInitializationMessage);
    return dialogInitializationMessage;
}

void FinTsDialog::parseReplyDialogInitialization(Message *replyMessage)
{
    qDebug() << "FinTsDialog::parseDialogInitializationReply";
    QListIterator<Segment *> segmentIterator(replyMessage->getSegments());
    while (segmentIterator.hasNext()) {
        Segment *currentSegment = segmentIterator.next();
        QString segmentIdentifier = currentSegment->getIdentifier();
        if (segmentIdentifier == SEGMENT_MESSAGE_HEADER_ID) { parseSegmentMessageHeader(currentSegment); }
        if (segmentIdentifier == SEGMENT_MESSAGE_FEEDBACK_ID) { parseSegmentMessageFeedback(currentSegment); }
        if (segmentIdentifier == SEGMENT_SEGMENT_FEEDBACK_ID) { parseSegmentSegmentFeedback(currentSegment); }
        if (segmentIdentifier == SEGMENT_BANK_PARAMETER_ID) { parseSegmentBankParameter(currentSegment); }
        if (segmentIdentifier == SEGMENT_SECURITY_PROCEDURE_ID) { parseSegmentSecurityProcedure(currentSegment); }
    }
}

Message *FinTsDialog::createMessageCloseDialog()
{
    qDebug() << "FinTsDialog::createMessageCloseDialog";
    Message *closeDialogMessage = new Message();
    this->myMessageNumber++;
    closeDialogMessage->addSegment(createSegmentMessageHeader(closeDialogMessage, closeDialogMessage->getNextSegmentNumber(), this->myDialogId, this->myMessageNumber));
    closeDialogMessage->addSegment(createSegmentDialogEnd(closeDialogMessage, closeDialogMessage->getNextSegmentNumber()));
    closeDialogMessage->addSegment(createSegmentMessageTermination(closeDialogMessage, closeDialogMessage->getNextSegmentNumber(), this->myMessageNumber));

    insertMessageLength(closeDialogMessage);
    return closeDialogMessage;
}

void FinTsDialog::parseReplyCloseDialog(Message *replyMessage)
{
    qDebug() << "FinTsDialog::parseReplyCloseDialog";
    QListIterator<Segment *> segmentIterator(replyMessage->getSegments());
    while (segmentIterator.hasNext()) {
        Segment *currentSegment = segmentIterator.next();
        QString segmentIdentifier = currentSegment->getIdentifier();
        if (segmentIdentifier == SEGMENT_MESSAGE_FEEDBACK_ID) { parseSegmentMessageFeedback(currentSegment); }
    }
}

// See Formals, page 15
Segment *FinTsDialog::createSegmentMessageHeader(FinTsElement *parentElement, int segmentNumber, QString dialogId, int messageNumber)
{
    Segment *messageHeaderSegment = new Segment(parentElement);
    messageHeaderSegment->setHeader(createDegSegmentHeader(messageHeaderSegment, SEGMENT_MESSAGE_HEADER_ID, QString::number(segmentNumber), SEGMENT_MESSAGE_HEADER_VERSION));

    messageHeaderSegment->addDataElement(new DataElement(messageHeaderSegment, MESSAGE_LENGTH_PLACEHOLDER));
    messageHeaderSegment->addDataElement(new DataElement(messageHeaderSegment, FINTS_VERSION));
    messageHeaderSegment->addDataElement(new DataElement(messageHeaderSegment, dialogId));
    messageHeaderSegment->addDataElement(new DataElement(messageHeaderSegment, QString::number(messageNumber)));
    return messageHeaderSegment;
}

void FinTsDialog::parseSegmentMessageHeader(Segment *segmentMessageHeader)
{
    QList<DataElement *> dataElements = segmentMessageHeader->getDataElements();
    if (dataElements.size() >= 3) {
        this->myDialogId = dataElements.at(2)->getValue();
        qDebug() << "[FinTsDialog] Bank told us to use dialog ID" << this->myDialogId;
    }
}

// See Formals, page 24
void FinTsDialog::parseSegmentMessageFeedback(Segment *segmentMessageFeedback)
{
    QListIterator<DataElement *> feedbackElementIterator(segmentMessageFeedback->getDataElements());
    while (feedbackElementIterator.hasNext()) {
        DataElement *feedbackElement = feedbackElementIterator.next();
        if (feedbackElement->getType() == FinTsElement::DEG) {
            DataElementGroup *feedbackGroup = qobject_cast<DataElementGroup *>(feedbackElement);
            QList<DataElement *> feedbackElements = feedbackGroup->getDataElements();
            if (feedbackElements.size() >= 3) {
                qDebug() << "[FinTsDialog] Feedback for message: " << feedbackElements.at(0)->getValue() << feedbackElements.at(2)->getValue();
            }
        }
    }
}

// See Formals, page 24
void FinTsDialog::parseSegmentSegmentFeedback(Segment *segmentSegmentFeedback)
{
    QString referenceSegmentNumber = "0";
    QList<DataElement *> segmentHeaderElements = segmentSegmentFeedback->getHeader()->getDataElements();
    if (segmentHeaderElements.size() >= 4) {
        referenceSegmentNumber = segmentHeaderElements.at(3)->getValue();
    }
    QListIterator<DataElement *> feedbackElementIterator(segmentSegmentFeedback->getDataElements());
    while (feedbackElementIterator.hasNext()) {
        DataElement *feedbackElement = feedbackElementIterator.next();
        if (feedbackElement->getType() == FinTsElement::DEG) {
            DataElementGroup *feedbackGroup = qobject_cast<DataElementGroup *>(feedbackElement);
            QList<DataElement *> feedbackElements = feedbackGroup->getDataElements();
            if (feedbackElements.size() >= 3) {
                qDebug() << "[FinTsDialog] Feedback for segment " << referenceSegmentNumber << ":" << feedbackElements.at(0)->getValue() << feedbackElements.at(2)->getValue();
            }
        }
    }
}

// See Formals, page 79
void FinTsDialog::parseSegmentBankParameter(Segment *segmentBankParameter)
{
    QList<DataElement *> bankParameterElements = segmentBankParameter->getDataElements();
    if (bankParameterElements.size() >= 6) {
        QString bpdVersion = bankParameterElements.at(0)->getValue();
        this->bankParameterData.insert(BPD_KEY_VERSION, bpdVersion);
        qDebug() << "[FinTsDialog] Bank Parameter Data (BPD) Version: " << bpdVersion;
        QList<DataElement *> kikElements = qobject_cast<DataElementGroup *>(bankParameterElements.at(1))->getDataElements();
        QString countryCode = kikElements.at(0)->getValue();
        this->bankParameterData.insert(BPD_KEY_COUNTRY_CODE, countryCode);
        qDebug() << "[FinTsDialog] Bank country code: " << countryCode;
        QString bankCode = kikElements.at(1)->getValue();
        this->bankParameterData.insert(BPD_KEY_BANK_CODE, bankCode);
        qDebug() << "[FinTsDialog] Bank code: " << bankCode;
        QString bankName = bankParameterElements.at(2)->getValue();
        this->bankParameterData.insert(BPD_KEY_BANK_NAME, bankName);
        qDebug() << "[FinTsDialog] Bank name: " << bankName;
        QString maxTransactions = bankParameterElements.at(3)->getValue();
        this->bankParameterData.insert(BPD_KEY_MAX_TRANSACTIONS, maxTransactions);
        qDebug() << "[FinTsDialog] Maximum transactions per message: " << maxTransactions;
        QString supportedLanguage = bankParameterElements.at(4)->getValue();
        this->bankParameterData.insert(BPD_KEY_SUPPORTED_LANGUAGE, supportedLanguage);
        qDebug() << "[FinTsDialog] Supported Language (0: Default, 1: German, 2: English, 3: French): " << supportedLanguage;
        QString supportedHBCIVersion = bankParameterElements.at(5)->getValue();
        this->bankParameterData.insert(BPD_KEY_SUPPORTED_HBCI_VERSION, supportedHBCIVersion);
        qDebug() << "[FinTsDialog] Supported HBCI version: " << supportedHBCIVersion;
    }
}

// See Formals, page 81
void FinTsDialog::parseSegmentSecurityProcedure(Segment *segmentSecurityProcedure)
{
    QList<DataElement *> securityProcedureElements = segmentSecurityProcedure->getDataElements();
    QListIterator<DataElement *> securityProcedureIterator(securityProcedureElements);
    bool pinTanSupported = false;
    while (securityProcedureIterator.hasNext()) {
        DataElement *currentSecurityProcedureElement = securityProcedureIterator.next();
        if (currentSecurityProcedureElement->getType() == FinTsElement::DEG) {
            QList<DataElement *> currentSecurityProcedureInfo = qobject_cast<DataElementGroup *>(currentSecurityProcedureElement)->getDataElements();
            DataElement *currentSecurityProcedureName = currentSecurityProcedureInfo.at(0);
            if (currentSecurityProcedureName->getValue() == "PIN") {
                pinTanSupported = true;
            }
            qDebug() << "[FinTsDialog] Supported security procedure: " << currentSecurityProcedureName->getValue() << currentSecurityProcedureInfo.at(1)->getValue();
        }
    }
    this->bankParameterData.insert(BPD_KEY_PIN_TAN_SUPPORTED, pinTanSupported);
    qDebug() << "[FinTsDialog] PIN/TAN supported: " << pinTanSupported;
}

// See Formals, page 43
Segment *FinTsDialog::createSegmentIdentification(FinTsElement *parentElement, int segmentNumber, const QString &blz)
{
    Segment *messageIdentificationSegment = new Segment(parentElement);
    messageIdentificationSegment->setHeader(createDegSegmentHeader(messageIdentificationSegment, SEGMENT_IDENTIFICATION_ID, QString::number(segmentNumber), SEGMENT_IDENTIFICATION_VERSION));

    messageIdentificationSegment->addDataElement(createDegBankId(messageIdentificationSegment, blz));
    messageIdentificationSegment->addDataElement(new DataElement(messageIdentificationSegment, FINTS_PLACEHOLDER_CUSTOMER_ID));
    messageIdentificationSegment->addDataElement(new DataElement(messageIdentificationSegment, CUSTOMER_SYSTEM_ID));
    messageIdentificationSegment->addDataElement(new DataElement(messageIdentificationSegment, CUSTOMER_SYSTEM_STATUS));
    return messageIdentificationSegment;
}

// See Formals, page 45
Segment *FinTsDialog::createSegmentProcessPreparation(FinTsElement *parentElement, int segmentNumber)
{
    Segment *processPreparationSegment = new Segment(parentElement);
    processPreparationSegment->setHeader(createDegSegmentHeader(processPreparationSegment, SEGMENT_PROCESS_PREPARATION_ID, QString::number(segmentNumber), SEGMENT_PROCESS_PREPARATION_VERSION));

    processPreparationSegment->addDataElement(new DataElement(processPreparationSegment, this->bankParameterData.value(BPD_KEY_VERSION).toString()));
    processPreparationSegment->addDataElement(new DataElement(processPreparationSegment, this->bankParameterData.value(UPD_KEY_VERSION).toString()));
    processPreparationSegment->addDataElement(new DataElement(processPreparationSegment, this->bankParameterData.value(BPD_KEY_SUPPORTED_LANGUAGE).toString()));
    processPreparationSegment->addDataElement(new DataElement(processPreparationSegment, FINTS_PRODUCT_ID));
    processPreparationSegment->addDataElement(new DataElement(processPreparationSegment, FINTS_PRODUCT_VERSION));
    return processPreparationSegment;
}

// See Formals, page 15/16
Segment *FinTsDialog::createSegmentMessageTermination(FinTsElement *parentElement, int segmentNumber, int messageNumber)
{
    Segment *messageTerminationSegment = new Segment(parentElement);
    messageTerminationSegment->setHeader(createDegSegmentHeader(messageTerminationSegment, SEGMENT_MESSAGE_TERMINATION_ID, QString::number(segmentNumber), SEGMENT_MESSAGE_TERMINATION_VERSION));

    messageTerminationSegment->addDataElement(new DataElement(messageTerminationSegment, QString::number(messageNumber)));
    return messageTerminationSegment;
}

// See Fomals, page 54
Segment *FinTsDialog::createSegmentDialogEnd(FinTsElement *parentElement, int segmentNumber)
{
    Segment *dialogEndSegment = new Segment(parentElement);
    dialogEndSegment->setHeader(createDegSegmentHeader(dialogEndSegment, SEGMENT_DIALOG_END_ID, QString::number(segmentNumber), SEGMENT_DIALOG_END_VERSION));

    dialogEndSegment->addDataElement(new DataElement(dialogEndSegment, this->myDialogId));
    return dialogEndSegment;
}

// See Formals, page 123
DataElementGroup *FinTsDialog::createDegSegmentHeader(FinTsElement *parentElement, const QString &segmentId, const QString &segmentNumber, const QString &segmentVersion)
{
    qDebug() << "FinTsDialog::createSegmentHeader" << segmentId << segmentNumber << segmentVersion;
    DataElementGroup *segmentHeader = new DataElementGroup(parentElement);
    segmentHeader->addDataElement(new DataElement(segmentHeader, segmentId));
    segmentHeader->addDataElement(new DataElement(segmentHeader, segmentNumber));
    segmentHeader->addDataElement(new DataElement(segmentHeader, segmentVersion));
    return segmentHeader;
}

// See Geschäftsvorfälle, page 3
DataElementGroup *FinTsDialog::createDegBankId(FinTsElement *parentElement, const QString &blz)
{
    DataElementGroup *bankId = new DataElementGroup(parentElement);
    bankId->addDataElement(new DataElement(bankId, this->bankParameterData.value(BPD_KEY_COUNTRY_CODE).toString()));
    bankId->addDataElement(new DataElement(bankId, blz));
    return bankId;
}

void FinTsDialog::insertMessageLength(Message *message)
{
    QString messageLengthString = QString::number(message->getCompleteLength()).rightJustified(12, '0');
    message->getSegments().at(0)->getDataElements().at(0)->setValue(messageLengthString);
}
