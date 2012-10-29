/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtCore>
#include <QTimer>
#include <QTextEdit>
#include <QGeoPositionInfoSource>

#include "clientapplication.h"

ClientApplication::ClientApplication(QWidget *parent)
    : QMainWindow(parent),
      source(NULL)
{
    textEdit = new QTextEdit;
    setCentralWidget(textEdit);
    QTimer::singleShot(0, this, SLOT(delayedInit()));
}

void ClientApplication::delayedInit()
{
    source = QGeoPositionInfoSource::createSource("win7", this);
    connect(source, SIGNAL(positionUpdated(QGeoPositionInfo)),
            this, SLOT(positionUpdated(QGeoPositionInfo)));
    connect(source, SIGNAL(updateTimeout()),
            this, SLOT(updateTimeout()));

    source->setPreferredPositioningMethods(QGeoPositionInfoSource::SatellitePositioningMethods);

    if (!source)
        return ;
    source->requestUpdate(5000);
    QTimer::singleShot(7000, this, SLOT(startUpdates()));
}

void ClientApplication::startUpdates()
{
    source->requestUpdate(1000);
    source->startUpdates();
    source->setUpdateInterval(2000);
}

void ClientApplication::updateTimeout()
{
    textEdit->append(QString("Timeout..."));
}

void ClientApplication::positionUpdated(const QGeoPositionInfo &info)
{
    textEdit->append(QString("Position updated: Date/time = %1, Coordinate = %2").arg(info.timestamp().toString()).arg(info.coordinate().toString()));
}
