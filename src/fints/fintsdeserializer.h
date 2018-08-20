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
    Message *decodeAndDeserialize(const QByteArray encodedMessage);
    void debugOut(Message *message);

signals:

public slots:

private:
    DataElement *createDataElement(FinTsElement *parentElement, const QString &dataElementValue);
    DataElementGroup *createDataElementGroup(FinTsElement *parentElement, const QList<DataElement *> &dataElements);
    Segment *createSegment(FinTsElement *parentElement, DataElementGroup *header, const QList<DataElement *> &dataElements);
};

#endif // FINTSDESERIALIZER_H
