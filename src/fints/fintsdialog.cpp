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
#include <QDateTime>
#include <QTimeZone>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QLocale>

FinTsDialog::FinTsDialog(QObject *parent, QNetworkAccessManager *networkAccessManager) : QObject(parent)
{
    this->networkAccessManager = networkAccessManager;

    this->initializeParameters();

    connect(&institutesSearchWorker, SIGNAL(searchCompleted(QString, QVariantList)), this, SLOT(handleInstitutesSearchCompleted(QString, QVariantList)));
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("/usr/share/harbour-zaster/db/fints_institutes.db");

    if (!database.open()) {
       qDebug() << "Error: connection with institutes database failed";
    } else {
       qDebug() << "Institutes database: Connection OK";
    }

    QSettings finTsSettings("harbour-zaster", "finTsSettings");
    QVariant storedBankParameterData = finTsSettings.value("bankParameterData");
    if (storedBankParameterData.isValid()) {
        this->bankParameterData = storedBankParameterData.toMap();
    }

    QVariant storedUserParameterData = finTsSettings.value("userParameterData");
    if (storedUserParameterData.isValid()) {
        this->userParameterData = storedUserParameterData.toMap();
        this->anonymousDialog = false;
    }

}

void FinTsDialog::dialogInitialization()
{
    qDebug() << "FinTsDialog::dialogInitialization";
    Message *dialogInitializationMessage = this->createMessageDialogInitialization();
    QByteArray serializedInitializationMessage = serializer.serializeAndEncode(dialogInitializationMessage);

    QNetworkReply *reply = sendMessage(serializedInitializationMessage);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleDialogInitializationError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleDialogInitializationFinished()));

    dialogInitializationMessage->deleteLater();
}

void FinTsDialog::synchronization()
{
    qDebug() << "FinTsDialog::synchronization";
    Message *synchronizationMessage = this->createMessageSynchronization();
    QByteArray serializedSynchronizationMessage = serializer.serializeAndEncode(synchronizationMessage);

    QNetworkReply *reply = sendMessage(serializedSynchronizationMessage);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleSynchronizationError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleSynchronizationFinished()));

    synchronizationMessage->deleteLater();
}

void FinTsDialog::closeDialog()
{
    qDebug() << "FinTsDialog::closeDialog";
    Message *closeDialogMessage = this->createMessageCloseDialog();
    QByteArray serializedCloseDialogMessage = serializer.serializeAndEncode(closeDialogMessage);

    QNetworkReply *reply = sendMessage(serializedCloseDialogMessage);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleDialogEndError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleDialogEndFinished()));

}

void FinTsDialog::accountBalance()
{
    qDebug() << "FinTsDialog::accountBalance";
    Message *accountBalanceMessage = this->createMessageAccountBalance();
    QByteArray serializedAccountBalanceMessage = serializer.serializeAndEncode(accountBalanceMessage);

    QNetworkReply *reply = sendMessage(serializedAccountBalanceMessage);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleAccountBalanceError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleAccountBalanceFinished()));
}

bool FinTsDialog::supportsPinTan()
{
    return this->bankParameterData.value(BPD_KEY_PIN_TAN_SUPPORTED, false).toBool();
}

QString FinTsDialog::getBankId()
{
    return this->bankParameterData.value(BPD_KEY_BANK_ID).toString();
}

QString FinTsDialog::getBankName()
{
    return this->bankParameterData.value(BPD_KEY_BANK_NAME).toString();
}

void FinTsDialog::setBankData(const QString &bankId, const QString &bankName, const QString &url)
{
    this->bankParameterData.insert(BPD_KEY_BANK_ID, bankId);
    this->bankParameterData.insert(BPD_KEY_BANK_NAME, bankName);
    this->bankParameterData.insert(BPD_KEY_FINTS_URL, url);
}

void FinTsDialog::setUserData(const QString &userId, const QString &pin)
{
    this->userParameterData.insert(UPD_KEY_USER_ID, userId);
    this->myPin = pin;
}

void FinTsDialog::setPin(const QString &pin)
{
    this->myPin = pin;
}

void FinTsDialog::searchInstitute(const QString &queryString)
{
    qDebug() << "FinTsDialog::searchInstitute" << queryString;
    while (this->institutesSearchWorker.isRunning()) {
        this->institutesSearchWorker.requestInterruption();
    }
    this->institutesSearchWorker.setParameters(database, queryString);
    this->institutesSearchWorker.start();
}

void FinTsDialog::storeParameterData()
{
    qDebug() << "FinTsDialog::storeParameterData";
    QSettings finTsSettings("harbour-zaster", "finTsSettings");
    finTsSettings.setValue("bankParameterData", this->bankParameterData);
    finTsSettings.setValue("userParameterData", this->userParameterData);
}

bool FinTsDialog::isPinSet()
{
    return !this->myPin.isEmpty();
}

bool FinTsDialog::isInitialized()
{
    return !this->anonymousDialog;
}

void FinTsDialog::handleDialogInitializationError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "FinTsDialog::handleDialogInitializationError:" << (int)error << reply->errorString() << reply->readAll();
    emit dialogInitializationFailed();
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
    if (this->anonymousDialog) {
        emit dialogInitializationCompleted(true);
    } else {
        emit dialogInitializationCompleted(false);
    }
    replyMessage->deleteLater();
}

