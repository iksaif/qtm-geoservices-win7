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

#ifdef Q_LOCATION_WIN7_DEBUG
#include <QDebug>
#endif

#include <QtCore>

#include "qgeolocationevents_win7_p.h"
#include "qgeopositioninfosource_win7.h"

// Make Atl happy, else CreateInstance will crash :/
CComModule _Module;
extern __declspec(selectany) CAtlModule* _pAtlModule=&_Module;

QGeoPositionInfoSourceWin7::QGeoPositionInfoSourceWin7(QObject *parent)
    : QGeoPositionInfoSource(parent),
      m_timer(new QTimer(this)),
      m_spLocation(NULL),
      m_pLocationEvents(NULL),
      m_lastPositionFromSatellite(false)
{
    if (!initWin7LocationApi())
        exitWin7LocationApi();
}

QGeoPositionInfoSourceWin7::~QGeoPositionInfoSourceWin7()
{
    exitWin7LocationApi();
}

bool QGeoPositionInfoSourceWin7::initWin7LocationApi()
{
    HRESULT hr;

#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoPositionInfoSourceWin7::initWin7LocationApi: creating Location";
#endif

    hr = m_spLocation.CoCreateInstance(CLSID_Location);
    if (!SUCCEEDED(hr))
        return false;

#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoPositionInfoSourceWin7::initWin7LocationApi: creating LocationEvents";
#endif

    // Create the ILocation object that receives location reports.
    hr = CComObject<QGeoLocationEventsWin7>::CreateInstance(&m_pLocationEvents);
    if (!SUCCEEDED(hr))
        return false;
    m_pLocationEvents->AddRef();

#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoPositionInfoSourceWin7::initWin7LocationApi: requesting permissions";
#endif

    //IID reports_needed[] = { IID_ILatLongReport };
    //hr = m_spLocation->RequestPermissions(NULL, reports_needed, 1, TRUE);
    //if (!SUCCEEDED(hr))
    //    return false;

#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoPositionInfoSourceWin7::initWin7LocationApi: connecting signals";
#endif

    connect(m_pLocationEvents, SIGNAL(updateTimeout()), this, SIGNAL(updateTimeout()));
    connect(m_pLocationEvents, SIGNAL(positionUpdated(QGeoPositionInfo)), this, SIGNAL(positionUpdated(QGeoPositionInfo)));
    return true;
}

void QGeoPositionInfoSourceWin7::exitWin7LocationApi()
{
#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoPositionInfoSourceWin7::exitWin7LocationApi";
#endif
    if (m_pLocationEvents)
        m_pLocationEvents->Release();
    if (m_spLocation.p)
        m_spLocation.Release();
}

QGeoPositionInfo QGeoPositionInfoSourceWin7::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
    if (fromSatellitePositioningMethodsOnly) {
        if (m_lastPositionFromSatellite)
            return m_lastPosition;
        else
            return QGeoPositionInfo();
    }
    return m_lastPosition;
}

void QGeoPositionInfoSourceWin7::setPreferredPositioningMethods(PositioningMethods methods)
{
    LOCATION_DESIRED_ACCURACY desiredAccuracy;
    HRESULT hr;

    if (!m_spLocation)
        return ;

    if (methods == SatellitePositioningMethods)
        desiredAccuracy = LOCATION_DESIRED_ACCURACY_HIGH;
    else
        desiredAccuracy = LOCATION_DESIRED_ACCURACY_DEFAULT;

    hr = m_spLocation->SetDesiredAccuracy(IID_ILatLongReport, desiredAccuracy);
#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoPositionInfoSourceWin7 requested to set methods to: " << methods;
    qDebug() << "QGeoPositionInfoSourceWin7: pLocation->SetDesiredAccuracy("
             << desiredAccuracy << ") = "
             << (bool) SUCCEEDED(hr);
#endif
}

QGeoPositionInfoSourceWin7::PositioningMethods QGeoPositionInfoSourceWin7::supportedPositioningMethods() const
{
    if (!m_spLocation)
        return 0;
    return AllPositioningMethods;
}

void QGeoPositionInfoSourceWin7::setUpdateInterval(int msec)
{
    HRESULT hr;

    if (!m_spLocation) {
        emit updateTimeout();
        return ;
    }
#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoPositionInfoSourceWin7 called setUpdateInterval(" << msec << ")";
#endif

    // If msec is 0 we send updates as data becomes available, otherwise we force msec to be equal
    // to or larger than the minimum update interval.
    if (msec != 0 && msec < MinimumUpdateInterval)
        msec = MinimumUpdateInterval;
    hr = m_spLocation->SetReportInterval(IID_ILatLongReport, msec);

#ifdef Q_LOCATION_WIN7_DEBUG
    if (!SUCCEEDED(hr))
        qDebug() << "QGeoPositionInfoSourceWin7 pLocation->SetReportInterval() call failed with result:" << hr;
#endif
    if (SUCCEEDED(hr))
        QGeoPositionInfoSource::setUpdateInterval(msec);
}

int QGeoPositionInfoSourceWin7::minimumUpdateInterval() const
{
    return MinimumUpdateInterval;
}

void QGeoPositionInfoSourceWin7::startUpdates()
{
    HRESULT hr;

    if (!m_spLocation) {
        emit updateTimeout();
        return ;
    }
    hr = m_spLocation->RegisterForReport(m_pLocationEvents, IID_ILatLongReport, updateInterval());
    if (!SUCCEEDED(hr)) {
#ifdef Q_LOCATION_WIN7_DEBUG
        qDebug() << "QGeoPositionInfoSourceWin7 pLocation->RegisterForReport() call failed with result:" << hr;
#endif
        emit updateTimeout();
    }
    // TODO: use a timer for futures updateTimeout() ?
}

void QGeoPositionInfoSourceWin7::stopUpdates()
{
    HRESULT hr;

    if (!m_spLocation)
        return ;
    hr = m_spLocation->UnregisterForReport(IID_ILatLongReport);
#ifdef Q_LOCATION_WIN7_DEBUG
    if (!SUCCEEDED(hr))
        qDebug() << "QGeoPositionInfoSourceWin7 pLocation->UnregisterForReport() call failed with result:" << hr;
#endif
}

void QGeoPositionInfoSourceWin7::requestUpdate(int timeout)
{
    if (m_spLocation) {
        emit updateTimeout();
        return ;
    }
    // A timeout of 0 means to use the default timeout, which is handled by the QGeoInfoThreadWinCE
    // instance, otherwise if timeout is less than the minimum update interval we emit a
    // updateTimeout signal
    if (timeout < minimumUpdateInterval() && timeout != 0)
        emit updateTimeout();
    else {
        //
        // TODO infoThread->requestUpdate(timeout); // start timeout timer ?
    }
}
