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

#include "fintsdeserializer.h"
#include <QListIterator>
#include <QRegExp>
#include <QDate>
#include <QLocale>

FinTsDeserializer::FinTsDeserializer(QObject *parent) : QObject(parent)
{

}

Message *FinTsDeserializer::decodeAndDeserialize(const QByteArray &encodedMessage)
{
    return deserialize(QByteArray::fromBase64(encodedMessage));
}

Message *FinTsDeserializer::deserialize(const QByteArray &decodedMessage)
{
    Message *newMessage = new Message();
    // qDebug() << "[FinTsDeserializer] Raw Message: " << QString::fromLatin1(decodedMessage);
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
        QChar currentCharacter(decodedMessage.at(i));
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
            if (currentCharacter.isNonCharacter()) {
                currentValue.append("X");
            } else {
                currentValue.append(currentCharacter);
            }
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

bool transactionSorter(const QVariant &transaction1, const QVariant &transaction2)
 {
     return transaction1.toMap().value("volume").toMap().value("date").toDate() > transaction2.toMap().value("volume").toMap().value("date").toDate();
 }

QVariantList FinTsDeserializer::deserializeSwiftTransactions(const QString &rawSwiftMessage)
{
    QVariantList messageComponentList;
    QStringList swiftComponents = rawSwiftMessage.split(QRegExp("\r\n"));
    if (swiftComponents.length() <= 1) {
        swiftComponents.clear();
        // Not in the standard, but received that. STRANGE!
        swiftComponents = rawSwiftMessage.split(QRegExp("\\@\\@"));
    }
    QListIterator<QString> swiftComponentIterator(swiftComponents);
    QRegExp swiftIdentifierRegEx("\\:([a-zA-Z0-9]+)\\:");
    bool inMultiFunction = false;
    QString multiFunctionField;
    QVariantMap currentTransaction;
    while (swiftComponentIterator.hasNext()) {
        QString nextSwiftComponent = swiftComponentIterator.next();
        if (nextSwiftComponent.indexOf(swiftIdentifierRegEx) == 0) {
            // SWIFT Identifier
            inMultiFunction = false;
            if (!multiFunctionField.isEmpty()) {
                currentTransaction.insert("details", parseSwiftMultiFunctionField(multiFunctionField));
                messageComponentList.append(currentTransaction);
                currentTransaction.clear();
            }
            multiFunctionField.clear();
            int offset = 0;
            if (swiftIdentifierRegEx.cap(1) == SWIFT_TRANSACTION_VOLUME) {
                QVariantMap volumeMap;
                // We will need to check that code latest in year 2079!
                int year = nextSwiftComponent.mid(4, 2).toInt();
                if (year > 79) {
                    year = 1900 + year;
                } else {
                    year = 2000 + year;
                }
                QDate transactionDate(year, nextSwiftComponent.mid(6, 2).toInt(), nextSwiftComponent.mid(8, 2).toInt());
                volumeMap.insert("date", transactionDate);
                offset = 10;
                while (nextSwiftComponent.at(offset).isDigit()) {
                    offset++;
                }
                volumeMap.insert("reversal", false);
                if (nextSwiftComponent.mid(offset, 1) == "R") {
                    volumeMap.insert("reversal", true);
                    offset++;
                }
                volumeMap.insert("creditDebit", nextSwiftComponent.mid(offset, 1));
                offset++;
                if (!nextSwiftComponent.at(offset).isDigit()) {
                    offset++;
                }
                QRegExp actualVolume("(\\d+,\\d+)");
                if (nextSwiftComponent.indexOf(actualVolume, offset) != -1) {
                    QLocale germanLocale("de");
                    float floatValue = germanLocale.toFloat(actualVolume.cap(1));
                    volumeMap.insert("value", floatValue);
                }
                currentTransaction.insert("volume", volumeMap);
            }
            if (swiftIdentifierRegEx.cap(1) == SWIFT_MULTI_FUNCTION) {
                inMultiFunction = true;
                multiFunctionField.append(nextSwiftComponent.mid(4));
            }
        } else {
            // Other stuff
            if (nextSwiftComponent != "-" && inMultiFunction) {
                multiFunctionField.append(nextSwiftComponent);
            }
        }
    }
    qSort(messageComponentList.begin(), messageComponentList.end(), transactionSorter);
    return messageComponentList;
}

QVariantList FinTsDeserializer::deserializeSwiftPortfolioInfo(const QString &rawSwiftMessage)
{
    QVariantList portfolioItems;
    QStringList swiftComponents = rawSwiftMessage.split(QRegExp("\r\n"));
    if (swiftComponents.length() <= 1) {
        swiftComponents.clear();
        // Not in the standard, but received that. STRANGE!
        swiftComponents = rawSwiftMessage.split(QRegExp("\\@\\@"));
    }
    QListIterator<QString> swiftComponentIterator(swiftComponents);
    QRegExp swiftIdentifierRegEx("\\:([a-zA-Z0-9]+)\\:");
    QVariantMap currentPortfolioItem;
    bool inPortfolioItemId = false;
    QString portfolioItemId;
    QLocale germanLocale("de");
    while (swiftComponentIterator.hasNext()) {
        QString nextSwiftComponent = swiftComponentIterator.next();
        if (nextSwiftComponent.indexOf(swiftIdentifierRegEx) == 0) {
            if (inPortfolioItemId) {
                currentPortfolioItem.insert("itemId", portfolioItemId);
                inPortfolioItemId = false;
            }
            if (swiftIdentifierRegEx.cap(1) == SWIFT_BLOCK_BEGIN) {
                if (nextSwiftComponent.mid(5) == SWIFT_BLOCK_IDENTIFIER) {
                    currentPortfolioItem.clear();
                }
            }
            if (swiftIdentifierRegEx.cap(1) == SWIFT_BLOCK_END) {
                if (nextSwiftComponent.mid(5) == SWIFT_BLOCK_IDENTIFIER) {
                    portfolioItems.append(currentPortfolioItem);
                }
            }
            if (swiftIdentifierRegEx.cap(1) == SWIFT_PORTFOLIO_ITEM_ID) {
                portfolioItemId = nextSwiftComponent.mid(5);
                inPortfolioItemId = true;
            }
            if (swiftIdentifierRegEx.cap(1) == SWIFT_PORTFOLIO_ITEM_PRICE) {
                int currencyOffset = nextSwiftComponent.indexOf("//ACTU/");
                if (currencyOffset > -1 && nextSwiftComponent.length() > (currencyOffset + 10)) {
                    currencyOffset += 7;
                    currentPortfolioItem.insert("priceCurrency", nextSwiftComponent.mid(currencyOffset, 3));
                    float floatPrice = germanLocale.toFloat(nextSwiftComponent.mid(currencyOffset + 3));
                    currentPortfolioItem.insert("price", floatPrice);
                }

            }
            if (swiftIdentifierRegEx.cap(1) == SWIFT_PORTFOLIO_ITEM_AMOUNT) {
                QRegExp actualAmountRegEx("(\\w{4})\\/\\/(\\w{4})\\/(\\w{4})\\/(.+)");
                if (nextSwiftComponent.indexOf(actualAmountRegEx) != -1) {
                    float floatAmount;
                    if (actualAmountRegEx.cap(4).at(0) == 'N') {
                        floatAmount = germanLocale.toFloat(actualAmountRegEx.cap(4).mid(1));
                        currentPortfolioItem.insert("amountNegative", true);
                    } else {
                        floatAmount = germanLocale.toFloat(actualAmountRegEx.cap(4));
                        currentPortfolioItem.insert("amountNegative", false);
                    }
                    currentPortfolioItem.insert("amount", floatAmount);
                }
            }
            if (swiftIdentifierRegEx.cap(1) == SWIFT_PORTFOLIO_ITEM_VALUE) {
                int valueOffset = nextSwiftComponent.indexOf("HOLD//");
                if (valueOffset > -1 && nextSwiftComponent.length() > (valueOffset + 9)) {
                    valueOffset += 6;
                    if (nextSwiftComponent.at(valueOffset) == 'N') {
                        currentPortfolioItem.insert("valueNegative", true);
                        valueOffset++;
                    } else {
                        currentPortfolioItem.insert("valueNegative", false);
                    }
                    currentPortfolioItem.insert("valueCurrency", nextSwiftComponent.mid(valueOffset, 3));
                    float floatValue = germanLocale.toFloat(nextSwiftComponent.mid(valueOffset + 3));
                    currentPortfolioItem.insert("value", floatValue);
                }
            }
        } else if (inPortfolioItemId) {
            portfolioItemId.append("<br>");
            portfolioItemId.append(nextSwiftComponent);
        }
    }
    return portfolioItems;
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

QVariantMap FinTsDeserializer::parseSwiftMultiFunctionField(const QString &multiFunctionField)
{
    QVariantMap multiFunctionMap;
    QStringList multiFunctionComponents = multiFunctionField.split("?");
    if (multiFunctionComponents.size() > 1) {
        QString transactionPurpose;
        QString otherPartyName;
        QListIterator<QString> multiFunctionComponentIterator(multiFunctionComponents);
        multiFunctionComponentIterator.next();
        // We start with the second element as the first one doesn't contain any valuable stuff
        while (multiFunctionComponentIterator.hasNext()) {
            QString multiFunctionElement = multiFunctionComponentIterator.next();
            QString elementIdentifier = multiFunctionElement.left(2);
            if (elementIdentifier == "00") {
                multiFunctionMap.insert("transactionText", multiFunctionElement.mid(2));
            }
            if (elementIdentifier.at(0) == '2' || elementIdentifier.at(0) == '6') {
                transactionPurpose.append(multiFunctionElement.mid(2));
            }
            if (elementIdentifier == "30") {
                multiFunctionMap.insert("otherPartyBankId", multiFunctionElement.mid(2));
            }
            if (elementIdentifier == "31") {
                multiFunctionMap.insert("otherPartyAccountId", multiFunctionElement.mid(2));
            }
            if (elementIdentifier == "32" || elementIdentifier == "33") {
                otherPartyName.append(multiFunctionElement.mid(2));
            }
        }
        multiFunctionMap.insert("transactionPurpose", transactionPurpose);
        multiFunctionMap.insert("otherPartyName", otherPartyName);
    }
    return multiFunctionMap;
}
