/***************************************************************************
**
** Copyright (C) 2013 BlackBerry Limited. All rights reserved.
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtBluetooth module of the Qt Toolkit.
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

#include "device.h"
#include <qbluetoothaddress.h>
#include <qbluetoothdevicediscoveryagent.h>
#include <qbluetoothlocaldevice.h>
#include <qbluetoothdeviceinfo.h>
#include <qbluetoothservicediscoveryagent.h>
#include <QDebug>
#include <QList>
#include <QTimer>
#include <QString>

Device::Device(BLEWriter *writer):
    connected(false), controller(nullptr),
    m_deviceScanState(false), randomAddress(false)
{
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    connect(discoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
            this, SLOT(addDevice(const QBluetoothDeviceInfo&)));
    connect(discoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
            this, SLOT(deviceScanError(QBluetoothDeviceDiscoveryAgent::Error)));
    connect(discoveryAgent, SIGNAL(finished()), this, SLOT(deviceScanFinished()));

    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled,
            this, &Device::deviceDiscoveryStopped);


    if (writer == nullptr) return;
    //writer setup follows:

    // informs writer when device is ready to write
    // TODO: add stop signal
    connect(this, &Device::writeProceed,
            writer, &BLEWriter::onWriteProceed);

    connect(writer, &BLEWriter::TransmissionFinished,
            this, &Device::onCharacteristicWritten);

    setUpdate("Search");
}

Device::~Device()
{
    delete discoveryAgent;
    delete controller;
    qDeleteAll(devices);
    qDeleteAll(m_services);
    qDeleteAll(m_characteristics);
    devices.clear();
    m_services.clear();
    m_characteristics.clear();
}

// called when discovery is stopped after we found our device
// TODO: handle stop in case of error or "device no longer needed" situations
void Device::deviceDiscoveryStopped()
{
    deviceScanFinished();
}

void Device::startDeviceDiscovery()
{
    qDeleteAll(devices);
    devices.clear();
    emit devicesUpdated();

    setUpdate("Scanning for devices ...");
    discoveryAgent->start();

    if (discoveryAgent->isActive()) {
        m_deviceScanState = true;
        Q_EMIT stateChanged();
    }
}

void Device::addDevice(const QBluetoothDeviceInfo &info)
{
    if (info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        DeviceInfo *d = new DeviceInfo(info);
        auto address = d->getAddress();
        qDebug() << address;

        // ignore unknown clients
        if (address != BT_HWAddress)
        {
            return;
        }

        devices.append(d);
        setUpdate("Last device added: " + d->getName());

        discoveryAgent->stop();
        //QMetaObject::invokeMethod(discoveryAgent, "stop",
        //                          Qt::QueuedConnection);
    }
}

void Device::deviceScanFinished()
{
    emit devicesUpdated();
    m_deviceScanState = false;
    emit stateChanged();
    if (devices.isEmpty())
    {
        setUpdate("No Low Energy devices found...");
        return;
    }

    setUpdate("Done! Scan Again!");
    scanServices(BT_HWAddress);
}

QVariant Device::getDevices()
{
    return QVariant::fromValue(devices);
}

QVariant Device::getServices()
{
    return QVariant::fromValue(m_services);
}

QVariant Device::getCharacteristics()
{
    return QVariant::fromValue(m_characteristics);
}

QString Device::getUpdate()
{
    return m_message;
}



void Device::scanServices(const QString &address)
{
    // We need the current device for service discovery.

    for (int i = 0; i < devices.size(); i++) {
        if (static_cast<DeviceInfo*>(devices.at(i))->getAddress() == address )
        {
            currentDevice.setDevice(static_cast<DeviceInfo*>(devices.at(i))->getDevice());
        }
    }

    if (!currentDevice.getDevice().isValid()) {
        qWarning() << "Not a valid device";
        return;
    }

    qDeleteAll(m_characteristics);
    m_characteristics.clear();
    emit characteristicsUpdated();
    qDeleteAll(m_services);
    m_services.clear();
    emit servicesUpdated();

    setUpdate("Back\n(Connecting to device...)");

    if (controller && m_previousAddress != currentDevice.getAddress()) {
        controller->disconnectFromDevice();
        delete controller;
        controller = nullptr;
    }

    if (!controller) {
        // Connecting signals and slots for connecting to LE services.
        controller = new QLowEnergyController(currentDevice.getDevice());
        connect(controller, SIGNAL(connected()),
                this, SLOT(deviceConnected()));
        connect(controller, SIGNAL(error(QLowEnergyController::Error)),
                this, SLOT(errorReceived(QLowEnergyController::Error)));
        connect(controller, SIGNAL(disconnected()),
                this, SLOT(deviceDisconnected()));
        connect(controller, SIGNAL(serviceDiscovered(QBluetoothUuid)),
                this, SLOT(addLowEnergyService(QBluetoothUuid)));
        connect(controller, SIGNAL(discoveryFinished()),
                this, SLOT(serviceScanDone()));
    }

    if (isRandomAddress())
        controller->setRemoteAddressType(QLowEnergyController::RandomAddress);
    else
        controller->setRemoteAddressType(QLowEnergyController::PublicAddress);
    controller->connectToDevice();

    m_previousAddress = currentDevice.getAddress();
}

void Device::addLowEnergyService(const QBluetoothUuid &serviceUuid)
{
    if (serviceUuid != QBluetoothUuid(BT_ServiceUUID)) return;

    QLowEnergyService *service = controller->createServiceObject(serviceUuid);
    if (!service) {
        qWarning() << "Cannot create service for uuid";
        return;
    }
    ServiceInfo *serv = new ServiceInfo(service);
    m_services.append(serv);

    emit servicesUpdated();
}

void Device::serviceScanDone()
{
    setUpdate("Back\n(Service scan done!)");
    // force UI in case we didn't find anything
    if (m_services.isEmpty())
    {
        emit servicesUpdated();
        return;
    }

    connectToService(BT_ServiceShortUUID);
}

void Device::connectToService(const QString &uuid)
{
    QLowEnergyService *service = nullptr;
    for (int i = 0; i < m_services.size(); i++) {
        ServiceInfo *serviceInfo = static_cast<ServiceInfo*>(m_services.at(i));
        if (serviceInfo->getUuid() == uuid) {
            service = serviceInfo->service();
            break;
        }
    }

    if (!service)
        return;

    qDeleteAll(m_characteristics);
    m_characteristics.clear();
    emit characteristicsUpdated();

    if (service->state() == QLowEnergyService::DiscoveryRequired) {
        connect(service, SIGNAL(stateChanged(QLowEnergyService::ServiceState)),
                this, SLOT(serviceDetailsDiscovered(QLowEnergyService::ServiceState)));
        service->discoverDetails();
        setUpdate("Back\n(Discovering details...)");

        return;
    }

    //discovery already done
    const QList<QLowEnergyCharacteristic> chars = service->characteristics();
    foreach (const QLowEnergyCharacteristic &ch, chars) {
        CharacteristicInfo *cInfo = new CharacteristicInfo(ch);
        m_characteristics.append(cInfo);
    }

    QTimer::singleShot(0, this, SIGNAL(characteristicsUpdated()));

    emit writeProceed();
}

void Device::deviceConnected()
{
    setUpdate("Back\n(Discovering services...)");
    connected = true;
    controller->discoverServices();
}

void Device::errorReceived(QLowEnergyController::Error /*error*/)
{
    qWarning() << "Error: " << controller->errorString();
    setUpdate(QString("Back\n(%1)").arg(controller->errorString()));
}

