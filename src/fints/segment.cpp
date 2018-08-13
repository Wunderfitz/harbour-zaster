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

#include "segment.h"

Segment::Segment(FinTsElement *parent) : FinTsElement(parent)
{

}

void Segment::setHeader(const FinTsElement::DataElementGroup &newHeader)
{
    this->header = newHeader;
    emit headerChanged(this->header);
}

FinTsElement::DataElementGroup Segment::getHeader()
{
    return this->header;
}

void Segment::setDataElements(const QList<FinTsElement::DataElement> &newDataElements)
{
    this->dataElements = newDataElements;
    emit dataElementsChanged(this->dataElements);
}

QList<FinTsElement::DataElement> Segment::getDataElements()
{
    return this->dataElements;
}

void Segment::addDataElement(const FinTsElement::DataElement &dataElement)
{
    this->dataElements.append(dataElement);
    emit dataElementsChanged(this->dataElements);
}
