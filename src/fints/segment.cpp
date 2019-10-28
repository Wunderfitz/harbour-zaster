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

#include "segment.h"
#include <QListIterator>
#include <QDebug>

Segment::Segment(FinTsElement *parent) : FinTsElement(parent)
{
    this->type = FinTsElement::SEG;
}

Segment::~Segment()
{
    // Children are deleted automatically by Qt's parent/child handling
}

void Segment::setHeader(DataElementGroup *newHeader)
{
    this->header = newHeader;
    emit headerChanged(this->header);
}

DataElementGroup *Segment::getHeader()
{
    return this->header;
}

void Segment::setDataElements(const QList<DataElement *> &newDataElements)
{
    this->dataElements = newDataElements;
    emit dataElementsChanged(this->dataElements);
}

QList<DataElement *> Segment::getDataElements()
{
    return this->dataElements;
}

void Segment::addDataElement(DataElement *dataElement)
{
    this->dataElements.append(dataElement);
    emit dataElementsChanged(this->dataElements);
}

void Segment::replaceDataElement(int index, DataElement *dataElement)
{
    this->dataElements.replace(index, dataElement);
    emit dataElementsChanged(this->dataElements);
}

int Segment::getCompleteLength()
{
    QListIterator<DataElement *> dataElementIterator(this->dataElements);
    int completeLength = 0;
    while (dataElementIterator.hasNext()) {
        completeLength = completeLength + dataElementIterator.next()->getCompleteLength();
        if (dataElementIterator.hasNext()) {
            completeLength++;
        }
    }
    QListIterator<DataElement *> headerIterator(this->header->getDataElements());
    while (headerIterator.hasNext()) {
        completeLength = completeLength + headerIterator.next()->getCompleteLength();
        if (headerIterator.hasNext()) {
            completeLength++;
        }
    }
    // This is the separator between the header and the rest of the segment
    completeLength++;
    return completeLength;
}

QString Segment::getIdentifier()
{
    QString myIdentifier = "//undefined//";
    if (this->header) {
        return this->header->getDataElements().at(0)->getValue();
    }
    return myIdentifier;
}