void FinTsDialog::handleSynchronizationError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "FinTsDialog::handleSynchronizationError:" << (int)error << reply->errorString() << reply->readAll();
    emit synchronizationFailed();
}

void FinTsDialog::handleSynchronizationFinished()
{
    qDebug() << "FinTsDialog::handleSynchronizationFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    Message *replyMessage = deserializer.decodeAndDeserialize(reply->readAll());
    parseReplyDialogInitialization(replyMessage);
    if (this->anonymousDialog) {
        emit synchronizationCompleted();
    } else {
        emit synchronizationCompleted();
    }
    replyMessage->deleteLater();
}

void FinTsDialog::handleDialogEndError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "FinTsDialog::handleDialogEndError:" << (int)error << reply->errorString() << reply->readAll();
    emit dialogEndFailed();
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
    if (this->anonymousDialog) {
        this->anonymousDialog = false;
        emit dialogEndCompleted(true);
    } else {
        emit dialogEndCompleted(false);
    }
    replyMessage->deleteLater();
}

void FinTsDialog::handleAccountBalanceError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "FinTsDialog::handleAccountBalanceError:" << (int)error << reply->errorString() << reply->readAll();
    emit accountBalanceFailed();
}

void FinTsDialog::handleAccountBalanceFinished()
{
    qDebug() << "FinTsDialog::handleAccountBalanceFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    Message *replyMessage = deserializer.decodeAndDeserialize(reply->readAll());
    emit accountBalanceCompleted(parseReplyAccountBalance(replyMessage));
    replyMessage->deleteLater();
}

void FinTsDialog::handleInstitutesSearchCompleted(const QString &queryString, const QVariantList &resultList)
{
    qDebug() << "FinTsDialog::handleInstitutesSearchCompleted" << queryString;
    emit institutesSearchCompleted(resultList);
}

QNetworkReply *FinTsDialog::sendMessage(const QByteArray &serializedMessage)
{
    QUrl url = QUrl(this->bankParameterData.value(BPD_KEY_FINTS_URL).toString());
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    return networkAccessManager->post(request, serializedMessage);
}

Message *FinTsDialog::createMessageDialogInitialization()
{
    qDebug() << "FinTsDialog::createMessageDialogInitialization" << this->anonymousDialog;
    this->myMessageNumber++;
    Message *dialogInitializationMessage = new Message();
    dialogInitializationMessage->addSegment(createSegmentMessageHeader(dialogInitializationMessage, dialogInitializationMessage->getNextSegmentNumber(), this->myDialogId, this->myMessageNumber));
    if (!this->anonymousDialog) {
        dialogInitializationMessage->addSegment(createSegmentSignatureHeader(dialogInitializationMessage, dialogInitializationMessage->getNextSegmentNumber()));
    }
    // Usually it's the German "Bankleitzahl" or BLZ, see Geschäftsvorfälle page 608
    dialogInitializationMessage->addSegment(createSegmentIdentification(dialogInitializationMessage, dialogInitializationMessage->getNextSegmentNumber(), this->bankParameterData.value(BPD_KEY_BANK_ID).toString()));
    dialogInitializationMessage->addSegment(createSegmentProcessPreparation(dialogInitializationMessage, dialogInitializationMessage->getNextSegmentNumber()));
    if (!this->anonymousDialog) {
        dialogInitializationMessage->addSegment(createSegmentSignatureFooter(dialogInitializationMessage, dialogInitializationMessage->getNextSegmentNumber()));
    }
    dialogInitializationMessage->addSegment(createSegmentMessageTermination(dialogInitializationMessage, dialogInitializationMessage->getNextSegmentNumber(), this->myMessageNumber));

    if (this->anonymousDialog) {
        insertMessageLength(dialogInitializationMessage);
        return dialogInitializationMessage;
    } else {
        return packageMessage(dialogInitializationMessage);
    }
}

Message *FinTsDialog::createMessageSynchronization()
{
    qDebug() << "FinTsDialog::createMessageSynchronization";
    this->myMessageNumber++;
    Message *dialogInitializationMessage = new Message();
    dialogInitializationMessage->addSegment(createSegmentMessageHeader(dialogInitializationMessage, dialogInitializationMessage->getNextSegmentNumber(), this->myDialogId, this->myMessageNumber));
    dialogInitializationMessage->addSegment(createSegmentSignatureHeader(dialogInitializationMessage, dialogInitializationMessage->getNextSegmentNumber()));

    // Usually it's the German "Bankleitzahl" or BLZ, see Geschäftsvorfälle page 608
    dialogInitializationMessage->addSegment(createSegmentIdentification(dialogInitializationMessage, dialogInitializationMessage->getNextSegmentNumber(), this->bankParameterData.value(BPD_KEY_BANK_ID).toString()));
    dialogInitializationMessage->addSegment(createSegmentProcessPreparation(dialogInitializationMessage, dialogInitializationMessage->getNextSegmentNumber()));

    // TODO: Probably don't get customer ID if we already have one :D
    dialogInitializationMessage->addSegment(createSegmentSynchronization(dialogInitializationMessage, dialogInitializationMessage->getNextSegmentNumber()));
    dialogInitializationMessage->addSegment(createSegmentSignatureFooter(dialogInitializationMessage, dialogInitializationMessage->getNextSegmentNumber()));
    dialogInitializationMessage->addSegment(createSegmentMessageTermination(dialogInitializationMessage, dialogInitializationMessage->getNextSegmentNumber(), this->myMessageNumber));

    return packageMessage(dialogInitializationMessage);
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
        if (segmentIdentifier == SEGMENT_PIN_TAN_INFORMATION_ID) { parseSegmentPinTanInformation(currentSegment); }
        if (segmentIdentifier == SEGMENT_USER_PARAMETER_DATA_ID) { parseSegmentUserParameterData(currentSegment); }
        if (segmentIdentifier == SEGMENT_SYNCHRONIZATION_RESPONSE_ID) { parseSegmentSynchronizationResponse(currentSegment); }
        if (segmentIdentifier == SEGMENT_ACCOUNT_INFORMATION_ID) { parseSegmentAccountInformation(currentSegment); }
        if (segmentIdentifier == SEGMENT_ENCRYPTED_DATA_ID) { parseReplyDialogInitialization(parseSegmentEncryptedMessage(currentSegment)); }
    }
}

