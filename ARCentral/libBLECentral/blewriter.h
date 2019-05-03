#ifndef BLEWRITER_H
#define BLEWRITER_H

#include <QObject>
#include <QTimer>
#include <QByteArray>

#include <QtBluetooth/qlowenergyservice.h>

#include "device/payload.h"

// forward declarations
class Device;

class BLEWriter : public QObject
{
    Q_OBJECT
public:
    BLEWriter();

    ~BLEWriter()
    {
        //nothing to do here
    }

    void Start()
    {
        m_transmissionTimer->start(1000);
    }

    void Write();
    void InitBLEDevice();
    void SetService(QLowEnergyService *service) { m_service = service; }
    void SetCharacteristic(QLowEnergyCharacteristic characteristic) { m_characteristic = characteristic; }

signals:
    void ReadyForTransmission();
    void TransmissionFinished();

public slots:
    void onWriteProceed();
    void writePayload(Payload p);

private:
    QTimer *m_transmissionTimer;
    QLowEnergyService *m_service;
    QLowEnergyCharacteristic m_characteristic;
    int m_transmissionIdx = 0;
    void transmitBytes();

    bool m_isWriteEnabled;
    bool m_transmitDelayed;
    Payload m_payload;

    Device *m_device;
};

#endif // BLEWRITER_H