void Device::setUpdate(QString message)
{
    qDebug() << message;
    emit updateChanged();
}

void Device::disconnectFromDevice()
{
    // UI always expects disconnect() signal when calling this signal
    // TODO what is really needed is to extend state() to a multi value
    // and thus allowing UI to keep track of controller progress in addition to
    // device scan progress

    if (controller->state() != QLowEnergyController::UnconnectedState)
        controller->disconnectFromDevice();
    else
        deviceDisconnected();
}

void Device::deviceDisconnected()
{
    qWarning() << "Disconnect from device";
    emit disconnected();
}

void Device::serviceDetailsDiscovered(QLowEnergyService::ServiceState newState)
{
    if (newState != QLowEnergyService::ServiceDiscovered) {
        // do not hang in "Scanning for characteristics" mode forever
        // in case the service discovery failed
        // We have to queue the signal up to give UI time to even enter
        // the above mode
        if (newState != QLowEnergyService::DiscoveringServices) {
            emit characteristicsUpdated();
            //QMetaObject::invokeMethod(this, "characteristicsUpdated",
            //                          Qt::QueuedConnection);
        }
        return;
    }

    QLowEnergyService *service = qobject_cast<QLowEnergyService *>(sender());
    if (!service)
        return;

    const QList<QLowEnergyCharacteristic> chars = service->characteristics();
    foreach (const QLowEnergyCharacteristic &ch, chars) {
        CharacteristicInfo *cInfo = new CharacteristicInfo(ch);
        m_characteristics.append(cInfo);
    }

    emit writeProceed();
    emit characteristicsUpdated();
}

void Device::onCharacteristicRead(const QLowEnergyCharacteristic& /*&characteristic*/, const QByteArray &value)
{
    qDebug() << "Read characteristic: " << value;
    setMessage(QString(value));
}

void Device::onCharacteristicWritten()
{
    qDebug() << "Write finished!";
}

void Device::deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
        setUpdate("The Bluetooth adaptor is powered off, power it on before doing discovery.");
    else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError)
        setUpdate("Writing or reading from the device resulted in an error.");
    else
        setUpdate("An unknown error has occurred.");

    m_deviceScanState = false;
    emit devicesUpdated();
    emit stateChanged();
}

bool Device::state()
{
    return m_deviceScanState;
}

bool Device::hasControllerError() const
{
    if (controller && controller->error() != QLowEnergyController::NoError)
        return true;
    return false;
}

bool Device::isRandomAddress() const
{
    return randomAddress;
}

void Device::setRandomAddress(bool newValue)
{
    randomAddress = newValue;
    emit randomAddressChanged();
}

QLowEnergyService *Device::initServiceAndCharacteristic(QLowEnergyCharacteristic &characteristic)
{
    if (m_services.length() <= 0)
    {
        return nullptr;
    }

    if (m_services.at(0) == nullptr)
    {
        return nullptr;
    }

    ServiceInfo *serviceInfo = nullptr;
    serviceInfo = qobject_cast<ServiceInfo *>(m_services.at(0));
    if (serviceInfo == nullptr)
    {
        return nullptr;
    }

    characteristic = serviceInfo->service()->characteristic(QBluetoothUuid(BT_CharacteristicUUID));
    if (!characteristic.isValid())
    {
        qDebug() << "INVALID CHARACTERISTIC!";
        return nullptr;
    }

    return serviceInfo->service();
}

void Device::writeMessage()
{}

void Device::readMessageBegin()
{
    m_service = initServiceAndCharacteristic(m_characteristic);
    if(!m_service)
    {
        return;
    }

    connect(m_service, &QLowEnergyService::characteristicRead,
            this,    &Device::onCharacteristicRead);
    auto controllerState = controller->state();
    while(controllerState == QLowEnergyController::ControllerState::ConnectedState)
    {
        m_service->readCharacteristic(m_characteristic);
    }
}
