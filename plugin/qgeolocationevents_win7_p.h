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
#ifndef QGEOLOCATIONEVENTS_WIN7_H
#define QGEOLOCATIONEVENTS_WIN7_H

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

#include <windows.h>
#include <atlbase.h>
#include <atlcom.h>
#include <locationapi.h>
#include <sensors.h>

#include <QtCore>

#include "qmobilityglobal.h"
#include "qgeopositioninfo.h"

QTM_USE_NAMESPACE

class QGeoLocationEventsWin7 : public QObject, public CComObjectRoot, public ILocationEvents
{
    Q_OBJECT
public:
    explicit QGeoLocationEventsWin7(QObject* parent = 0);
    virtual ~QGeoLocationEventsWin7();

    DECLARE_NOT_AGGREGATABLE(QGeoLocationEventsWin7)

    BEGIN_COM_MAP(QGeoLocationEventsWin7)
        COM_INTERFACE_ENTRY(ILocationEvents)
    END_COM_MAP()

    // ILocationEvents

    // This is called when there is a new location report.
    STDMETHOD(OnLocationChanged)(REFIID reportType,
                                 ILocationReport* pLocationReport);

    // This is called when the status of a report type changes.
    // The LOCATION_REPORT_STATUS enumeration is defined
    // in Locationapi.h in the SDK
    STDMETHOD(OnStatusChanged)(REFIID reportType,
                               LOCATION_REPORT_STATUS status);
signals:
    void updateError();
    void positionUpdated(const QGeoPositionInfo& update);
};

#endif
