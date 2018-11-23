/*
    Copyright (C) 2018 Sebastian J. Wolf

    This file is part of Zaster Banker.

    Zaster Banker is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Zaster Banker is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Zaster Banker. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WAGNIS_H
#define WAGNIS_H

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/pem.h>

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QVariantMap>
#include <QJsonDocument>

const char MIME_TYPE_JSON[] = "application/json";
const char API_REGISTRATION[] = "https://werkwolf.eu/wagnis/v1/registration.php";
const char API_CONTRIBUTION[] = "https://werkwolf.eu/wagnis/v1/contribution.php";
const char PUBLIC_KEY[] = "-----BEGIN PUBLIC KEY-----\nMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAqpEDAUc4s/FAIBEhiMXV\ndn9YZNhab8tEB1wOt57muqOFv2YMenLUJSKkph5S2eMI81o3LuFlxEcAGUZucibJ\nYuy4Xbz96nlqmRg5pwd8XBRwVGML2WuEBDGqaXzz/Xjg+KVQbBQSIPSJruKe2kkx\ninm514LnlSVpzs9P4k9/Uc9d7q6C6JBk64UO73Y2PYImG5W8s2eI38EMQt44NgiP\npfxrhFkZNpAg3vJ+U9LhXPePU3XcFiDvcWZS+XCWCYlVVqHAEp5mUTzNy41uMKnI\nXLRvMcLsFuQB8Ojv49MABvUDnfWIRYHeDvIDt50cl7wYKKk2nXEcJ9j5+LAv25X7\nEwIDAQAB\n-----END PUBLIC KEY-----";

class Wagnis : public QObject
{
    Q_OBJECT
public:
    explicit Wagnis(QNetworkAccessManager *manager, const QString &applicationName, const QString applicationVersion, QObject *parent = 0);
    ~Wagnis();
    Q_INVOKABLE QString getId();
    Q_INVOKABLE QString getCandidateRegistrationData();
    Q_INVOKABLE void registerApplication();
    Q_INVOKABLE void resetRegistration();
    Q_INVOKABLE void getApplicationRegistration();
    Q_INVOKABLE bool isRegistered();
    Q_INVOKABLE bool hasFeature(const QString &featureName);
    Q_INVOKABLE void validateContribution(const QString &contributionKey);
    Q_INVOKABLE QVariantMap getRegistrationData();
    Q_INVOKABLE bool inTestingPeriod();
    Q_INVOKABLE int getRemainingTime();

signals:
    void registrationError(const QString &errorMessage);
    void registrationValid(const QVariantMap &registrationInformation);
    void registrationInvalid();
    void contributionValidated();
    void contributionValidationError(const QString &errorMessage);

public slots:

private:

    void generateId();
    QStringList getImeis();
    QString getSerialNumber();
    QString getWifiMacAddress();
    QJsonDocument getRegistrationDocument();
    void readRegistration();
    void validateRegistrationData(const QByteArray &registrationData, const bool &saveData);
    void getIpInfo();
    QString getRegistrationFileName();
    bool isSignatureValid(const QString &content, const QString &signature);

    QString applicationName;
    QString applicationVersion;
    QString wagnisId;
    QVariantMap ipInfo;
    QVariantMap validatedRegistration;
    int remainingSeconds = 0;
    QNetworkAccessManager *manager;

private slots:
    void handleGetIpInfoError(QNetworkReply::NetworkError error);
    void handleGetIpInfoFinished();
    void handleRegisterApplicationError(QNetworkReply::NetworkError error);
    void handleRegisterApplicationFinished();
    void handleGetApplicationRegistrationError(QNetworkReply::NetworkError error);
    void handleValidateContributionFinished();
    void handleValidateContributionError(QNetworkReply::NetworkError error);
};

#endif // WAGNIS_H
