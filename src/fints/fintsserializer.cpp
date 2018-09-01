#include "fintsserializer.h"
#include <QListIterator>
#include <QDebug>

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
        serializedDataElement.append(dataElement->getValue());
    }
    return serializedDataElement;
}
