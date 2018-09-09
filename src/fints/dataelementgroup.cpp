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

#include "dataelementgroup.h"
#include <QListIterator>
#include <QDebug>

DataElementGroup::DataElementGroup(FinTsElement *parent) : DataElement(parent)
{
    this->type = FinTsElement::DEG;
}

DataElementGroup::~DataElementGroup()
{
    // Children are deleted automatically by Qt's parent/child handling
}


QList<DataElement *> DataElementGroup::getDataElements()
{
    return this->dataElements;
}

void DataElementGroup::setDataElements(const QList<DataElement *> &newDataElements)
{
    this->dataElements = newDataElements;
    emit dataElementsChanged(this->dataElements);
}

void DataElementGroup::addDataElement(DataElement *dataElement)
{
    this->dataElements.append(dataElement);
    emit dataElementsChanged(this->dataElements);
}

bool DataElementGroup::isEmpty()
{
    return this->dataElements.size() == 0;
}

int DataElementGroup::getCompleteLength()
{
    QListIterator<DataElement *> dataElementIterator(this->dataElements);
    int completeLength = 0;
    while (dataElementIterator.hasNext()) {
        completeLength = completeLength + dataElementIterator.next()->getCompleteLength();
        if (dataElementIterator.hasNext()) {
            completeLength++;
        }
    }
    return completeLength;
}


