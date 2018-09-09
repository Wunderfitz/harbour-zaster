#include "fintsdeserializer.h"

FinTsDeserializer::FinTsDeserializer(QObject *parent) : QObject(parent)
{

}

Message *FinTsDeserializer::decodeAndDeserialize(const QByteArray &encodedMessage)
{
    return deserialize(QString::fromLatin1(QByteArray::fromBase64(encodedMessage)));
}

Message *FinTsDeserializer::deserialize(const QString &decodedMessage)
{
    Message *newMessage = new Message();
    qDebug() << "[FinTsDeserializer] Raw Message: " << decodedMessage;
    bool inEscape = false;
    bool inGroup = false;
    bool inBinaryLength = false;
    QString rawBinaryLength;
    int binaryLength = 0;
    QList<DataElement *> currentGroupDataElements;
    QList<DataElement *> currentDataElements;
    QList<Segment *> currentSegments;
    QString currentValue;
    DataElementGroup *segmentHeader = new DataElementGroup(newMessage);
    for (int i = 0; i < decodedMessage.size(); i++) {
        QChar currentCharacter = decodedMessage.at(i);
        if (inEscape) {
            currentValue.append(currentCharacter);
            inEscape = false;
            continue;
        }
        if (inBinaryLength) {
            if (currentCharacter == '@') {
                inBinaryLength = false;
                binaryLength = rawBinaryLength.toInt();
            } else {
                rawBinaryLength.append(currentCharacter);
            }
            continue;
        }
        if (binaryLength > 0) {
            currentValue.append(currentCharacter);
            binaryLength--;
            continue;
        }
        switch (currentCharacter.toLatin1()) {
        case '?':
            // escape signal
            inEscape = true;
            break;
        case '\'':
            // segment end
            if (inGroup) {
                currentGroupDataElements.append(createDataElement(newMessage, currentValue));
                inGroup = false;
                currentDataElements.append(createDataElementGroup(newMessage, currentGroupDataElements));
                currentGroupDataElements.clear();
            } else {
                currentDataElements.append(createDataElement(newMessage, currentValue));
            }
            if (!segmentHeader->isEmpty()) {
                currentSegments.append(createSegment(newMessage, segmentHeader, currentDataElements));
                currentDataElements.clear();
                segmentHeader = new DataElementGroup(newMessage);
            }
            currentValue.clear();
            break;
        case '+':
            // data element (group) end
            if (inGroup) {
                currentGroupDataElements.append(createDataElement(newMessage, currentValue));
                if (segmentHeader->isEmpty()) {
                    segmentHeader->setDataElements(currentGroupDataElements);
                } else {
                    currentDataElements.append(createDataElementGroup(newMessage, currentGroupDataElements));
                }
                currentGroupDataElements.clear();
            } else {
                currentDataElements.append(createDataElement(newMessage, currentValue));
            }
            inGroup = false;
            currentValue.clear();
            break;
        case ':':
            // data element end within group
            inGroup = true;
            currentGroupDataElements.append(createDataElement(newMessage, currentValue));
            currentValue.clear();
            break;
        case '@':
            // Starting binary bullshit...
            inBinaryLength = true;
            rawBinaryLength.clear();
            binaryLength = 0;
            break;
        default:
            currentValue.append(currentCharacter);
            break;
        }
    }
    newMessage->setSegments(currentSegments);
    return newMessage;
}

void FinTsDeserializer::debugOut(Message *message)
{
    QListIterator<Segment *> segmentIterator(message->getSegments());
    while (segmentIterator.hasNext()) {
        Segment *currentSegment = segmentIterator.next();
        QListIterator<DataElement *> headerElementsIterator(currentSegment->getHeader()->getDataElements());
        qDebug() << "[Segment] " << headerElementsIterator.next()->getValue();
        while (headerElementsIterator.hasNext()) {
            qDebug() << "-" << headerElementsIterator.next()->getValue();
        }
        QListIterator<DataElement *> segmentsElementsIterator(currentSegment->getDataElements());
        while (segmentsElementsIterator.hasNext()) {
            DataElement *myDataElement = segmentsElementsIterator.next();
            if (myDataElement->getType() == FinTsElement::DEG) {
                qDebug() << "  [Data Element Group]";
                DataElementGroup *dataElementGroup = qobject_cast<DataElementGroup *>(myDataElement);
                QListIterator<DataElement *> groupElementIterator(dataElementGroup->getDataElements());
                while (groupElementIterator.hasNext()) {
                    qDebug() << "---" << groupElementIterator.next()->getValue();
                }
            } else {
                qDebug() << "--" << myDataElement->getValue();
            }
        }
    }
}

DataElement *FinTsDeserializer::createDataElement(FinTsElement *parentElement, const QString &dataElementValue)
{
    // qDebug() << "[FinTsDeserializer] Creating data element " << dataElementValue;
    DataElement *dataElement = new DataElement(parentElement);
    dataElement->setValue(dataElementValue);
    return dataElement;
}

DataElementGroup *FinTsDeserializer::createDataElementGroup(FinTsElement *parentElement, const QList<DataElement *> &dataElements)
{
    // qDebug() << "[FinTsDeserializer] Creating data element group";
    DataElementGroup *dataElementGroup = new DataElementGroup(parentElement);
    dataElementGroup->setDataElements(dataElements);
    return dataElementGroup;
}

Segment *FinTsDeserializer::createSegment(FinTsElement *parentElement, DataElementGroup *header, const QList<DataElement *> &dataElements)
{
    // qDebug() << "[FinTsDeserializer] Creating segment " << header->getDataElements().at(0)->getValue();
    Segment *segment = new Segment(parentElement);
    segment->setHeader(header);
    segment->setDataElements(dataElements);
    return segment;
}
