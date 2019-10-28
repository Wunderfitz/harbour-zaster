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

#ifndef FINTSSERIALIZER_H
#define FINTSSERIALIZER_H

#include <QObject>
#include "message.h"
#include "segment.h"
#include "dataelement.h"
#include "dataelementgroup.h"
#include "messageconstants.h"

class FinTsSerializer : public QObject
{
    Q_OBJECT
public:
    explicit FinTsSerializer(QObject *parent = 0);
    QByteArray serializeAndEncode(Message *message);
    QString serializeCore(Message *message);

signals:

public slots:

private:

    QString serialize(Message *message, const bool &coreOnly);
    QString serialize(Segment *segment);
    QString serialize(DataElement *dataElement);

};

#endif // FINTSSERIALIZER_H
