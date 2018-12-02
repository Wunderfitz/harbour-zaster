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

#ifndef ZASTER_H
#define ZASTER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QSettings>
#include "wagnis/wagnis.h"
#include "fints/fintsdialog.h"
#include "fints/fintsbalances.h"
#include "fints/fintsaccounts.h"

class Zaster : public QObject
{
    Q_OBJECT
public:
    explicit Zaster(QObject *parent = 0);
    ~Zaster();
    Wagnis *getWagnis();
    FinTsDialog *getFinTsDialog();
    FinTsBalances *getFinTsBalances();
    FinTsAccounts *getFinTsAccounts();

signals:

public slots:

private:
    QNetworkAccessManager *networkAccessManager;
    Wagnis *wagnis;
    QSettings settings;
    FinTsDialog *finTsDialog;
    FinTsBalances *finTsBalances;
    FinTsAccounts *finTsAccounts;

};

#endif // ZASTER_H
