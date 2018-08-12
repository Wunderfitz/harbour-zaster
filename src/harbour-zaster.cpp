/*
    Copyright (C) 2018 Sebastian J. Wolf

    This file is part of Zaster.

    Zaster is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Zaster is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Zaster. If not, see <http://www.gnu.org/licenses/>.
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

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> view(SailfishApp::createView());

    QQmlContext *context = view.data()->rootContext();
    Zaster zaster;
    context->setContextProperty("zaster", &zaster);
    Wagnis *wagnis = zaster.getWagnis();
    context->setContextProperty("wagnis", wagnis);

    view->setSource(SailfishApp::pathTo("qml/harbour-zaster.qml"));
    view->show();
    return app->exec();
}
