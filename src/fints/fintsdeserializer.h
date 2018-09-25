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

#ifndef FINTSDESERIALIZER_H
#define FINTSDESERIALIZER_H

#include <QObject>
#include <QDebug>
#include "message.h"
#include "segment.h"
#include "dataelement.h"
#include "dataelementgroup.h"

class FinTsDeserializer : public QObject
{
    Q_OBJECT
public:
    explicit FinTsDeserializer(QObject *parent = 0);
    Message *decodeAndDeserialize(const QByteArray &encodedMessage);
    Message *deserialize(const QByteArray &decodedMessage);
    void debugOut(Message *message);

signals:

public slots:

private:
    DataElement *createDataElement(FinTsElement *parentElement, const QString &dataElementValue);
    DataElementGroup *createDataElementGroup(FinTsElement *parentElement, const QList<DataElement *> &dataElements);
    Segment *createSegment(FinTsElement *parentElement, DataElementGroup *header, const QList<DataElement *> &dataElements);
};

#endif // FINTSDESERIALIZER_H
