#include "blequeuemanager.h"

BLEQueueManager::BLEQueueManager(QObject *parent) : QObject(parent)
  , m_isTransmissionFinished(false)
  , m_isTransmissionReady(false)
  , m_delayedDequeueTimer(new QTimer(this))
  , m_writer(nullptr)
{
    m_writer = new BLEWriter();

    connect(this, &BLEQueueManager::writePayload,
            m_writer, &BLEWriter::writePayload);

    // enqueue next write
    //connect(m_writer, &BLEWriter::TransmissionFinished,
    //        this, &BLEQueueManager::writeNext);

    //connect(m_writer, &BLEWriter::ReadyForTransmission,
    //        this, &BLEQueueManager::writeNext);

    connect(m_writer, &BLEWriter::TransmissionFinished,
            this, &BLEQueueManager::setTransmissionFinished);

    connect(m_writer, &BLEWriter::ReadyForTransmission,
            this, &BLEQueueManager::setTransmissionReady);

    m_delayedDequeueTimer->setInterval(0); //30

    connect(m_delayedDequeueTimer, &QTimer::timeout,
            this, &BLEQueueManager::writeNext);

    m_delayedDequeueTimer->start();
}

void BLEQueueManager::SetWriter(BLEWriter *writer)
{
    m_writer = writer;

    //m_writer->InitBLEDevice();
}

void BLEQueueManager::AddToQueue(const DataPoint &dp)
{
    m_outgoingQueue.enqueue(dp);

    //emit onCommandDataReceived();
}

void BLEQueueManager::onCommandDataReceived()
{
    if (m_outgoingQueue.count() == 0) return;
    if (!(m_isTransmissionReady || m_isTransmissionFinished)) return;

    DataPoint dp = m_outgoingQueue.dequeue();
    Payload p = dataPointToPayload(dp);

    emit writePayload(p);
    setTransmissionFinishedStatus(false);
    setTransmissionReadyStatus(false);
}

void BLEQueueManager::writeNext()
{
    onCommandDataReceived();
}

Payload BLEQueueManager::dataPointToPayload(const DataPoint &dp)
{
    Payload p;
    p.data[X_COORDINATE] = static_cast<char>(dp.X);
    p.data[Y_COORDINATE] = static_cast<char>(dp.Y);

    setEvenParityByte(p);

    p.data[METADATA] |= dp.type << 3;

    return p;
}