Message *FinTsDialog::createMessageCloseDialog()
{
    qDebug() << "FinTsDialog::createMessageCloseDialog";
    Message *closeDialogMessage = new Message();
    this->myMessageNumber++;
    closeDialogMessage->addSegment(createSegmentMessageHeader(closeDialogMessage, closeDialogMessage->getNextSegmentNumber(), this->myDialogId, this->myMessageNumber));
    if (!this->anonymousDialog) {
        closeDialogMessage->addSegment(createSegmentSignatureHeader(closeDialogMessage, closeDialogMessage->getNextSegmentNumber()));
    }
    closeDialogMessage->addSegment(createSegmentDialogEnd(closeDialogMessage, closeDialogMessage->getNextSegmentNumber()));
    if (!this->anonymousDialog) {
        closeDialogMessage->addSegment(createSegmentSignatureFooter(closeDialogMessage, closeDialogMessage->getNextSegmentNumber()));
    }
    closeDialogMessage->addSegment(createSegmentMessageTermination(closeDialogMessage, closeDialogMessage->getNextSegmentNumber(), this->myMessageNumber));

    if (this->anonymousDialog) {
        insertMessageLength(closeDialogMessage);
        return closeDialogMessage;
    } else {
        return packageMessage(closeDialogMessage);
    }
}

void FinTsDialog::parseReplyCloseDialog(Message *replyMessage)
{
    qDebug() << "FinTsDialog::parseReplyCloseDialog";
    QListIterator<Segment *> segmentIterator(replyMessage->getSegments());
    while (segmentIterator.hasNext()) {
        Segment *currentSegment = segmentIterator.next();
        QString segmentIdentifier = currentSegment->getIdentifier();
        if (segmentIdentifier == SEGMENT_MESSAGE_FEEDBACK_ID) { parseSegmentMessageFeedback(currentSegment); }
        if (segmentIdentifier == SEGMENT_ENCRYPTED_DATA_ID) { parseReplyCloseDialog(parseSegmentEncryptedMessage(currentSegment)); }
    }
    this->myDialogId = "0";
    this->myMessageNumber = 0;
}

Message *FinTsDialog::createMessageAccountBalance()
{
    qDebug() << "FinTsDialog::createMessageAccountBalance";
    Message *accountBalanceMessage = new Message();
    this->myMessageNumber++;
    accountBalanceMessage->addSegment(createSegmentMessageHeader(accountBalanceMessage, accountBalanceMessage->getNextSegmentNumber(), this->myDialogId, this->myMessageNumber));
    accountBalanceMessage->addSegment(createSegmentSignatureHeader(accountBalanceMessage, accountBalanceMessage->getNextSegmentNumber()));
    accountBalanceMessage->addSegment(createSegmentAccountBalance(accountBalanceMessage, accountBalanceMessage->getNextSegmentNumber()));
    accountBalanceMessage->addSegment(createSegmentSignatureFooter(accountBalanceMessage, accountBalanceMessage->getNextSegmentNumber()));
    accountBalanceMessage->addSegment(createSegmentMessageTermination(accountBalanceMessage, accountBalanceMessage->getNextSegmentNumber(), this->myMessageNumber));
    return packageMessage(accountBalanceMessage);
}

QVariantList FinTsDialog::parseReplyAccountBalance(Message *replyMessage)
{
    qDebug() << "FinTsDialog::parseReplyAccountBalance";
    QVariantList accountBalances;
    QListIterator<Segment *> segmentIterator(replyMessage->getSegments());
    while (segmentIterator.hasNext()) {
        Segment *currentSegment = segmentIterator.next();
        QString segmentIdentifier = currentSegment->getIdentifier();
        if (segmentIdentifier == SEGMENT_MESSAGE_HEADER_ID) { parseSegmentMessageHeader(currentSegment); }
        if (segmentIdentifier == SEGMENT_MESSAGE_FEEDBACK_ID) { parseSegmentMessageFeedback(currentSegment); }
        if (segmentIdentifier == SEGMENT_SEGMENT_FEEDBACK_ID) { parseSegmentSegmentFeedback(currentSegment); }
        if (segmentIdentifier == SEGMENT_ACCOUNT_BALANCE_RESPONSE_ID) { accountBalances.append(parseSegmentAccountBalance(currentSegment)); }
        if (segmentIdentifier == SEGMENT_ENCRYPTED_DATA_ID) { accountBalances.append(parseReplyAccountBalance(parseSegmentEncryptedMessage(currentSegment))); }
    }
    return accountBalances;
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
                QString messageFeedbackCode = feedbackElements.at(0)->getValue();
                qDebug() << "[FinTsDialog] Feedback for message: " << messageFeedbackCode << feedbackElements.at(2)->getValue();
                if (messageFeedbackCode.startsWith("9")) {
                    emit errorOccurred();
                }
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
        QString bankId = kikElements.at(1)->getValue();
        this->bankParameterData.insert(BPD_KEY_BANK_ID, bankId);
        qDebug() << "[FinTsDialog] Bank ID: " << bankId;
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
    qDebug() << "[FinTsDialog] PIN/TAN explicitly supported: " << pinTanSupported;
}

