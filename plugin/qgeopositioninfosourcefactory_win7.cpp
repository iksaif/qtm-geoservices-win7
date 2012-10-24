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

#include "qgeopositioninfosourcefactory_win7.h"
#include "qgeopositioninfosource_win7.h"

#include <QtPlugin>

QGeoPositionInfoSourceFactoryWin7::QGeoPositionInfoSourceFactoryWin7() { }

QGeoPositionInfoSourceFactoryWin7::~QGeoPositionInfoSourceFactoryWin7() {}

QGeoPositionInfoSource* QGeoPositionInfoSourceFactoryWin7::positionInfoSource(QObject* parent)
{
    return new QGeoPositionInfoSourceWin7(parent);
}

QGeoSatelliteInfoSource* QGeoPositionInfoSourceFactoryWin7::satelliteInfoSource(QObject* parent)
{
    Q_UNUSED(parent);
    return 0;
}

QString QGeoPositionInfoSourceFactoryWin7::sourceName() const
{
    return "win7";
}

int QGeoPositionInfoSourceFactoryWin7::sourceVersion() const
{
    return 1;
}

int QGeoPositionInfoSourceFactoryWin7::sourcePriority() const
{
    return 100;
}

Q_EXPORT_PLUGIN2(qtposition_win7, QGeoPositionInfoSourceFactoryWin7)

