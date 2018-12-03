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

#include "fintsaccounts.h"
#include <QDir>
#include <QRegExp>
#include <QDebug>
#include <QStandardPaths>
#include <QSettings>

FinTsAccounts::FinTsAccounts(QObject *parent, FinTsDialog *finTsDialog) : QObject(parent)
{
    this->finTsDialog = finTsDialog;
    initializeAccounts();
}

QVariantList FinTsAccounts::getAllAccounts()
{
    return this->myAccounts;
}

void FinTsAccounts::registerNewAccount()
{
    qDebug() << "FinTsAccounts::registerNewAccount";
    QSettings settings("harbour-zaster", "finTsSettings");
    QString currentAccountUUID = settings.value("accountUUID").toString();
    QFile::rename(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster/finTsSettings.conf", QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster/finTsSettings-" + currentAccountUUID + ".conf");
    this->finTsDialog->initializeParameters();
}

void FinTsAccounts::removeCurrentAccount()
{
    qDebug() << "FinTsAccounts::removeCurrentAccount";
    QSettings settings("harbour-zaster", "finTsSettings");
    QString currentAccountUUID = settings.value("accountUUID").toString();
    QFile::remove(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster/finTsSettings.conf");
    if (this->myAccounts.size() > 1) {
        QListIterator<QVariant> accountsIterator(this->myAccounts);
        while (accountsIterator.hasNext()) {
            QVariantMap currentAccount = accountsIterator.next().toMap();
            QString otherAccountUUID = currentAccount.value("accountUUID").toString();
            if (otherAccountUUID != currentAccountUUID) {
                QFile::rename(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster/finTsSettings-" + otherAccountUUID + ".conf", QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster/finTsSettings.conf");
            }
        }
    }
    this->finTsDialog->initializeParameters();
}

void FinTsAccounts::switchAccount(const QString &newAccountUUID)
{
    qDebug() << "FinTsAccounts::switchAccount" << newAccountUUID;
    QSettings settings("harbour-zaster", "finTsSettings");
    QString currentAccountUUID = settings.value("accountUUID").toString();
    QFile::rename(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster/finTsSettings.conf", QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster/finTsSettings-" + currentAccountUUID + ".conf");
    QFile::rename(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster/finTsSettings-" + newAccountUUID + ".conf", QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster/finTsSettings.conf");
    this->finTsDialog->initializeParameters();
}

void FinTsAccounts::initializeAccounts()
{
    qDebug() << "FinTsAccounts::readAccounts";
    this->myAccounts.clear();
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster";
    QDir configDirectory(configPath);
    QStringList nameFilter("finTsSettings*.conf");
    QStringList accountFiles = configDirectory.entryList(nameFilter);
    QStringListIterator accountFilesIterator(accountFiles);
    while (accountFilesIterator.hasNext()) {
        QVariantMap singleAccount;
        QString accountFileName = accountFilesIterator.next();
        QRegExp accountFileMatcher("(finTsSettings\\-?[\\w]*)\\.conf");
        if (accountFileMatcher.indexIn(accountFileName) != -1) {
            QSettings currentSettings("harbour-zaster", accountFileMatcher.cap(1));
            qDebug() << "Found account: " << currentSettings.value("accountUUID").toString() << currentSettings.value("accountDescriptorID").toString() << currentSettings.value("accountDescriptorText").toString();
            singleAccount.insert("uuid", currentSettings.value("accountUUID").toString());
            singleAccount.insert("descriptorId", currentSettings.value("accountDescriptorID").toString());
            singleAccount.insert("descriptorText", currentSettings.value("accountDescriptorText").toString());
            this->myAccounts.append(singleAccount);
        }
    }
}