void FinTsDialog::parseSegmentPinTanInformation(Segment *segmentPinTanInformation)
{
    QList<DataElement *> segmentPinTanInformationElements = segmentPinTanInformation->getDataElements();
    if (segmentPinTanInformationElements.size() > 0) {
        this->bankParameterData.insert(BPD_KEY_PIN_TAN_SUPPORTED, true);
        qDebug() << "[FinTsDialog] PIN/TAN implicitly supported by PIN/TAN information segment";
    }
}

// See Formals, page 87
void FinTsDialog::parseSegmentUserParameterData(Segment *segmentUserParameterData)
{
    QList<DataElement *> userParameterElements = segmentUserParameterData->getDataElements();
    if (userParameterElements.size() >= 2) {
        QString userId = userParameterElements.at(0)->getValue();
        this->userParameterData.insert(UPD_KEY_USER_ID, userId);
        qDebug() << "[FinTsDialog] User ID: " << userId;
        QString updVersion = userParameterElements.at(1)->getValue();
        this->userParameterData.insert(UPD_KEY_VERSION, updVersion);
        qDebug() << "[FinTsDialog] User Parameter Data (UPD) version: " << updVersion;
    }
}

void FinTsDialog::parseSegmentSynchronizationResponse(Segment *segmentSynchronizationResponse)
{
    QList<DataElement *> synchronizationResponseElements = segmentSynchronizationResponse->getDataElements();
    if (synchronizationResponseElements.size() > 0) {
        QString newCustomerSystemId = synchronizationResponseElements.at(0)->getValue();
        this->userParameterData.insert(UPD_KEY_CUSTOMER_SYSTEM_ID, newCustomerSystemId.replace("+", "?+"));
        qDebug() << "[FinTsDialog] New Customer System ID: " << newCustomerSystemId;
    }
}

// See Formals, page 88
void FinTsDialog::parseSegmentAccountInformation(Segment *segmentAccountInformation)
{
    QVariantMap currentAccount;
    QVariantList allowedTransactions;

    QList<DataElement *> accountInformationElements = segmentAccountInformation->getDataElements();
    if (accountInformationElements.size() >= 6) {
        qDebug() << "[FinTsDialog] === Found account! ===";
        QList<DataElement *> ktvElements = qobject_cast<DataElementGroup *>(accountInformationElements.at(0))->getDataElements();
        currentAccount.insert(UPD_KEY_ACCOUNT_ID, ktvElements.at(0)->getValue());
        qDebug() << "[FinTsDialog] Account ID: " << ktvElements.at(0)->getValue();
        currentAccount.insert(BPD_KEY_BANK_ID, ktvElements.at(3)->getValue());
        qDebug() << "[FinTsDialog] Bank ID: " << ktvElements.at(3)->getValue();
        currentAccount.insert(UPD_KEY_IBAN, accountInformationElements.at(1)->getValue());
        qDebug() << "[FinTsDialog] IBAN: " << accountInformationElements.at(1)->getValue();
        currentAccount.insert(UPD_KEY_USER_ID, accountInformationElements.at(2)->getValue());
        qDebug() << "[FinTsDialog] User ID: " << accountInformationElements.at(2)->getValue();
        // See Formals, page 114
        currentAccount.insert(UPD_KEY_ACCOUNT_KIND, accountInformationElements.at(3)->getValue());
        qDebug() << "[FinTsDialog] Account Kind: " << accountInformationElements.at(3)->getValue();
        currentAccount.insert(UPD_KEY_ACCOUNT_CURRENCY, accountInformationElements.at(4)->getValue());
        qDebug() << "[FinTsDialog] Account Currency: " << accountInformationElements.at(4)->getValue();
        currentAccount.insert(UPD_KEY_ACCOUNT_OWNER_1, accountInformationElements.at(5)->getValue());
        qDebug() << "[FinTsDialog] Account Owner 1: " << accountInformationElements.at(5)->getValue();
    }
    if (accountInformationElements.size() >= 7) {
        currentAccount.insert(UPD_KEY_ACCOUNT_OWNER_2, accountInformationElements.at(6)->getValue());
        qDebug() << "[FinTsDialog] Account Owner 2: " << accountInformationElements.at(6)->getValue();
    }
    if (accountInformationElements.size() >= 8) {
        currentAccount.insert(UPD_KEY_ACCOUNT_DESCRIPTION, accountInformationElements.at(7)->getValue());
        qDebug() << "[FinTsDialog] Account Description: " << accountInformationElements.at(7)->getValue();
    }
    if (accountInformationElements.size() >= 9) {
        currentAccount.insert(UPD_KEY_ACCOUNT_LIMIT, accountInformationElements.at(8)->getValue());
        qDebug() << "[FinTsDialog] Account Limit: " << accountInformationElements.at(8)->getValue();
    }
    for (int i = 10; i <= 1008; i++) {
        if (accountInformationElements.size() >= i) {
            DataElement *rawTransactionElement = accountInformationElements.at(i - 1);
            if (rawTransactionElement->getType() == FinTsElement::DEG) {
                QList<DataElement *> allowedTransactionElements = qobject_cast<DataElementGroup *>(rawTransactionElement)->getDataElements();
                allowedTransactions.append(allowedTransactionElements.at(0)->getValue());
                qDebug() << "[FinTsDialog] Allowed Transaction: " << allowedTransactionElements.at(0)->getValue();
            }
        } else {
            break;
        }
    }
    if (accountInformationElements.size() >= 1009) {
        QJsonDocument accountExtensionJson = QJsonDocument::fromJson( accountInformationElements.at(1008)->getValue().toLatin1() );
        currentAccount.insert(UPD_KEY_ACCOUNT_EXTENSION, accountExtensionJson.toVariant());
        qDebug() << "[FinTsDialog] Account Extension found";
        QJsonObject accountExtensionObject = accountExtensionJson.object();
        if (accountExtensionObject.contains("umsltzt")) {
            QDateTime changedAtTimestamp = QDateTime::fromString(accountExtensionObject.value("umsltzt").toString(), "yyyy-MM-dd-hh.mm.ss.000zzz");
            qDebug() << "[FinTsDialog] Last changed at: " << changedAtTimestamp.toString();
            currentAccount.insert(UPD_KEY_ACCOUNT_CHANGED_AT, changedAtTimestamp);
        }
    }
    QVariantList accounts = this->userParameterData.value(UPD_KEY_ACCOUNTS, QVariantList()).toList();
    accounts.append(currentAccount);
    this->userParameterData.insert(UPD_KEY_ACCOUNTS, accounts);
}

