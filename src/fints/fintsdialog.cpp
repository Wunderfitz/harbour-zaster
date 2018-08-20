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

#include "fintsdialog.h"

FinTsDialog::FinTsDialog(QObject *parent) : QObject(parent)
{

}

void FinTsDialog::initialize()
{
    qDebug() << "FinTsDialog::initialize";
    Message *dialogInitializationMessage = createDialogInitializationMessage();
    // TODO: Send message and receive bank response...
}

Message *FinTsDialog::createDialogInitializationMessage()
{
    qDebug() << "FinTsDialog::createDialogInitialization";
    Message *dialogInitializationMessage = new Message();
    int currentSegmentNumber = 1;
    // Dialog-ID - first message is always "0", see Formals, page 109 // TODO: use the received ID from bank later!
    // Message number - first message is always "1", see Formals, page 120 // TODO: increment later!
    dialogInitializationMessage->addSegment(createMessageHeaderSegment(dialogInitializationMessage, currentSegmentNumber, 0, 1));
    // TODO: Dialoginitialisierung fortsetzen -> Segmentnummer inkrementieren -> Andere Segmente hinzufügen
    return dialogInitializationMessage;
}

// See Formals, page 15
Segment *FinTsDialog::createMessageHeaderSegment(FinTsElement *parentElement, int segmentNumber, int dialogId, int messageNumber)
{
    Segment *messageHeaderSegment = new Segment(parentElement);
    messageHeaderSegment->setHeader(createSegmentHeader(messageHeaderSegment, MESSAGE_HEADER_ID, QString::number(segmentNumber), MESSAGE_HEADER_VERSION));
    // TODO: Hard-coded message length, that should be changed
    messageHeaderSegment->addDataElement(new DataElement(messageHeaderSegment, "000000000125"));
    // HBCI-Version - always fixed version 3.0 -> "300", see Formals, page 15
    messageHeaderSegment->addDataElement(new DataElement(messageHeaderSegment, "300"));
    // Dialog-ID
    messageHeaderSegment->addDataElement(new DataElement(messageHeaderSegment, QString::number(dialogId)));
    // Message number
    messageHeaderSegment->addDataElement(new DataElement(messageHeaderSegment, QString::number(messageNumber)));
    return messageHeaderSegment;
}

DataElementGroup *FinTsDialog::createSegmentHeader(FinTsElement *parentElement, const QString &segmentId, const QString &segmentNumber, const QString &segmentVersion)
{
    qDebug() << "FinTsDialog::createSegmentHeader" << segmentId << segmentNumber << segmentVersion;
    DataElementGroup *segmentHeader = new DataElementGroup(parentElement);
    segmentHeader->addDataElement(new DataElement(segmentHeader, segmentId));
    segmentHeader->addDataElement(new DataElement(segmentHeader, segmentNumber));
    segmentHeader->addDataElement(new DataElement(segmentHeader, segmentVersion));
    return segmentHeader;
}
