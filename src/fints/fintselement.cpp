/*
    Copyright (C) 2018 Sebastian J. Wolf

    This file is part of ZasterBanker.

    ZasterBanker is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ZasterBanker is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ZasterBanker. If not, see <http://www.gnu.org/licenses/>.
*/

#include "fintselement.h"

FinTsElement::FinTsElement(QObject *parent) : QObject(parent)
{

}

FinTsElement::Type FinTsElement::getType()
{
    return this->type;
}

void FinTsElement::setType(const FinTsElement::Type &newType)
{
    this->type = newType;
    emit typeChanged(this->type);
}
