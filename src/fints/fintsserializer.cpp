#include "fintsserializer.h"
#include <QListIterator>

FinTsSerializer::FinTsSerializer(QObject *parent) : QObject(parent)
{

}

QByteArray FinTsSerializer::serializeAndEncode(Message *message)
{
    return serialize(message).toLatin1();
    return QByteArray();
}

QString FinTsSerializer::serialize(Message *message)
{
    QString serializedMessage;
    QListIterator<Segment *> segmentIterator(message->getSegments());
    while (segmentIterator.hasNext()) {
        serializedMessage.append(serialize(segmentIterator.next()));
        serializedMessage.append("'");
    }
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
        DataElementGroup *dataElementGroup = qobject_cast<DataElementGroup *> dataElement;
        QListIterator<DataElement *> dataElementIterator(dataElementGroup->getDataElements());
        while (dataElementIterator.hasNext()) {
            serializedDataElement.append(serialize(dataElementIterator.next()));
            if (dataElementIterator.hasNext()) {
                serializedSegment.append(":");
            }
        }
    } else {
        serializedDataElement.append(dataElement->getValue());
    }
    return serializedDataElement;
}
