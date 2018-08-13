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

#ifndef DATAELEMENTGROUP_H
#define DATAELEMENTGROUP_H

#include "dataelement.h"
#include <QList>

class DataElementGroup : public DataElement
{
    Q_OBJECT
    Q_PROPERTY(QList<DataElement> dataElements READ getDataElements WRITE setDataElements NOTIFY dataElementsChanged)
public:
    explicit DataElementGroup(DataElement *parent = 0);
    QList<DataElement> getDataElements();
    void setDataElements(const QList<DataElement> &newDataElements);
    void addDataElement(const DataElement &dataElement);

signals:
    void dataElementsChanged(const QList<DataElement> newDataElements);

public slots:

private:
    QList<DataElement> dataElements;
};

#endif // DATAELEMENTGROUP_H
