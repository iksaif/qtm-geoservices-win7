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
#include <qgeopositioninfo.h>
#include <qgeocoordinate.h>

#include "qgeopositioninfosource_win7.h"

#include <propvarutil.h>

/* =========== Helpers ============= */

void ConvertKnotsToMetresPerSecond(double* knots) {
  *knots *= 0.5144;
}

QDateTime ConvertSystemTimeToQDateTime(SYSTEMTIME* st)
{
    QDate date = QDate(st->wYear, st->wMonth, st->wDay);
    QTime time = QTime(st->wHour, st->wMinute, st->wSecond, st->wMilliseconds);
    return QDateTime(date, time);
}

bool SetCoordinates(CComPtr<ILatLongReport> pLatLongReport, QGeoPositionInfo* info)
{
    HRESULT hr;
    double latitude, longitude, altitude;

    info->setCoordinate(QGeoCoordinate());

    hr = pLatLongReport->GetLatitude(&latitude);
    if (!SUCCEEDED(hr))
      return false;
    hr = pLatLongReport->GetLongitude(&longitude);
    if (!SUCCEEDED(hr))
      return false;
    hr = pLatLongReport->GetAltitude(&altitude);
    if (!SUCCEEDED(hr))
      altitude = 0; // Altitude is not mandatory
#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "SetCoordinates:" << latitude << longitude << altitude;
#endif
    info->setCoordinate(QGeoCoordinate(latitude, longitude, altitude));
    return true;
}

bool SetTimestamp(CComPtr<ILatLongReport> pLatLongReport, QGeoPositionInfo* info)
{
    HRESULT hr;
    SYSTEMTIME st;

    info->setTimestamp(QDateTime::currentDateTime());

    hr = pLatLongReport->GetTimestamp(&st);
    if (!SUCCEEDED(hr))
        return false;

    QDate date = QDate(st.wYear, st.wMonth, st.wDay);
    QTime time = QTime(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    QDateTime timestamp = QDateTime(date, time);

    if (!timestamp.isValid())
        return false;
    info->setTimestamp(timestamp);
    return true;
}

void SetAccuracy(CComPtr<ILatLongReport> pLatLongReport, QGeoPositionInfo* info)
{
    HRESULT hr;
    double accuracy, vertical_accuracy;

    hr = pLatLongReport->GetErrorRadius(&accuracy);
    if (SUCCEEDED(hr) && accuracy > 0)
        info->setAttribute(QGeoPositionInfo::HorizontalAccuracy, accuracy);

    hr = pLatLongReport->GetAltitudeError(&vertical_accuracy);
    if (SUCCEEDED(hr))
        info->setAttribute(QGeoPositionInfo::VerticalAccuracy, vertical_accuracy);
#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "SetAccuracy:" << accuracy << vertical_accuracy;
#endif
}

void SetExtraAttributes(CComPtr<ILatLongReport> pLatLongReport, QGeoPositionInfo* info)
{
    HRESULT hr;
    PROPVARIANT variant;
    double value;

    PropVariantInit(&variant);

    // Direction
    hr = pLatLongReport->GetValue(
        SENSOR_DATA_TYPE_TRUE_HEADING_DEGREES, &variant);
    if (SUCCEEDED(hr)) {
      PropVariantToDouble(variant, &value);
      info->setAttribute(QGeoPositionInfo::Direction, value);
    }

    // Speed
    hr = pLatLongReport->GetValue(
        SENSOR_DATA_TYPE_SPEED_KNOTS, &variant);
    if (SUCCEEDED(hr)) {
      PropVariantToDouble(variant, &value);
      ConvertKnotsToMetresPerSecond(&value);
      info->setAttribute(QGeoPositionInfo::GroundSpeed, value);
    }

    // Magnetic variation
    hr = pLatLongReport->GetValue(
        SENSOR_DATA_TYPE_MAGNETIC_VARIATION, &variant);
    if (SUCCEEDED(hr)) {
      PropVariantToDouble(variant, &value);
      info->setAttribute(QGeoPositionInfo::MagneticVariation, value);
    }
    return ;
}

/* ========== CLocationEvents ============ */

QGeoLocationEventsWin7::QGeoLocationEventsWin7(QObject* parent)
    : QObject(parent)
{
}

QGeoLocationEventsWin7::~QGeoLocationEventsWin7()
{
}

// This is called when there is a new location report.
STDMETHODIMP QGeoLocationEventsWin7::OnLocationChanged(REFIID reportType,
    ILocationReport* pLocationReport)
{
    HRESULT hr;
    CComPtr<ILatLongReport> spLatLongReport;

#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoLocationEventsWin7: OnLocationChanged";
#endif

    // If the report type is a Latitude/Longitude report
    // (as opposed to IID_ICivicAddressReport or another type)
    if (IID_ILatLongReport != reportType)
        return S_OK;

    // Get the ILatLongReport interface from ILocationReport
    hr = pLocationReport->QueryInterface(IID_PPV_ARGS(&spLatLongReport));
    if (!SUCCEEDED(hr) || spLatLongReport.p == NULL)
        return S_OK;

    QGeoPositionInfo info;

    if (!SetCoordinates(spLatLongReport, &info))
        return false;
    SetTimestamp(spLatLongReport, &info);
    SetAccuracy(spLatLongReport, &info);
    SetExtraAttributes(spLatLongReport, &info);

#ifdef Q_LOCATION_WIN7_DEBUG
    qDebug() << "QGeoLocationEventsWin7::positionUpdated:"
             << info;
#endif
    if (!info.isValid())
        emit updateError();
    else
        emit positionUpdated(info);
    return S_OK;
}

// This is called when the status of a report type changes.
// The LOCATION_REPORT_STATUS enumeration is defined in LocApi.h in the SDK
STDMETHODIMP QGeoLocationEventsWin7::OnStatusChanged(REFIID reportType, LOCATION_REPORT_STATUS status)
{
    if (IID_ILatLongReport != reportType)
        return S_OK;

    switch (status)
    {
    case REPORT_NOT_SUPPORTED:
    case REPORT_ERROR:
    case REPORT_ACCESS_DENIED:
        emit updateError();  // TODO only do that once (or after a previous positionUpdated)
        break;
    }

#ifdef Q_LOCATION_WIN7_DEBUG
    switch (status)
    {
    case REPORT_NOT_SUPPORTED:
        qDebug() << "QGeoLocationEventsWin7: No location sensor installed for this report type.";
        break;
    case REPORT_ERROR:
        qDebug() << "QGeoLocationEventsWin7: Report error.";
        break;
    case REPORT_ACCESS_DENIED:
        qDebug() << "QGeoLocationEventsWin7: Access denied to reports.";
        break;
    case REPORT_INITIALIZING:
        qDebug() << "QGeoLocationEventsWin7: Report is initializing.";
        break;
    case REPORT_RUNNING:
        qDebug() << "QGeoLocationEventsWin7: Running.";
        break;
    }
#endif
    return S_OK;
}
