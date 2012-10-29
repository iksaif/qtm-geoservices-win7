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

/* ====== Win7 Location API ===== */
// Make Atl happy, else CreateInstance will crash :/
CComModule _Module;
extern __declspec(selectany) CAtlModule* _pAtlModule=&_Module;

class QGeoPositionInfoSourceWin7Private : public QObject
{
public:
    QGeoPositionInfoSourceWin7Private(QGeoPositionInfoSourceWin7 *parent);
    ~QGeoPositionInfoSourceWin7Private();

    bool init();
    void exit();
    bool startUpdates(int interval);
    bool stopUpdates();
    bool setUpdateInterval(int interval);
    int getUpdateInterval();
    bool setDesiredAccuracy(bool high);

private:
    CComPtr<ILocation> m_spLocation;
    CComObject<QGeoLocationEventsWin7>* m_pLocationEvents;
    QGeoPositionInfoSourceWin7 *q;

    friend class QGeoPositionInfoSourceWin7;
};

QGeoPositionInfoSourceWin7Private::QGeoPositionInfoSourceWin7Private(QGeoPositionInfoSourceWin7* parent)
    : m_spLocation(NULL),
      m_pLocationEvents(NULL),
      q(parent)
{
}

QGeoPositionInfoSourceWin7Private::~QGeoPositionInfoSourceWin7Private()
{
    exit();
}

bool QGeoPositionInfoSourceWin7Private::init()
{
    HRESULT hr;

    if (m_spLocation)
        return true;

#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoPositionInfoSourceWin7Private::init(): creating Location";
#endif

    hr = m_spLocation.CoCreateInstance(CLSID_Location);
    if (!SUCCEEDED(hr))
        goto error;

#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoPositionInfoSourceWin7Private::init(): creating LocationEvents";
#endif

    // Create the ILocation object that receives location reports.
    hr = CComObject<QGeoLocationEventsWin7>::CreateInstance(&m_pLocationEvents);
    if (!SUCCEEDED(hr))
        goto error;
    m_pLocationEvents->AddRef();

#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoPositionInfoSourceWin7Private::init(): requesting permissions";
#endif

    IID reports_needed[] = { IID_ILatLongReport };
    hr = m_spLocation->RequestPermissions(NULL, reports_needed, 1, TRUE);
    if (!SUCCEEDED(hr))
        goto error;

#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoPositionInfoSourceWin7Private::init(): connecting signals";
#endif

    // TODO: check that m_pLocationEvents really respects updateTimeout() semantics
    connect(m_pLocationEvents, SIGNAL(updateError()), q, SLOT(reportUpdateError()));
    connect(m_pLocationEvents, SIGNAL(positionUpdated(QGeoPositionInfo)), q, SLOT(reportPositionUpdated(QGeoPositionInfo)));
    return true;
error:
    exit();
    return false;
}

void QGeoPositionInfoSourceWin7Private::exit()
{
#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoPositionInfoSourceWin7Private::exit";
#endif
    if (m_pLocationEvents)
        m_pLocationEvents->Release();
    if (m_spLocation.p)
        m_spLocation.Release();
}

bool QGeoPositionInfoSourceWin7Private::startUpdates(int interval)
{
    HRESULT hr;
    int current_interval = getUpdateInterval();

#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoPositionInfoSourceWin7Private::startUpdates(" << interval << ")";
#endif

    /* already started */
    if (current_interval != -1) {
        if (current_interval == interval)
            return true;
        return setUpdateInterval(interval);
    }

    hr = m_spLocation->RegisterForReport(m_pLocationEvents, IID_ILatLongReport, interval);
#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoPositionInfoSourceWin7Private pLocation->RegisterForReport"
             << "(" << interval << ") = " << hr;
#endif
    return SUCCEEDED(hr);
}

bool QGeoPositionInfoSourceWin7Private::stopUpdates()
{
    HRESULT hr = m_spLocation->UnregisterForReport(IID_ILatLongReport);
#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoPositionInfoSourceWin7Private pLocation->UnregisterForReport() = " << hr;
#endif
    return SUCCEEDED(hr);
}

bool QGeoPositionInfoSourceWin7Private::setUpdateInterval(int interval)
{
    HRESULT hr = m_spLocation->SetReportInterval(IID_ILatLongReport, interval);
#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoPositionInfoSourceWin7Private pLocation->SetReportInterval"
             << "(" << interval << ") = " << hr;
#endif
    return SUCCEEDED(hr);
}

int QGeoPositionInfoSourceWin7Private::getUpdateInterval()
{
    DWORD msec = -1;
    HRESULT hr = m_spLocation->GetReportInterval(IID_ILatLongReport, &msec);

    return SUCCEEDED(hr) ? msec : -1;
}

