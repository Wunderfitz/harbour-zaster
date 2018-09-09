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

#include "message.h"
#include <QDebug>

Message::Message(FinTsElement *parent) : FinTsElement(parent)
{
    this->type = FinTsElement::MSG;
}

Message::~Message()
{
    // Children are deleted automatically by Qt's parent/child handling
}

void Message::setSegments(const QList<Segment *> &newSegments)
{
    this->segments = newSegments;
    emit segmentsChanged(this->segments);
}

QList<Segment *> Message::getSegments()
{
    return this->segments;
}

void Message::addSegment(Segment *segment)
{
    this->segments.append(segment);
    emit segmentsChanged(this->segments);
}

int Message::getCompleteLength()
{
    QListIterator<Segment *> segmentIterator(this->segments);
    int completeLength = 0;
    while (segmentIterator.hasNext()) {
        completeLength = completeLength + segmentIterator.next()->getCompleteLength();
        completeLength++;
    }
    return completeLength;
}

int Message::getNextSegmentNumber()
{
    return this->segments.size() + 1;
}
