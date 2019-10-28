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

#ifndef DATAELEMENT_H
#define DATAELEMENT_H

#include "fintselement.h"

class DataElement : public FinTsElement
{
    Q_OBJECT
    Q_PROPERTY(QString value READ getValue WRITE setValue NOTIFY valueChanged)
public:
    explicit DataElement(FinTsElement *parent = 0, const QString &myValue = "");
    ~DataElement();
    void setValue(const QString &newValue);
    QString getValue();
    bool isBinary();
    void setBinary(const bool &binary);
    int getCompleteLength();

signals:
    void valueChanged(const QString &newValue);

public slots:

private:
    QString value;
    bool binary;

};

#endif // DATAELEMENT_H