// See Geschäftsvorfälle, page 48
QVariantMap FinTsDialog::parseSegmentAccountBalance(Segment *segmentAccountBalance)
{
    QVariantMap accountBalance;
    QList<DataElement *> accountBalanceElements = segmentAccountBalance->getDataElements();
    if (accountBalanceElements.size() >= 4) {
        qDebug() << "[FinTsDialog] === New Account Balance! ===";
        QList<DataElement *> ktiElements = qobject_cast<DataElementGroup *>(accountBalanceElements.at(0))->getDataElements();
        accountBalance.insert(TRANSACTION_KEY_ACCOUNT_ID, ktiElements.at(2)->getValue());
        qDebug() << "[FinTsDialog] Account ID: " << ktiElements.at(2)->getValue();
        accountBalance.insert(TRANSACTION_KEY_ACCOUNT_DESCRIPTION, accountBalanceElements.at(1)->getValue());
        qDebug() << "[FinTsDialog] Account Description: " << accountBalanceElements.at(1)->getValue();
        QList<DataElement *> valueElements = qobject_cast<DataElementGroup *>(accountBalanceElements.at(3))->getDataElements();
        accountBalance.insert(TRANSACTION_KEY_CREDIT_DEBIT, valueElements.at(0)->getValue());
        qDebug() << "[FinTsDialog] Credit/Debit: " << valueElements.at(0)->getValue();
        QLocale germanLocale("de");
        float floatValue = germanLocale.toFloat(valueElements.at(1)->getValue());
        accountBalance.insert(TRANSACTION_KEY_VALUE, floatValue);
        qDebug() << "[FinTsDialog] Value: " << floatValue;
        accountBalance.insert(TRANSACTION_KEY_CURRENCY, valueElements.at(2)->getValue());
        qDebug() << "[FinTsDialog] Currency: " << valueElements.at(2)->getValue();
    }
    return accountBalance;
}

Message *FinTsDialog::parseSegmentEncryptedMessage(Segment *segmentEncryptedMessage)
{
    QList<DataElement *> encryptedMessageElements = segmentEncryptedMessage->getDataElements();
    if (encryptedMessageElements.size() >= 1) {
        return deserializer.deserialize(encryptedMessageElements.at(0)->getValue().toLatin1());
    } else {
        qDebug() << "[FinTsDialog] ERROR: Unable to decrypt message!";
        return 0;
    }
}

// See Formals, page 43
Segment *FinTsDialog::createSegmentIdentification(FinTsElement *parentElement, int segmentNumber, const QString &blz)
{
    Segment *messageIdentificationSegment = new Segment(parentElement);
    messageIdentificationSegment->setHeader(createDegSegmentHeader(messageIdentificationSegment, SEGMENT_IDENTIFICATION_ID, QString::number(segmentNumber), SEGMENT_IDENTIFICATION_VERSION));

    messageIdentificationSegment->addDataElement(createDegBankId(messageIdentificationSegment, blz));
    messageIdentificationSegment->addDataElement(new DataElement(messageIdentificationSegment, this->userParameterData.value(UPD_KEY_USER_ID).toString()));
    messageIdentificationSegment->addDataElement(new DataElement(messageIdentificationSegment, this->userParameterData.value(UPD_KEY_CUSTOMER_SYSTEM_ID).toString()));
    if (this->anonymousDialog) {
        this->userParameterData.insert(UPD_KEY_CUSTOMER_SYSTEM_STATUS, "0");
    } else {
        this->userParameterData.insert(UPD_KEY_CUSTOMER_SYSTEM_STATUS, "1");
    }
    messageIdentificationSegment->addDataElement(new DataElement(messageIdentificationSegment, this->userParameterData.value(UPD_KEY_CUSTOMER_SYSTEM_STATUS).toString()));
    return messageIdentificationSegment;
}

