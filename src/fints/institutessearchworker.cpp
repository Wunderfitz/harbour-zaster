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

#include "institutessearchworker.h"
#include <QVariantMap>
#include <QDebug>

InstitutesSearchWorker::InstitutesSearchWorker(QObject *parent) : QThread(parent)
{

}

void InstitutesSearchWorker::setParameters(const QSqlDatabase &database, const QString &queryString)
{
    this->database = database;
    this->queryString = queryString;
}

void InstitutesSearchWorker::performSearch()
{
    QVariantList resultList;

    if (database.open()) {
        QSqlQuery query(database);
        query.prepare("select * from fints_institutes where fints_institutes match (:queryString)");
        query.bindValue(":queryString", queryString + "*");
        query.exec();
        while (query.next()) {
            if (isInterruptionRequested()) {
                break;
            }
            QVariantMap foundInstitute;
            foundInstitute.insert("blz", query.value(1).toString());
            foundInstitute.insert("name", query.value(2).toString());
            foundInstitute.insert("location", query.value(3).toString());
            foundInstitute.insert("url", query.value(4).toString());
            resultList.append(foundInstitute);
        }
    } else {
        qDebug() << "Unable to perform a query on database";
    }

    emit searchCompleted(queryString, resultList);
}
