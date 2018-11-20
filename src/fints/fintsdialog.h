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
#include "transactionconstants.h"
#include "institutessearchworker.h"

// Settings version (to check compatibility)
const int SETTINGS_VERSION = 1;

class FinTsDialog : public QObject
{
    Q_OBJECT
public:
    explicit FinTsDialog(QObject *parent = 0, QNetworkAccessManager *networkAccessManager = 0);

    Q_INVOKABLE void initializeParameters();
    Q_INVOKABLE void dialogInitialization();
    Q_INVOKABLE void synchronization();
    Q_INVOKABLE void closeDialog();
    Q_INVOKABLE void accountBalance(const QString &accountId = "");
    Q_INVOKABLE void accountTransactions(const QString &accountId);
    Q_INVOKABLE void portfolioInfo(const QString &portfolioId);
    Q_INVOKABLE bool supportsPinTan();
    Q_INVOKABLE QString getBankId();
    Q_INVOKABLE QString getBankName();
    Q_INVOKABLE QVariantMap getUserParameterData();
    Q_INVOKABLE void setBankData(const QString &bankId, const QString &bankName, const QString &url);
    Q_INVOKABLE void setUserData(const QString &loginId, const QString &pin);
    Q_INVOKABLE void setPin(const QString &pin);
    Q_INVOKABLE void searchInstitute(const QString &queryString);
    Q_INVOKABLE void storeParameterData();
    Q_INVOKABLE bool isPinSet();
    Q_INVOKABLE bool isInitialized();
    Q_INVOKABLE bool canRetrieveTransactions(const QString &accountId);
    Q_INVOKABLE bool canRetrievePortfolioInfo(const QString &accountId);
    Q_INVOKABLE QVariantList getErrorMessages();

signals:
    void dialogInitializationCompleted();
    void dialogInitializationFailed();
    void synchronizationCompleted();
    void synchronizationFailed();
    void dialogEndCompleted();
    void dialogEndFailed();
    void accountBalanceCompleted(const QVariantList &accountBalances);
    void accountBalanceFailed();
    void accountTransactionsCompleted(const QVariantList &accountTransactions);
    void accountTransactionsFailed();
    void institutesSearchCompleted(const QVariantList &resultList);
    void errorOccurred();
    void portfolioInfoCompleted(const QVariantList &portfolioItems);
    void portfolioInfoFailed();

public slots:

private slots:
    void handleDialogInitializationError(QNetworkReply::NetworkError error);
    void handleDialogInitializationFinished();
    void handleSynchronizationError(QNetworkReply::NetworkError error);
    void handleSynchronizationFinished();
    void handleDialogEndError(QNetworkReply::NetworkError error);
    void handleDialogEndFinished();
    void handleAccountBalanceError(QNetworkReply::NetworkError error);
    void handleAccountBalanceFinished();
    void handleAccountTransactionsError(QNetworkReply::NetworkError error);
    void handleAccountTransactionsFinished();
    void handleInstitutesSearchCompleted(const QString &queryString, const QVariantList &resultList);
    void handlePortfolioInfoError(QNetworkReply::NetworkError error);
    void handlePortfolioInfoFinished();

private:    

    QNetworkReply *sendMessage(const QByteArray &serializedMessage);

    Message *createMessageDialogInitialization();
    Message *createMessageSynchronization();
    void parseReplyDialogInitialization(Message *replyMessage);
    Message *createMessageCloseDialog();
    void parseReplyCloseDialog(Message *replyMessage);
    Message *createMessageAccountBalance(const QString &accountId);
    QVariantList parseReplyAccountBalance(Message *replyMessage);
    Message *createMessageAccountTransactions(const QString &accountId);
    QVariantList parseReplyAccountTransactions(Message *replyMessage);
    Message *createMessagePortfolioInfo(const QString &portfolioId);
    QVariantList parseReplyPortfolioInfo(Message *replyMessage);

    Segment *createSegmentMessageHeader(Message *parentMessage);
    Segment *createSegmentIdentification(Message *parentMessage);
    Segment *createSegmentProcessPreparation(Message *parentMessage);
    Segment *createSegmentSynchronization(Message *parentMessage);
    Segment *createSegmentMessageTermination(Message *parentMessage);
    Segment *createSegmentDialogEnd(Message *parentMessage);
    Segment *createSegmentSignatureHeader(Message *parentMessage);
    Segment *createSegmentSignatureFooter(Message *parentMessage);
    Segment *createSegmentEncryptionHeader(FinTsElement *parentElement);
    Segment *createSegmentEncryptedData(FinTsElement *parentElement, const QString &encryptedData);
    Segment *createSegmentAccountBalance(Message *parentMessage, const QString &accountId);
    Segment *createSegmentAccountTransactions(Message *parentMessage, const QString &accountId);
    Segment *createSegmentPortfolioInfo(Message *parentMessage, const QString &portfolioId);
    void parseSegmentMessageHeader(Segment *segmentMessageHeader);
    void parseSegmentMessageFeedback(Segment *segmentMessageFeedback);
    void parseSegmentSegmentFeedback(Segment *segmentSegmentFeedback);
    void parseSegmentBankParameter(Segment *segmentBankParameter);
    void parseSegmentSecurityProcedure(Segment *segmentSecurityProcedure);
    void parseSegmentPinTanInformation(Segment *segmentPinTanInformation);
    void parseSegmentTanTwoStepInformation(Segment *segmentTanTwoStepInformation);
    void parseSegmentUserParameterData(Segment *segmentUserParameterData);
    void parseSegmentSynchronizationResponse(Segment *segmentSynchronizationResponse);
    void parseSegmentAccountInformation(Segment *segmentAccountInformation);
    void parseSegmentAccountBalanceParameters(Segment *segmentAccountBalanceParameters);
    void parseSegmentTransactionsParameters(Segment *segmentTransactionsParameters);
    void parseSegmentPortfolioInfoParameters(Segment *segmentPortfolioInfoParameters);
    QVariantList parseSegmentAccountTransactions(Segment *segmentAccountTransactions);
    QVariantMap parseSegmentAccountBalance(Segment *segmentAccountBalance);
    QVariantList parseSegmentPortfolioInfo(Segment *segmentPortfolioInfo);
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
    DataElementGroup *createDegAccountId(FinTsElement *parentElement, const QString &blz, const QString &accountNumber, const int &messageVersion = SEGMENT_ACCOUNT_BALANCE_VERSION);
    DataElementGroup *createDegAccountIdInternational(FinTsElement *parentElement, const QString &blz, const QString &accountNumber);

    void insertMessageLength(Message *message);
    QString convertToBinaryFormat(const QString &originalString);
    Message *packageMessage(Message *originalMessage);

    void appendErrorMessage(const QString &errorCode, const QString &errorText);

    QNetworkAccessManager *networkAccessManager;
    FinTsSerializer serializer;
    FinTsDeserializer deserializer;
    QString myDialogId;
    QString myPin;
    int myMessageNumber;
    bool initialized;
    QVariantMap bankParameterData;
    QVariantMap userParameterData;
    QSqlDatabase database;
    InstitutesSearchWorker institutesSearchWorker;
    QVariantList errorMessages;
};

#endif // FINTSDIALOG_H
