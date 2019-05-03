#include "blewriter.h"
#include "device/payload.h"

#include "device/device.h"

BLEWriter::BLEWriter()
    : m_transmissionTimer(new QTimer(this)),
      m_service(nullptr),
      m_characteristic(),
      m_isWriteEnabled(false),
      m_transmitDelayed(true),
      m_device(new Device(this))
{

    m_device->startDeviceDiscovery();

    m_transmissionTimer->setInterval(0); //300
    connect(m_transmissionTimer, &QTimer::timeout,
            this, &BLEWriter::transmitBytes);
}

void BLEWriter::InitBLEDevice()
{
    qDebug() << "Starting BLE Device Discovery & Init";
    m_device->startDeviceDiscovery();
}

void BLEWriter::Write()
{

}

// triggered when device is ready for write
void BLEWriter::onWriteProceed()
{
    qDebug() << "BLE Device connected";
    m_isWriteEnabled = true;

    m_service = m_device->initServiceAndCharacteristic(m_characteristic);
    if(!m_service)
    {
        m_isWriteEnabled = false;
        return;
    }

    emit ReadyForTransmission();
}

void BLEWriter::writePayload(Payload p)
{
    if (m_isWriteEnabled == false)
    {
        return;
    }

    m_payload = p;

    if (m_transmitDelayed)
    {
        m_transmissionTimer->start();
        return;
    }

    transmitBytes();
}

void resetPayload(Payload &p)
{
    memset(p.data, 0, PAYLOAD_SIZE * sizeof(p.data[0]));
}

void BLEWriter::transmitBytes()
{
    if (!m_isWriteEnabled)
    {
        return;
    }

    if (m_transmissionIdx >= PAYLOAD_SIZE)
    {
        m_transmissionTimer->stop();
        m_transmissionIdx = 0;
        qDebug() << "Send struct complete";
        qDebug() << "Length is: " << sizeof(m_payload.data);
        resetPayload(m_payload);
        emit TransmissionFinished();
        onWriteProceed();

        return;
    }
    //QByteArray payloadBA = QByteArray(&m_payload.data[m_transmissionIdx], 1);
    QByteArray payloadBA = QByteArray(&m_payload.data[0], 4);
    m_service->writeCharacteristic(m_characteristic, payloadBA, QLowEnergyService::WriteWithoutResponse);

    qDebug() << "Send Byte["<<m_transmissionIdx<<"] complete.";
    //++m_transmissionIdx;
    m_transmissionIdx += 4;
}
