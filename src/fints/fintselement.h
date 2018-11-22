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

#ifndef FINTSELEMENT_H
#define FINTSELEMENT_H

#include <QObject>

class FinTsElement : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Type type READ getType WRITE setType NOTIFY typeChanged)
public:
    explicit FinTsElement(QObject *parent = 0);
    enum Type {
        DE,
        DEG,
        SEG,
        MSG
    };
    Q_ENUM(Type)

    Type getType();
    void setType(const Type &newType);
    virtual int getCompleteLength() = 0;

signals:
    void typeChanged(const Type &type);

public slots:

protected:
    Type type;

};

#endif // FINTSELEMENT_H