bool QGeoPositionInfoSourceWin7Private::setDesiredAccuracy(bool high)
{
    LOCATION_DESIRED_ACCURACY desiredAccuracy;
    HRESULT hr;

    if (high)
        desiredAccuracy = LOCATION_DESIRED_ACCURACY_HIGH;
    else
        desiredAccuracy = LOCATION_DESIRED_ACCURACY_DEFAULT;

    hr = m_spLocation->SetDesiredAccuracy(IID_ILatLongReport, desiredAccuracy);
#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoPositionInfoSourceWin7Private: pLocation->SetDesiredAccuracy("
             << desiredAccuracy << ") = "
             << (bool) SUCCEEDED(hr);
#endif
    return SUCCEEDED(hr);
}

/* ======= Qt API ====== */
QGeoPositionInfoSourceWin7::QGeoPositionInfoSourceWin7(QObject *parent)
    : QGeoPositionInfoSource(parent),
      m_requestTimer(new QTimer(this)),
      m_updateStarted(false),
      d(new QGeoPositionInfoSourceWin7Private(this))
{
    m_requestTimer->setSingleShot(true);
    connect(m_requestTimer, SIGNAL(timeout()), this, SLOT(requestUpdateTimeout()));
}

QGeoPositionInfoSourceWin7::~QGeoPositionInfoSourceWin7()
{
    delete d;
}

void QGeoPositionInfoSourceWin7::reportUpdateError()
{
    requestDone();
    emit updateTimeout();
}

void QGeoPositionInfoSourceWin7::reportPositionUpdated(const QGeoPositionInfo& position)
{
#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoPositionInfoSourceWin7::reportPositionUpdated"
             << position;
#endif
    requestDone();

    if (!position.isValid())
        return ;

    if (preferredPositioningMethods() == SatellitePositioningMethods)
        m_lastPositionFromSatellite = position;
    else
        m_lastPosition = position;

    emit positionUpdated(position);
}

void QGeoPositionInfoSourceWin7::requestUpdateTimeout()
{
#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoPositionInfoSourceWin7: requestUpdate timeout occurred.";
#endif
    // If we end up here, there has not been valid position update.
    requestDone();
    emit updateTimeout();
}

void QGeoPositionInfoSourceWin7::requestDone()
{
    if (!m_requestTimer->isActive())
        return ;
    if (!m_updateStarted)
        d->stopUpdates();
    else if (updateInterval() != d->getUpdateInterval())
        d->setUpdateInterval(updateInterval());
    m_requestTimer->stop();
}

QGeoPositionInfo QGeoPositionInfoSourceWin7::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
    if (fromSatellitePositioningMethodsOnly) {
        return m_lastPositionFromSatellite;
    }
    return m_lastPosition;
}

void QGeoPositionInfoSourceWin7::setPreferredPositioningMethods(PositioningMethods methods)
{
    if (!d->init())
        return ;

    d->setDesiredAccuracy(methods == SatellitePositioningMethods);
}

QGeoPositionInfoSourceWin7::PositioningMethods QGeoPositionInfoSourceWin7::supportedPositioningMethods() const
{
    return AllPositioningMethods;
}

void QGeoPositionInfoSourceWin7::setUpdateInterval(int msec)
{
#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoPositionInfoSourceWin7 called setUpdateInterval(" << msec << ")";
#endif

    if (!d->init())
        return ;

    // If msec is 0 we send updates as data becomes available, otherwise we force msec to be equal
    // to or larger than the minimum update interval.
    if (msec != 0 && msec < MinimumUpdateInterval)
        msec = MinimumUpdateInterval;
    if (m_updateStarted)
        d->setUpdateInterval(msec);
    // TODO: better error handling
    QGeoPositionInfoSource::setUpdateInterval(msec);
}

int QGeoPositionInfoSourceWin7::minimumUpdateInterval() const
{
    return MinimumUpdateInterval;
}

void QGeoPositionInfoSourceWin7::startUpdates()
{
    if (!d->init()) {
        emit updateTimeout();
        return ;
    }

    m_updateStarted = true;
    if (!d->startUpdates(updateInterval())) {
        m_updateStarted = false;
        emit updateTimeout();
        return ;
    }
}

void QGeoPositionInfoSourceWin7::stopUpdates()
{
    // Don't stop something not started
    if (!m_updateStarted)
        return ;

    m_updateStarted = false;
    if (!m_requestTimer->isActive())
        d->stopUpdates();
}

void QGeoPositionInfoSourceWin7::requestUpdate(int timeout)
{
    // Location API not available
    if (!d->init()) {
        emit updateTimeout();
        return ;
    }

    if (m_requestTimer->isActive())
        return ;

    // A timeout of 0 means to use the default timeout,
    // otherwise if timeout is less than the minimum update interval we emit a
    // updateTimeout signal
    if (timeout < minimumUpdateInterval() && timeout != 0)
        emit updateTimeout();
    else {
        // Start will only change the update interval if already started
        m_requestTimer->start(timeout);
        if (!m_updateStarted || timeout < d->getUpdateInterval()) {
            d->startUpdates(timeout);
        }
    }
}
