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

#ifndef DATAELEMENTGROUP_H
#define DATAELEMENTGROUP_H

#include "dataelement.h"
#include <QList>

class DataElementGroup : public DataElement
{
    Q_OBJECT
public:
    explicit DataElementGroup(FinTsElement *parent = 0);
    ~DataElementGroup();
    QList<DataElement *> getDataElements();
    void setDataElements(const QList<DataElement *> &newDataElements);
    void addDataElement(DataElement *dataElement);
    bool isEmpty();
    int getCompleteLength();

signals:
    void dataElementsChanged(const QList<DataElement *> &newDataElements);

public slots:

private:
    QList<DataElement *> dataElements;
};

#endif // DATAELEMENTGROUP_H