// See Formals, page 45
Segment *FinTsDialog::createSegmentProcessPreparation(FinTsElement *parentElement, int segmentNumber)
{
    Segment *processPreparationSegment = new Segment(parentElement);
    processPreparationSegment->setHeader(createDegSegmentHeader(processPreparationSegment, SEGMENT_PROCESS_PREPARATION_ID, QString::number(segmentNumber), SEGMENT_PROCESS_PREPARATION_VERSION));

    processPreparationSegment->addDataElement(new DataElement(processPreparationSegment, this->bankParameterData.value(BPD_KEY_VERSION).toString()));
    processPreparationSegment->addDataElement(new DataElement(processPreparationSegment, this->userParameterData.value(UPD_KEY_VERSION).toString()));
    processPreparationSegment->addDataElement(new DataElement(processPreparationSegment, this->bankParameterData.value(BPD_KEY_SUPPORTED_LANGUAGE).toString()));
    processPreparationSegment->addDataElement(new DataElement(processPreparationSegment, FINTS_PRODUCT_ID));
    processPreparationSegment->addDataElement(new DataElement(processPreparationSegment, FINTS_PRODUCT_VERSION));
    return processPreparationSegment;
}

Segment *FinTsDialog::createSegmentSynchronization(FinTsElement *parentElement, int segmentNumber)
{
    Segment *synchronizationSegment = new Segment(parentElement);
    synchronizationSegment->setHeader(createDegSegmentHeader(synchronizationSegment, SEGMENT_SYNCHRONIZATION_ID, QString::number(segmentNumber), SEGMENT_SYNCHRONIZATION_VERSION));

    synchronizationSegment->addDataElement(new DataElement(synchronizationSegment, "0"));
    return synchronizationSegment;
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

// See HBCI, page 49
Segment *FinTsDialog::createSegmentSignatureHeader(FinTsElement *parentElement, int segmentNumber)
{
    Segment *signatureHeaderSegment = new Segment(parentElement);
    signatureHeaderSegment->setHeader(createDegSegmentHeader(signatureHeaderSegment, SEGMENT_SIGNATURE_HEADER_ID, QString::number(segmentNumber), SEGMENT_SIGNATURE_HEADER_VERSION));

    signatureHeaderSegment->addDataElement(createDegSecurityProfile(signatureHeaderSegment));
    // See PIN/TAN, page 58
    signatureHeaderSegment->addDataElement(new DataElement(signatureHeaderSegment, "999"));
    signatureHeaderSegment->addDataElement(new DataElement(signatureHeaderSegment, SIGNATURE_CONTROL_REFERENCE));
    // See HBCI, page 50
    signatureHeaderSegment->addDataElement(new DataElement(signatureHeaderSegment, "1"));
    // See HBCI, page 50
    signatureHeaderSegment->addDataElement(new DataElement(signatureHeaderSegment, "1"));
    signatureHeaderSegment->addDataElement(createDegSecurityIdentificationDetails(signatureHeaderSegment));
    // Security Reference Number, if I interpret the docs correctly it's simply the user ID here, could also be 0
    //signatureHeaderSegment->addDataElement(new DataElement(signatureHeaderSegment, this->userParameterData.value(UPD_KEY_USER_ID).toString()));
    signatureHeaderSegment->addDataElement(new DataElement(signatureHeaderSegment, "1"));
    signatureHeaderSegment->addDataElement(createDegDateTime(signatureHeaderSegment));
    signatureHeaderSegment->addDataElement(createDegHashAlgorithm(signatureHeaderSegment));
    signatureHeaderSegment->addDataElement(createDegSignatureAlgorithm(signatureHeaderSegment));
    signatureHeaderSegment->addDataElement(createDegKeyName(signatureHeaderSegment, "S"));
    return signatureHeaderSegment;
}

// See HBCI, page 52
Segment *FinTsDialog::createSegmentSignatureFooter(FinTsElement *parentElement, int segmentNumber)
{
    Segment *signatureFooterSegment = new Segment(parentElement);
    signatureFooterSegment->setHeader(createDegSegmentHeader(signatureFooterSegment, SEGMENT_SIGNATURE_FOOTER_ID, QString::number(segmentNumber), SEGMENT_SIGNATURE_FOOTER_VERSION));
    signatureFooterSegment->addDataElement(new DataElement(signatureFooterSegment, SIGNATURE_CONTROL_REFERENCE));
    signatureFooterSegment->addDataElement(new DataElement(signatureFooterSegment, ""));
    // See PIN/TAN, page 59
    signatureFooterSegment->addDataElement(new DataElement(signatureFooterSegment, this->myPin));
    return signatureFooterSegment;
}

// See HBCI, page 53
Segment *FinTsDialog::createSegmentEncryptionHeader(FinTsElement *parentElement, int segmentNumber)
{
    Segment *encryptionHeaderSegment = new Segment(parentElement);
    encryptionHeaderSegment->setHeader(createDegSegmentHeader(encryptionHeaderSegment, SEGMENT_ENCRYPTION_HEADER_ID, QString::number(segmentNumber), SEGMENT_ENCRYPTION_HEADER_VERSION));
    encryptionHeaderSegment->addDataElement(createDegSecurityProfile(encryptionHeaderSegment));
    //See PIN/TAN, page 59
    encryptionHeaderSegment->addDataElement(new DataElement(encryptionHeaderSegment, "998"));
    encryptionHeaderSegment->addDataElement(new DataElement(encryptionHeaderSegment, "1"));
    encryptionHeaderSegment->addDataElement(createDegSecurityIdentificationDetails(encryptionHeaderSegment));
    encryptionHeaderSegment->addDataElement(createDegDateTime(encryptionHeaderSegment));
    encryptionHeaderSegment->addDataElement(createDegEncryptionAlgorithm(encryptionHeaderSegment));
    encryptionHeaderSegment->addDataElement(createDegKeyName(encryptionHeaderSegment, "V"));
    encryptionHeaderSegment->addDataElement(new DataElement(encryptionHeaderSegment, "0"));
    return encryptionHeaderSegment;
}

Segment *FinTsDialog::createSegmentEncryptedData(FinTsElement *parentElement, int segmentNumber, const QString &encryptedData)
{
    Segment *encryptionDataSegment = new Segment(parentElement);
    encryptionDataSegment->setHeader(createDegSegmentHeader(encryptionDataSegment, SEGMENT_ENCRYPTED_DATA_ID, QString::number(segmentNumber), SEGMENT_ENCRYPTED_DATA_VERSION));
    encryptionDataSegment->addDataElement(new DataElement(encryptionDataSegment, encryptedData));
    return encryptionDataSegment;
}

Segment *FinTsDialog::createSegmentAccountBalance(FinTsElement *parentElement, int segmentNumber)
{
    Segment *accountBalanceSegment = new Segment(parentElement);
    accountBalanceSegment->setHeader(createDegSegmentHeader(accountBalanceSegment, SEGMENT_ACCOUNT_BALANCE_ID, QString::number(segmentNumber), SEGMENT_ACCOUNT_BALANCE_VERSION));
    QVariantMap firstAccount = this->userParameterData.value(UPD_KEY_ACCOUNTS).toList().at(0).toMap();
    accountBalanceSegment->addDataElement(createDegAccountId(accountBalanceSegment, this->bankParameterData.value(BPD_KEY_BANK_ID).toString(), firstAccount.value(UPD_KEY_ACCOUNT_ID).toString()));
    accountBalanceSegment->addDataElement(new DataElement(accountBalanceSegment, "J"));
    return accountBalanceSegment;
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

// See PIN/TAN, page 58 and HBCI, page 174
DataElementGroup *FinTsDialog::createDegSecurityProfile(FinTsElement *parentElement)
{
    DataElementGroup *securityProfile = new DataElementGroup(parentElement);
    securityProfile->addDataElement(new DataElement(securityProfile, "PIN"));
    // One-step for now, as we don't do transactions yet
    securityProfile->addDataElement(new DataElement(securityProfile, "1"));
    return securityProfile;
}

// See HBCI, page 173
DataElementGroup *FinTsDialog::createDegSecurityIdentificationDetails(FinTsElement *parentElement)
{
    DataElementGroup *securityDetails = new DataElementGroup(parentElement);
    // We are the message sender
    securityDetails->addDataElement(new DataElement(securityDetails, "1"));
    securityDetails->addDataElement(new DataElement(securityDetails, ""));
    securityDetails->addDataElement(new DataElement(securityDetails, this->userParameterData.value(UPD_KEY_CUSTOMER_SYSTEM_ID).toString()));
    return securityDetails;
}

DataElementGroup *FinTsDialog::createDegDateTime(FinTsElement *parentElement)
{
    DataElementGroup *dateTime = new DataElementGroup(parentElement);
    // Security Timestamp, see HBCI page 163
    dateTime->addDataElement(new DataElement(dateTime, "1"));
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QTimeZone timezone("Europe/Berlin");
    currentDateTime.setTimeZone(timezone);
    dateTime->addDataElement(new DataElement(dateTime, currentDateTime.toString("yyyyMMdd:hhmmss")));
    return dateTime;
}

// See HBCI, page 163
DataElementGroup *FinTsDialog::createDegHashAlgorithm(FinTsElement *parentElement)
{
    DataElementGroup *hashAlgorithm = new DataElementGroup(parentElement);
    hashAlgorithm->addDataElement(new DataElement(hashAlgorithm, "1"));
    hashAlgorithm->addDataElement(new DataElement(hashAlgorithm, "999"));
    hashAlgorithm->addDataElement(new DataElement(hashAlgorithm, "1"));
    return hashAlgorithm;
}

// See HBCI, page 175 & PIN/TAN, page 58
DataElementGroup *FinTsDialog::createDegSignatureAlgorithm(FinTsElement *parentElement)
{
    DataElementGroup *signatureAlgorithm = new DataElementGroup(parentElement);
    signatureAlgorithm->addDataElement(new DataElement(signatureAlgorithm, "6"));
    signatureAlgorithm->addDataElement(new DataElement(signatureAlgorithm, "10"));
    signatureAlgorithm->addDataElement(new DataElement(signatureAlgorithm, "16"));
    return signatureAlgorithm;
}

// See HBCI, page 170
DataElementGroup *FinTsDialog::createDegKeyName(FinTsElement *parentElement, const QString &keyType)
{
    DataElementGroup *keyName = new DataElementGroup(parentElement);
    keyName->addDataElement(createDegBankId(keyName, this->bankParameterData.value(BPD_KEY_BANK_ID).toString()));
    keyName->addDataElement(new DataElement(keyName, this->userParameterData.value(UPD_KEY_USER_ID).toString()));
    keyName->addDataElement(new DataElement(keyName, keyType));
    keyName->addDataElement(new DataElement(keyName, "0"));
    keyName->addDataElement(new DataElement(keyName, "0"));
    return keyName;
}

// See HBCI, page 177
DataElementGroup *FinTsDialog::createDegEncryptionAlgorithm(FinTsElement *parentElement)
{
    DataElementGroup *encryptionAlgorithm = new DataElementGroup(parentElement);
    encryptionAlgorithm->addDataElement(new DataElement(encryptionAlgorithm, "2"));
    encryptionAlgorithm->addDataElement(new DataElement(encryptionAlgorithm, "2"));
    encryptionAlgorithm->addDataElement(new DataElement(encryptionAlgorithm, "13"));
    encryptionAlgorithm->addDataElement(new DataElement(encryptionAlgorithm, "@1@X"));
    encryptionAlgorithm->addDataElement(new DataElement(encryptionAlgorithm, "5"));
    encryptionAlgorithm->addDataElement(new DataElement(encryptionAlgorithm, "1"));
    return encryptionAlgorithm;
}

// See Geschäftsvorfälle, page 4
DataElementGroup *FinTsDialog::createDegAccountId(FinTsElement *parentElement, const QString &blz, const QString &accountNumber)
{
    DataElementGroup *accountId = new DataElementGroup(parentElement);
    // IBAN (empty for national accounts)
    accountId->addDataElement(new DataElement(accountId, ""));
    // BIC (empty for national accounts)
    accountId->addDataElement(new DataElement(accountId, ""));
    accountId->addDataElement(new DataElement(accountId, accountNumber));
    accountId->addDataElement(new DataElement(accountId, ""));
    accountId->addDataElement(createDegBankId(accountId, blz));
    return accountId;
}

void FinTsDialog::insertMessageLength(Message *message)
{
    QString messageLengthString = QString::number(message->getCompleteLength()).rightJustified(12, '0');
    message->getSegments().at(0)->getDataElements().at(0)->setValue(messageLengthString);
}

QString FinTsDialog::convertToBinaryFormat(const QString &originalString)
{
    QByteArray binaryString = originalString.toLatin1();
    QString formattedBinaryString = "@";
    formattedBinaryString.append(QString::number(binaryString.length()));
    formattedBinaryString.append("@");
    formattedBinaryString.append(binaryString);
    return formattedBinaryString;
}

Message *FinTsDialog::packageMessage(Message *originalMessage)
{
    Message *packagedMessage = new Message();
    QList<Segment *> originalSegments = originalMessage->getSegments();
    Segment *headerSegment = originalSegments.first();
    headerSegment->setParent(packagedMessage);
    packagedMessage->addSegment(headerSegment);
    packagedMessage->addSegment(createSegmentEncryptionHeader(packagedMessage, 998));
    packagedMessage->addSegment(createSegmentEncryptedData(packagedMessage, 999, convertToBinaryFormat(serializer.serializeCore(originalMessage))));
    Segment *terminationSegment = originalSegments.last();
    terminationSegment->setParent(packagedMessage);
    packagedMessage->addSegment(terminationSegment);
    originalMessage->deleteLater();
    insertMessageLength(packagedMessage);
    return packagedMessage;
}

void FinTsDialog::setAnonymousDialog(const bool &isAnonymous)
{
    this->anonymousDialog = isAnonymous;
}

void FinTsDialog::initializeParameters()
{
    // Dialog-ID - first message is always "0", see Formals, page 109, TODO: use the received ID from bank later!
    this->myDialogId = "0";
    // Message number - first message is always "1", but is increased at message creation, see Formals, page 120 // TODO: increment later!
    this->myMessageNumber = 0;
    // We always start with an anonymous dialog to check if PIN/TAN is supported
    this->anonymousDialog = true;

    // Dialog language, needs to be "0" for Standard, see Formals page 109
    this->bankParameterData.insert(BPD_KEY_SUPPORTED_LANGUAGE, "0");
    // Bank Parameter Data (BPD) version, needs to be "0" for the initial call, see Formals page 45, TODO: make it adopting to received BPD
    this->bankParameterData.insert(BPD_KEY_VERSION, "0");
    // Country code according to  ISO 3166-1, 280 is fixed for Germany (instead of 276), see Geschäftsvorfälle page 613
    this->bankParameterData.insert(BPD_KEY_COUNTRY_CODE, "280");

    // Customer system ID must be 0 for PIN/TAN, see Formals page 116
    this->userParameterData.insert(UPD_KEY_CUSTOMER_SYSTEM_ID, "0");
    // Customer system status, is "0" for anonymous dialog, see Formals page 55
    // Needs to be "1" for a standard dialog, see Formals page 117
    this->userParameterData.insert(UPD_KEY_CUSTOMER_SYSTEM_STATUS, "0");

    this->userParameterData.insert(UPD_KEY_USER_ID, FINTS_DUMMY_USER_ID);

    // User Parameter Data (UPD) version, TODO: make it adopting to received UPD
    this->userParameterData.insert(UPD_KEY_VERSION, "0");
}
