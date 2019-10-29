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

#ifndef FINTSACCOUNTS_H
#define FINTSACCOUNTS_H

#include <QObject>
#include <QVariantList>
#include "fintsdialog.h"

class FinTsAccounts : public QObject
{
    Q_OBJECT
public:
    explicit FinTsAccounts(QObject *parent = 0, FinTsDialog *finTsDialog = 0);

    Q_INVOKABLE QVariantList getAllAccounts();
    Q_INVOKABLE void registerNewAccount();
    Q_INVOKABLE void removeCurrentAccount();
    Q_INVOKABLE void switchAccount(const QString &newAccountUUID);
    Q_INVOKABLE int getNumberOfAccounts();

signals:
    void accountSwitched();

public slots:

private slots:
    void handleDialogInitializationCompleted();

private:
    QVariantList myAccounts;
    FinTsDialog *finTsDialog;

    void initializeAccounts();
};

#endif // FINTSACCOUNTS_H
