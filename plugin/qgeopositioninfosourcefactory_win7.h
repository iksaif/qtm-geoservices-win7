/****************************************************************************
**
** Copyright (C) 2012 Corentin Chary
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Corentin Chary (corentin.chary@gmail.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOPOSITIONINFOSOURCEFACTORY_WIN7_H
#define QGEOPOSITIONINFOSOURCEFACTORY_WIN7_H

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

#include <qgeopositioninfosourcefactory>
#include <QObject>

QTM_USE_NAMESPACE

class QGeoPositionInfoSourceFactoryWin7 : public QObject, public QGeoPositionInfoSourceFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QGeoPositionInfoSourceFactory)
public:
    QGeoPositionInfoSourceFactoryWin7();
    ~QGeoPositionInfoSourceFactoryWin7();

    QGeoPositionInfoSource* positionInfoSource(QObject* parent);
    QGeoSatelliteInfoSource* satelliteInfoSource(QObject* parent);

    QString sourceName() const;
    int sourcePriority() const;
    int sourceVersion () const;
};

#endif

