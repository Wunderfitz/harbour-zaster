#include "fintsaccounts.h"
#include <QDir>
#include <QRegExp>
#include <QDebug>
#include <QStandardPaths>
#include <QSettings>

FinTsAccounts::FinTsAccounts(QObject *parent) : QObject(parent)
{
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
}

void FinTsAccounts::switchAccount(const QString &newAccountUUID)
{
    qDebug() << "FinTsAccounts::switchAccount" << newAccountUUID;
    QSettings settings("harbour-zaster", "finTsSettings");
    QString currentAccountUUID = settings.value("accountUUID").toString();
    QFile::rename(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster/finTsSettings.conf", QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster/finTsSettings-" + currentAccountUUID + ".conf");
    QFile::rename(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster/finTsSettings-" + newAccountUUID + ".conf", QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-zaster/finTsSettings.conf");
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
