/*
    Copyright (C) 2018 Sebastian J. Wolf

    This file is part of ZasterBanker.

    ZasterBanker is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ZasterBanker is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ZasterBanker. If not, see <http://www.gnu.org/licenses/>.
*/

#include "fintsbalances.h"
#include <QListIterator>

FinTsBalances::FinTsBalances(QObject *parent, FinTsDialog *finTsDialog) : QObject(parent)
{
    this->finTsDialog = finTsDialog;
    this->workInProgress = false;
    this->shallHandleDialogEnd = false;
    this->inError = false;
    connect(this->finTsDialog, SIGNAL(dialogInitializationFailed()), this, SLOT(handleDialogInitializationFailed()));
    connect(this->finTsDialog, SIGNAL(dialogInitializationCompleted()), this, SLOT(handleDialogInitializationCompleted()));
    connect(this->finTsDialog, SIGNAL(accountBalanceCompleted(QVariantList)), this, SLOT(handleAccountBalanceCompleted(QVariantList)));
    connect(this->finTsDialog, SIGNAL(accountBalanceFailed()), this, SLOT(handleAccountBalanceFailed()));
    connect(this->finTsDialog, SIGNAL(dialogEndFailed()), this, SLOT(handleDialogEndFailed()));
    connect(this->finTsDialog, SIGNAL(dialogEndCompleted()), this, SLOT(handleDialogEndCompleted()));
    connect(this->finTsDialog, SIGNAL(errorOccurred()), this, SLOT(handleErrorOccurred()));
}

void FinTsBalances::retrieveBalances()
{
    qDebug() << "FinTsBalances::retrieveBalances";
    setWorkInProgress(true);
    this->myAccounts = finTsDialog->getUserParameterData().value(UPD_KEY_ACCOUNTS).toList();
    finTsDialog->dialogInitialization();
}

void FinTsBalances::handleDialogInitializationCompleted()
{
    if (this->workInProgress) {
        if (inError) {
            this->handleErrorStatus();
            return;
        }
        this->finTsDialog->accountBalance();
    }
}

void FinTsBalances::handleDialogInitializationFailed()
{
    setWorkInProgress(false);
    emit errorRetrievingBalances("Error initializing dialog with your bank during balance retrieval!");
}

void FinTsBalances::handleAccountBalanceCompleted(const QVariantList &accountBalances)
{
    if (this->workInProgress) {
        if (inError) {
            this->handleErrorStatus();
            return;
        }
        // Did we receive all account balances or do we have others to go...
        QListIterator<QVariant> accountBalancesIterator(accountBalances);
        while (accountBalancesIterator.hasNext()) {
            QVariantMap accountBalance = accountBalancesIterator.next().toMap();
            this->retrievedAccounts.append(accountBalance.value(UPD_KEY_ACCOUNT_ID).toString());
        }
        this->retrievedBalances.append(accountBalances);
        qDebug() << "[FinTsBalances] Number of accounts: " << QString::number(this->myAccounts.size());
        QListIterator<QVariant> accountsIterator(this->myAccounts);
        while (accountsIterator.hasNext()) {
            QString myAccount = accountsIterator.next().toMap().value(UPD_KEY_ACCOUNT_ID).toString();
            qDebug() << "[FinTsBalances] Checking account " << myAccount;
            if (myAccount == this->accountInProgress) {
                // DRY! Most certainly an error/unsuppoerted format
                qDebug() << "[FinTsBalances] Previous balance retrieval not successful. Aborting...";
                break;
            }
            if (!this->retrievedAccounts.contains(myAccount)) {
                qDebug() << "[FinTsBalances] There is an account missing: " << myAccount;
                qDebug() << "[FinTsBalances] Retrieving balance for this account!";
                this->accountInProgress = myAccount;
                finTsDialog->accountBalance(myAccount);
                return;
            }
        }
        this->shallHandleDialogEnd = true;
        this->finTsDialog->closeDialog();
        emit balancesRetrieved(this->retrievedBalances);
    }
}

void FinTsBalances::handleAccountBalanceFailed()
{
    setWorkInProgress(false);
    emit errorRetrievingBalances("Error retrieving account balances!");
}

void FinTsBalances::handleDialogEndCompleted()
{
    if (this->shallHandleDialogEnd) {
        setWorkInProgress(false);
        this->shallHandleDialogEnd = false;
        this->inError = false;
        this->accountInProgress = "";
    }
}

void FinTsBalances::handleDialogEndFailed()
{
    setWorkInProgress(false);
    emit errorRetrievingBalances("Error closing dialog with your bank during balance retrieval!");
}

void FinTsBalances::handleErrorOccurred()
{
    this->inError = true;
}

void FinTsBalances::setWorkInProgress(const bool &inProgress)
{
    qDebug() << "FinTsBalances::setWorkInProgress" << inProgress;
    this->workInProgress = inProgress;
}

void FinTsBalances::handleErrorStatus()
{
    qDebug() << "[FinTsBalances] Error status detected. Closing dialog with bank.";
    this->shallHandleDialogEnd = true;
    this->finTsDialog->closeDialog();
}
