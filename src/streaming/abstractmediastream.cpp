/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Nokia Corporation
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "abstractmediastream.h"
#include "abstractmediastream_p.h"
#include "streaminterface_p.h"

namespace Phonon
{

AbstractMediaStream::AbstractMediaStream(QObject *parent)
    : QObject(parent),
    d_ptr(new AbstractMediaStreamPrivate)
{
    d_ptr->q_ptr = this;
}

AbstractMediaStream::AbstractMediaStream(AbstractMediaStreamPrivate &dd, QObject *parent)
    : QObject(parent),
    d_ptr(&dd)
{
    d_ptr->q_ptr = this;
}

AbstractMediaStream::~AbstractMediaStream()
{
}

qint64 AbstractMediaStream::streamSize() const
{
    return d_ptr->streamSize;
}

void AbstractMediaStream::setStreamSize(qint64 newSize)
{
    d_ptr->setStreamSize(newSize);
}

void AbstractMediaStreamPrivate::setStreamSize(qint64 newSize)
{
    streamSize = newSize;
    if (streamInterface) {
        streamInterface->setStreamSize(newSize);
    }
}

bool AbstractMediaStream::streamSeekable() const
{
    return d_ptr->streamSeekable;
}

void AbstractMediaStream::setStreamSeekable(bool s)
{
    d_ptr->setStreamSeekable(s);
}

void AbstractMediaStreamPrivate::setStreamSeekable(bool s)
{
    streamSeekable = s;
    if (streamInterface) {
        streamInterface->setStreamSeekable(s);
    }
}

void AbstractMediaStream::writeData(const QByteArray &data)
{
    d_ptr->writeData(data);
}

void AbstractMediaStreamPrivate::writeData(const QByteArray &data)
{
    if (ignoreWrites) {
        return;
    }
    Q_ASSERT(streamInterface);
    streamInterface->writeData(data);
}

void AbstractMediaStream::endOfData()
{
    d_ptr->endOfData();
}

void AbstractMediaStreamPrivate::endOfData()
{
    if (streamInterface) {
        streamInterface->endOfData();
    }
}

void AbstractMediaStream::error(Phonon::ErrorType type, const QString &text)
{
    Q_D(AbstractMediaStream);
    d->errorType = type;
    d->errorText = text;
}

void AbstractMediaStream::enoughData()
{
}

void AbstractMediaStream::seekStream(qint64)
{
    Q_ASSERT(!d_ptr->streamSeekable);
}

AbstractMediaStreamPrivate::~AbstractMediaStreamPrivate()
{
    if (streamInterface) {
        // TODO: StreamInterface might be in a different thread
        streamInterface->d->disconnectMediaStream();
    }
}

void AbstractMediaStreamPrivate::setStreamInterface(StreamInterface *iface)
{
    P_Q(AbstractMediaStream);
    streamInterface = iface;
    if (!iface) {
        // our subclass might be just about to call writeData, so tell it we have enoughData and
        // ignore the next writeData calls
        q->enoughData();
        ignoreWrites = true;
        return;
    }
    if (ignoreWrites) {
        ignoreWrites = false;
        // we had a StreamInterface before. The new StreamInterface expects us to start reading from
        // position 0
        q->reset();
    } else {
        iface->setStreamSize(streamSize);
        iface->setStreamSeekable(streamSeekable);
    }
}

} // namespace Phonon


#include "moc_abstractmediastream.cpp"

// vim: sw=4 sts=4 et tw=100