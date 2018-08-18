#ifndef FINTSSERIALIZER_H
#define FINTSSERIALIZER_H

#include <QObject>
#include "message.h"
#include "segment.h"
#include "dataelement.h"
#include "dataelementgroup.h"

class FinTsSerializer : public QObject
{
    Q_OBJECT
public:
    explicit FinTsSerializer(QObject *parent = 0);
    QByteArray serializeAndEncode(Message *message);

signals:

public slots:

private:

    QString serialize(Message *message);
    QString serialize(Segment *segment);
    QString serialize(DataElement *dataElement);

};

#endif // FINTSSERIALIZER_H
