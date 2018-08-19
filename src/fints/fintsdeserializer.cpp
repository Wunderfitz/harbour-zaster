#include "fintsdeserializer.h"

FinTsDeserializer::FinTsDeserializer(QObject *parent) : QObject(parent)
{

}

Message *FinTsDeserializer::decodeAndDeserialize(const QByteArray encodedMessage)
{
    Message *newMessage = new Message();
    QString rawMessage = QString::fromLatin1(encodedMessage);
    bool inEscape = false;
    bool inGroup = false;
    QList<DataElement *> currentGroupDataElements;
    QList<DataElement *> currentDataElements;
    QList<Segment *> currentSegments;
    QString currentValue;
    DataElementGroup *segmentHeader = new DataElementGroup();
    for (int i = 0; i < rawMessage.size(); i++) {
        QChar currentCharacter = rawMessage.at(i);
        if (inEscape) {
            currentValue.append(currentCharacter);
            inEscape = false;
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
                currentGroupDataElements.append(createDataElement(currentValue));
                inGroup = false;
                currentDataElements.append(createDataElementGroup(currentGroupDataElements));
                currentGroupDataElements.clear();
            } else {
                currentDataElements.append(createDataElement(currentValue));
            }
            if (!segmentHeader->isEmpty()) {
                currentSegments.append(createSegment(segmentHeader, currentDataElements));
                currentDataElements.clear();
                segmentHeader = new DataElementGroup();
            }
            currentValue.clear();
            break;
        case '+':
            // data element (group) end
            if (inGroup) {
                currentGroupDataElements.append(createDataElement(currentValue));
                if (segmentHeader->isEmpty()) {
                    segmentHeader->setDataElements(currentGroupDataElements);
                } else {
                    currentDataElements.append(createDataElementGroup(currentGroupDataElements));
                }
                currentGroupDataElements.clear();
            } else {
                currentDataElements.append(createDataElement(currentValue));
            }
            inGroup = false;
            currentValue.clear();
            break;
        case ':':
            // data element end within group
            inGroup = true;
            currentGroupDataElements.append(createDataElement(currentValue));
            currentValue.clear();
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

DataElement *FinTsDeserializer::createDataElement(const QString &dataElementValue)
{
    qDebug() << "[FinTsDeserializer] Creating data element " << dataElementValue;
    DataElement *dataElement = new DataElement();
    dataElement->setValue(dataElementValue);
    return dataElement;
}

DataElementGroup *FinTsDeserializer::createDataElementGroup(const QList<DataElement *> &dataElements)
{
    qDebug() << "[FinTsDeserializer] Creating data element group";
    DataElementGroup *dataElementGroup = new DataElementGroup();
    dataElementGroup->setDataElements(dataElements);
    return dataElementGroup;
}

Segment *FinTsDeserializer::createSegment(DataElementGroup *header, const QList<DataElement *> &dataElements)
{
    qDebug() << "[FinTsDeserializer] Creating segment " << header->getDataElements().at(0)->getValue();
    Segment *segment = new Segment();
    segment->setHeader(header);
    segment->setDataElements(dataElements);
    return segment;
}
