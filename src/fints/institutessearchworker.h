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

#ifndef INSTITUTESSEARCHWORKER_H
#define INSTITUTESSEARCHWORKER_H

#include <QThread>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariantList>

class InstitutesSearchWorker : public QThread
{
    Q_OBJECT
    void run() Q_DECL_OVERRIDE {
        performSearch();
    }
public:
    explicit InstitutesSearchWorker(QObject *parent = 0);
    void setParameters(const QSqlDatabase &database, const QString &queryString);

signals:
    void searchCompleted(const QString &queryString, const QVariantList &resultList);

public slots:

private:
    QSqlDatabase database;
    QString queryString;

    void performSearch();
};

#endif // INSTITUTESSEARCHWORKER_H
