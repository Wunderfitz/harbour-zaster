/*
    Copyright (C) 2018-19 Sebastian J. Wolf

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
    connect(this->finTsDialog, SIGNAL(dialogInitializationCompleted()), this, SLOT(handleDialogInitializationCompleted()));
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
    emit accountSwitched();
    this->finTsDialog->initializeParameters();
}

void FinTsAccounts::removeCurrentAccount()
{
    qDebug() << "FinTsAccounts::removeCurrentAccount";
    QSettings settings("harbour-zaster", "finTsSettings");
    QString currentAccountUUID = settings.value("accountUUID").toString();
    QFile::remove(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster/finTsSettings.conf");
    QListIterator<QVariant> accountsIterator(this->myAccounts);
    while (accountsIterator.hasNext()) {
        QVariantMap currentAccount = accountsIterator.next().toMap();
        QString otherAccountUUID = currentAccount.value("uuid").toString();
        if (otherAccountUUID != currentAccountUUID) {
            QFile::rename(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster/finTsSettings-" + otherAccountUUID + ".conf", QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster/finTsSettings.conf");
        }
    }
    emit accountSwitched();
    this->finTsDialog->initializeParameters();
    if (this->myAccounts.size() == 1) {
        this->myAccounts.clear();
    } else {
        this->initializeAccounts();
    }
}

void FinTsAccounts::switchAccount(const QString &newAccountUUID)
{
    qDebug() << "FinTsAccounts::switchAccount" << newAccountUUID;
    QSettings settings("harbour-zaster", "finTsSettings");
    QString currentAccountUUID = settings.value("accountUUID").toString();
    QFile::rename(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster/finTsSettings.conf", QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster/finTsSettings-" + currentAccountUUID + ".conf");
    QFile::rename(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster/finTsSettings-" + newAccountUUID + ".conf", QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster/finTsSettings.conf");
    emit accountSwitched();
    this->finTsDialog->initializeParameters();
}

void FinTsAccounts::handleDialogInitializationCompleted()
{
    initializeAccounts();
}

void FinTsAccounts::initializeAccounts()
{
    qDebug() << "FinTsAccounts::initializeAccounts";
    this->myAccounts.clear();
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster";
    QDir configDirectory(configPath);
    QVariantMap currentAccount;
    QSettings currentSettings("harbour-zaster", "finTsSettings");
    currentAccount.insert("uuid", currentSettings.value("accountUUID").toString());
    currentAccount.insert("descriptorId", currentSettings.value("accountDescriptorID").toString());
    currentAccount.insert("descriptorText", currentSettings.value("accountDescriptorText").toString());
    this->myAccounts.append(currentAccount);

    QStringList nameFilter("finTsSettings-*.conf");
    QStringList accountFiles = configDirectory.entryList(nameFilter);
    QStringListIterator accountFilesIterator(accountFiles);
    while (accountFilesIterator.hasNext()) {
        QVariantMap otherAccount;
        QString accountFileName = accountFilesIterator.next();
        QRegExp accountFileMatcher("(finTsSettings\\-[0-9a-f\\-]*)\\.conf");
        if (accountFileMatcher.indexIn(accountFileName) != -1) {
            QSettings otherSettings("harbour-zaster", accountFileMatcher.cap(1));
            qDebug() << "Found account: " << otherSettings.value("accountUUID").toString() << otherSettings.value("accountDescriptorID").toString() << otherSettings.value("accountDescriptorText").toString();
            otherAccount.insert("uuid", otherSettings.value("accountUUID").toString());
            otherAccount.insert("descriptorId", otherSettings.value("accountDescriptorID").toString());
            otherAccount.insert("descriptorText", otherSettings.value("accountDescriptorText").toString());
            this->myAccounts.append(otherAccount);
        }
    }
}
