#include "fintsbalances.h"
#include <QListIterator>

FinTsBalances::FinTsBalances(QObject *parent, FinTsDialog *finTsDialog) : QObject(parent)
{
    this->finTsDialog = finTsDialog;
    this->workInProgress = false;
    this->shallHandleDialogEnd = false;
    connect(this->finTsDialog, SIGNAL(dialogInitializationFailed()), this, SLOT(handleDialogInitializationFailed()));
    connect(this->finTsDialog, SIGNAL(dialogInitializationCompleted()), this, SLOT(handleDialogInitializationCompleted()));
    connect(this->finTsDialog, SIGNAL(accountBalanceCompleted(QVariantList)), this, SLOT(handleAccountBalanceCompleted(QVariantList)));
    connect(this->finTsDialog, SIGNAL(accountBalanceFailed()), this, SLOT(handleAccountBalanceFailed()));
    connect(this->finTsDialog, SIGNAL(dialogEndFailed()), this, SLOT(handleDialogEndFailed()));
    connect(this->finTsDialog, SIGNAL(dialogEndCompleted()), this, SLOT(handleDialogEndCompleted()));
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
            qDebug() << "Checking account " << myAccount;
            if (!this->retrievedAccounts.contains(myAccount)) {
                qDebug() << "[FinTsBalances] There is an account missing: " << myAccount;
                qDebug() << "[FinTsBalances] Retrieving balance for this account!";
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
    }
}

void FinTsBalances::handleDialogEndFailed()
{
    setWorkInProgress(false);
    emit errorRetrievingBalances("Error closing dialog with your bank during balance retrieval!");
}

void FinTsBalances::setWorkInProgress(const bool &inProgress)
{
    qDebug() << "FinTsBalances::setWorkInProgress" << inProgress;
    this->workInProgress = inProgress;
}
