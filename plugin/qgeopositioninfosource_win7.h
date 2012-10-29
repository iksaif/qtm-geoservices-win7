/****************************************************************************
**
** Copyright (C) 2012 Corentin Chary
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Corentin Chary (corentin.chary@gmail.com)
**
** This file is part of the Qt Mobility Components.
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
#ifndef QGEOPOSITIONINFOUSOURCE_WIN7_H
#define QGEOPOSITIONINFOUSOURCE_WIN7_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qgeopositioninfosource.h>
#include "qgeolocationevents_win7_p.h"

QTM_USE_NAMESPACE

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

class QGeoLocationEventsWin7;
class QGeoPositionInfoSourceWin7Private;

class QGeoPositionInfoSourceWin7 : public QGeoPositionInfoSource
{
    Q_OBJECT
public:
    enum {
        // The minimum acceptable interval for periodic updates.
        MinimumUpdateInterval = 100
    };

    explicit QGeoPositionInfoSourceWin7(QObject* parent = 0);
    ~QGeoPositionInfoSourceWin7();

    void setUpdateInterval(int msec);
    int minimumUpdateInterval() const;

    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const;

    void setPreferredPositioningMethods(PositioningMethods methods);
    PositioningMethods supportedPositioningMethods() const;

public slots:
    virtual void startUpdates();
    virtual void stopUpdates();
    virtual void requestUpdate(int timeout = 0);

private slots:
    void reportUpdateError();
    void reportPositionUpdated(const QGeoPositionInfo& position);
    void requestUpdateTimeout();
    void requestDone();

private:
    QTimer *m_requestTimer;
    bool m_updateStarted;
    QGeoPositionInfo m_lastPosition;
    QGeoPositionInfo m_lastPositionFromSatellite;
    QGeoPositionInfoSourceWin7Private *d;
};

#endif
