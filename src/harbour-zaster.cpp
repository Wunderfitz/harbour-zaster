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

#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <sailfishapp.h>
#include <QScopedPointer>
#include <QQuickView>
#include <QtQml>
#include <QQmlContext>
#include <QGuiApplication>

#include "wagnis/wagnis.h"
#include "zaster.h"
#include "fints/fintsdialog.h"
#include "fints/fintsbalances.h"
#include "fints/fintsaccounts.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> view(SailfishApp::createView());

    QQmlContext *context = view.data()->rootContext();
    Zaster zaster;
    context->setContextProperty("zaster", &zaster);
    Wagnis *wagnis = zaster.getWagnis();
    context->setContextProperty("wagnis", wagnis);
    FinTsDialog *finTsDialog = zaster.getFinTsDialog();
    context->setContextProperty("finTsDialog", finTsDialog);
    FinTsBalances *finTsBalances = zaster.getFinTsBalances();
    context->setContextProperty("finTsBalances", finTsBalances);
    FinTsAccounts *finTsAccounts = zaster.getFinTsAccounts();
    context->setContextProperty("finTsAccounts", finTsAccounts);

    view->setSource(SailfishApp::pathTo("qml/harbour-zaster.qml"));
    view->show();
    return app->exec();
}
