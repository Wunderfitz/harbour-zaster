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

#include "fintsserializer.h"
#include <QListIterator>
#include <QDebug>
#include <QRegExp>

FinTsSerializer::FinTsSerializer(QObject *parent) : QObject(parent)
{

}

QByteArray FinTsSerializer::serializeAndEncode(Message *message)
{
    return serialize(message, false).toLatin1().toBase64();
}

QString FinTsSerializer::serializeCore(Message *message)
{
    return serialize(message, true);
}

QString FinTsSerializer::serialize(Message *message, const bool &coreOnly)
{
    QString serializedMessage;
    QListIterator<Segment *> segmentIterator(message->getSegments());
    while (segmentIterator.hasNext()) {
        Segment *nextSegment = segmentIterator.next();
        if (coreOnly) {
            QString headerId = nextSegment->getHeader()->getDataElements().at(0)->getValue();
            if (headerId == SEGMENT_MESSAGE_HEADER_ID || headerId == SEGMENT_MESSAGE_TERMINATION_ID) {
                continue;
            }
        }
        serializedMessage.append(serialize(nextSegment));
        serializedMessage.append("'");
    }
    //qDebug() << "My message: " << serializedMessage;
    return serializedMessage;
}

QString FinTsSerializer::serialize(Segment *segment)
{
    QString serializedSegment;
    serializedSegment.append(serialize(segment->getHeader()));
    serializedSegment.append("+");
    QListIterator<DataElement *> dataElementIterator(segment->getDataElements());
    while (dataElementIterator.hasNext()) {
        serializedSegment.append(serialize(dataElementIterator.next()));
        if (dataElementIterator.hasNext()) {
            serializedSegment.append("+");
        }
    }
    return serializedSegment;
}

QString FinTsSerializer::serialize(DataElement *dataElement)
{
    QString serializedDataElement;
    if (dataElement->getType() == FinTsElement::DEG) {
        DataElementGroup *dataElementGroup = qobject_cast<DataElementGroup *>(dataElement);
        QListIterator<DataElement *> dataElementIterator(dataElementGroup->getDataElements());
        while (dataElementIterator.hasNext()) {
            serializedDataElement.append(serialize(dataElementIterator.next()));
            if (dataElementIterator.hasNext()) {
                serializedDataElement.append(":");
            }
        }
    } else {
        QString dataElementValue = dataElement->getValue();
        if (!dataElement->isBinary()) {
            dataElementValue = dataElementValue.replace(QRegExp("([\\'\\?\\@\\:\\+])"), "?\\1");
        }
        serializedDataElement.append(dataElementValue);
    }
    return serializedDataElement;
}
