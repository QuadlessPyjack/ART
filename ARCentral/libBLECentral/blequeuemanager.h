#ifndef BLEQUEUEMANAGER_H
#define BLEQUEUEMANAGER_H

#include <QObject>
#include <QQueue>
#include <QTimer>

#include <device/payload.h>

#include "libblecentral_global.h"

#include "dto.h"
#include "blewriter.h"

class LIBBLECENTRALSHARED_EXPORT BLEQueueManager : public QObject
{
    Q_OBJECT
public:
    explicit BLEQueueManager(QObject *parent = nullptr);
    void InitWriter();

    void AddToQueue(const DataPoint &dp);

signals:
    void writePayload(Payload p);
    void startEnqueueTimer();

public slots:
    void onCommandDataReceived();


private:
    void SetWriter(BLEWriter *writer);
    void writeNext();
    void setTransmissionFinished() { m_isTransmissionFinished = true; }
    void setTransmissionReady() { m_isTransmissionReady = true; }
    void setTransmissionFinishedStatus(bool transmissionFinishedStatus) { m_isTransmissionFinished = transmissionFinishedStatus; }
    void setTransmissionReadyStatus(bool transmissionReadyStatus) { m_isTransmissionReady = transmissionReadyStatus; }
    bool m_isTransmissionFinished;
    bool m_isTransmissionReady;
    QTimer *m_delayedDequeueTimer;
    BLEWriter *m_writer;
    Payload dataPointToPayload(const DataPoint &);
    QQueue<DataPoint> m_outgoingQueue;
};

#endif // BLEQUEUEMANAGER_H
