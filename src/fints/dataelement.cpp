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

#include "dataelement.h"
#include <QDebug>

DataElement::DataElement(FinTsElement *parent, const QString &myValue) : FinTsElement(parent)
{
    this->type = FinTsElement::DE;
    this->value = myValue;
    this->binary = false;
}

DataElement::~DataElement()
{
    // Children are deleted automatically by Qt's parent/child handling
}

void DataElement::setValue(const QString &newValue)
{
    this->value = newValue;
    emit valueChanged(this->value);
}

QString DataElement::getValue()
{
    return this->value;
}

bool DataElement::isBinary()
{
    return this->binary;
}

void DataElement::setBinary(const bool &binary)
{
    this->binary = binary;
}

int DataElement::getCompleteLength()
{
    return this->value.size();
}
