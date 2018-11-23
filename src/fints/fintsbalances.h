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

#ifndef FINTSBALANCES_H
#define FINTSBALANCES_H

#include <QObject>
#include <QVariantList>
#include "fintsdialog.h"
#include "updconstants.h"

class FinTsBalances : public QObject
{
    Q_OBJECT
public:
    explicit FinTsBalances(QObject *parent = 0, FinTsDialog *finTsDialog = 0);
    Q_INVOKABLE void retrieveBalances();

signals:
    void balancesRetrieved(const QVariantList &accountBalances);
    void errorRetrievingBalances(const QString &errorMessage);

public slots:

private slots:
    void handleDialogInitializationCompleted();
    void handleDialogInitializationFailed();
    void handleAccountBalanceCompleted(const QVariantList &accountBalances);
    void handleAccountBalanceFailed();
    void handleDialogEndCompleted();
    void handleDialogEndFailed();
    void handleErrorOccurred();

private:
    FinTsDialog *finTsDialog;
    bool workInProgress;
    bool shallHandleDialogEnd;
    bool inError;
    QString accountInProgress;
    QVariantList myAccounts;
    QVariantList retrievedAccounts;
    QVariantList retrievedBalances;

    void setWorkInProgress(const bool &inProgress);
    void handleErrorStatus();
};

#endif // FINTSBALANCES_H
