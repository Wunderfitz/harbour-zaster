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
#include "dataelementgroup.h"
#include "dataelement.h"
#include "message.h"
#include "segment.h"
#include "fintsglobals.h"

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
    void initialize();

signals:

public slots:

private:
    Message *createDialogInitializationMessage();

    Segment *createMessageHeaderSegment(FinTsElement *parentElement, int segmentNumber, int dialogId, int messageNumber);
    Segment *createIdentificationSegment(FinTsElement *parentElement, int segmentNumber, const QString &blz);
    Segment *createProcessPreparationSegment(FinTsElement *parentElement, int segmentNumber);
    Segment *createMessageTerminationSegment(FinTsElement *parentElement, int segmentNumber, int messageNumber);

    DataElementGroup *createSegmentHeader(FinTsElement *parentElement, const QString &segmentId, const QString &segmentNumber, const QString &segmentVersion);
    DataElementGroup *createBankId(FinTsElement *parentElement, const QString &blz);

    void insertMessageLength(Message *message);
};

#endif // FINTSDIALOG_H
