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

#ifndef FINTSDIALOG_H
#define FINTSDIALOG_H

#include <QObject>
#include <QDebug>
#include <QVariantMap>
#include "dataelementgroup.h"
#include "dataelement.h"
#include "message.h"
#include "segment.h"
#include "fintsglobals.h"
#include "bpdconstants.h"
#include "updconstants.h"

// HBCI-Version - always fixed version 3.0, see Formals, page 15
const char FINTS_VERSION[] = "300";

// Customer system ID must be 0 for PIN/TAN, see Formals page 116
const char CUSTOMER_SYSTEM_ID[] = "0";
// Customer system status, needs to be "1", see Formals page 117
const char CUSTOMER_SYSTEM_STATUS[] = "1";

// Placeholder message length, that will be changed when we completed the whole message
const char MESSAGE_LENGTH_PLACEHOLDER[] = "000000000000";
const char MESSAGE_HEADER_ID[] = "HNHBK";
const char MESSAGE_HEADER_VERSION[] = "3";
const char MESSAGE_IDENTIFICATION_ID[] = "HKIDN";
const char MESSAGE_IDENTIFICATION_VERSION[] = "2";
const char MESSAGE_PROCESS_PREPARATION_ID[] = "HKVVB";
const char MESSAGE_PROCESS_PREPARATION_VERSION[] = "3";
const char MESSAGE_TERMINATION_ID[] = "HNHBS";
const char MESSAGE_TERMINATION_VERSION[] = "1";

class FinTsDialog : public QObject
{
    Q_OBJECT
public:
    explicit FinTsDialog(QObject *parent = 0);
    Message *createDialogInitializationMessage();

signals:

public slots:

private:

    Segment *createMessageHeaderSegment(FinTsElement *parentElement, int segmentNumber, QString dialogId, int messageNumber);
    Segment *createIdentificationSegment(FinTsElement *parentElement, int segmentNumber, const QString &blz);
    Segment *createProcessPreparationSegment(FinTsElement *parentElement, int segmentNumber);
    Segment *createMessageTerminationSegment(FinTsElement *parentElement, int segmentNumber, int messageNumber);

    DataElementGroup *createSegmentHeader(FinTsElement *parentElement, const QString &segmentId, const QString &segmentNumber, const QString &segmentVersion);
    DataElementGroup *createBankId(FinTsElement *parentElement, const QString &blz);

    void insertMessageLength(Message *message);

    QString myDialogId;
    QString myDialogLanguage;
    int myMessageNumber;
    QVariantMap bankParameterData;
    QVariantMap userParameterData;
};

#endif // FINTSDIALOG_H