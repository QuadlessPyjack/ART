/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the demonstration applications of the Qt Toolkit.
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
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef DEVICE_H
#define DEVICE_H

#include <qtbluetoothglobal.h>
#include <QBluetoothLocalDevice>
#include <QObject>
#include <QVariant>
#include <QList>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyController>
#include <QBluetoothServiceInfo>

#include "deviceinfo.h"
#include "serviceinfo.h"
#include "characteristicinfo.h"
#include "payload.h"
#include "blewriter.h"

QT_FORWARD_DECLARE_CLASS (QBluetoothDeviceInfo)
QT_FORWARD_DECLARE_CLASS (QBluetoothServiceInfo)

static const QString BT_CharacteristicUUID = "{0000ffe1-0000-1000-8000-00805f9b34fb}";
static const QString BT_ServiceUUID = "{0000ffe0-0000-1000-8000-00805f9b34fb}";
static const QString BT_ServiceShortUUID = "0xffe0";
static const QString BT_HWAddress = "C8:FD:19:9F:FC:B4";

class Device: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant devicesList READ getDevices NOTIFY devicesUpdated)
    Q_PROPERTY(QVariant servicesList READ getServices NOTIFY servicesUpdated)
    Q_PROPERTY(QVariant characteristicList READ getCharacteristics NOTIFY characteristicsUpdated)
    Q_PROPERTY(QString update READ getUpdate WRITE setUpdate NOTIFY updateChanged)
    Q_PROPERTY(bool useRandomAddress READ isRandomAddress WRITE setRandomAddress NOTIFY randomAddressChanged)
    Q_PROPERTY(bool state READ state NOTIFY stateChanged)
    Q_PROPERTY(bool controllerError READ hasControllerError)
    Q_PROPERTY(QString message READ getMessage WRITE setMessage NOTIFY messageChanged)
public:
    Device(BLEWriter *writer = nullptr);
    ~Device();
    QVariant getDevices();
    QVariant getServices();
    QVariant getCharacteristics();
    QLowEnergyService *initServiceAndCharacteristic(QLowEnergyCharacteristic &characeristic);
    QString getUpdate();
    QString getMessage() { return m_message; }
    void setMessage(QString message) {
                                       m_message = message;
                                       emit messageChanged();
                                     }
    bool state();
    bool hasControllerError() const;

    bool isRandomAddress() const;
    void setRandomAddress(bool newValue);

    Q_INVOKABLE void writeMessage();
    Q_INVOKABLE void readMessageBegin();

    void deviceDiscoveryStopped();
public slots:
    void startDeviceDiscovery();
    void scanServices(const QString &address);

    void connectToService(const QString &uuid);
    void disconnectFromDevice();

private slots:
    // QBluetoothDeviceDiscoveryAgent related
    void addDevice(const QBluetoothDeviceInfo&);
    void deviceScanFinished();
    void deviceScanError(QBluetoothDeviceDiscoveryAgent::Error);

    // QLowEnergyController related
    void addLowEnergyService(const QBluetoothUuid &uuid);
    void deviceConnected();
    void errorReceived(QLowEnergyController::Error);
    void serviceScanDone();
    void deviceDisconnected();

    // QLowEnergyService related
    void serviceDetailsDiscovered(QLowEnergyService::ServiceState newState);

    void onCharacteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);

signals:
    void devicesUpdated();
    void servicesUpdated();
    void characteristicsUpdated();
    void updateChanged();
    void stateChanged();
    void disconnected();
    void randomAddressChanged();
    void messageChanged();
    void writeProceed();

private slots:
    void onCharacteristicWritten();

private:
    void setUpdate(QString message);
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    DeviceInfo currentDevice;
    QList<QObject*> devices;
    QList<QObject*> m_services;
    QList<QObject*> m_characteristics;
    QString m_previousAddress;
    QString m_message = "<placeholder>";
    bool connected;
    QLowEnergyController *controller;
    bool m_deviceScanState;
    bool randomAddress;

    QLowEnergyCharacteristic m_characteristic;
    QLowEnergyService *m_service;
};

#endif // DEVICE_H
