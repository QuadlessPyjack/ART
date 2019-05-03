#include <QtTest>

#include "utils/circularbuffer.h"
#include "utils/circularqueue.h"

// add necessary includes here

class Utils : public QObject
{
    Q_OBJECT

public:
    Utils();
    ~Utils();

private:
    Payload testPayload;
    CBufferPtr testCircularBuffer;

    CQueuePtr testCircularQueue;
    Data d0;
    Data d1;
    Data d2;
    Data d3;
    Payload p0;
    Payload p1;
    Payload p2;
    Payload p3;

private slots:
    void initTestCase();
    void test_checksumGeneration();
    void test_checksumCheckReturnValid();
    void test_checksumCheckReturnInvalid();
    void test_circularBufferInit();
    void test_circularBufferPut();
    void test_circularBufferGet();
    void test_circularBufferFull();
    void test_circularBufferFlush();
    void test_circularBufferEmpty();
    void test_circularBufferFirstEntryOverrideOnOverflow();
    void initCircularQueueTestCase();
    void test_circularQueueEnqueue();
    void test_circularQueueDequeue();
    void test_circularQueueFull();
    void test_circularQueueFlush();
    void test_circularQueueEmpty();
    void test_circularQueueEnqueueDequeueAll();
};

Utils::Utils()
{

}

Utils::~Utils()
{

}

void Utils::initTestCase()
{
    testPayload.data[X_COORDINATE] = 0b00000001;
    testPayload.data[Y_COORDINATE] = 0b00000011;
    testPayload.data[ROTATION]     = 0b00000100;
}

void Utils::test_checksumGeneration()
{
    setEvenParityByte(testPayload);
    QVERIFY2(testPayload.data[CHECKSUM] != 0, "Test checksum should not be 0");
    QVERIFY2(testPayload.data[CHECKSUM] == 0b00000101, "Wrong checksum value");
}

void Utils::test_checksumCheckReturnValid()
{
    int faultIdx = -2;
    bool isParityOk = isEvenParityValid(testPayload, faultIdx);

    QVERIFY2(faultIdx == -1, "Fault Index is set");
    QVERIFY2(isParityOk == true, "Even parity check failed" );
}

void Utils::test_checksumCheckReturnInvalid()
{
    Payload testPayload2;
    testPayload2.data[X_COORDINATE] = 0b00000001;
    testPayload2.data[Y_COORDINATE] = 0b00000011;
    testPayload2.data[ROTATION]     = 0b00000100;
    testPayload2.data[CHECKSUM]     = 0b00000001;

    int faultIdx = -2;
    bool isParityOk = isEvenParityValid(testPayload2, faultIdx);

    QVERIFY2(faultIdx == 2, "Wrong fault index value.");
    QVERIFY2(isParityOk == false, "Even parity check should have failed" );
}

void Utils::test_circularBufferInit()
{
    testCircularBuffer = cbuffer_init();
    QVERIFY2(testCircularBuffer->buffer[0] == 0, "Junk found.Should be 0");
    QVERIFY2(testCircularBuffer->count == 0, "Count should be 0.");
    QVERIFY2(testCircularBuffer->index == 0, "Index should be at 0");
    QVERIFY2(sizeof(testCircularBuffer->buffer) == CBUFFER_SIZE, "Wrong buffer size.");
}

void Utils::test_circularBufferPut()
{
    cbuffer_put(testCircularBuffer, 0b00000001);
    QVERIFY2(testCircularBuffer->buffer[0] == 0b00000001, "Wrong or no value inserted");
    QVERIFY2(testCircularBuffer->count == 1, "Wrong item count");
    QVERIFY2(testCircularBuffer->index == 1, "Wrong index position");
}

void Utils::test_circularBufferGet()
{
    uint8_t val = cbuffer_get(testCircularBuffer);
    QVERIFY2(val == 0b00000001, "Wrong or no value retrieved");
    QVERIFY2(testCircularBuffer->count == 0, "Wrong item count");
    QVERIFY2(testCircularBuffer->index == 0, "Wrong index position");
}

void Utils::test_circularBufferFull()
{
    for (uint8_t idx = 0; idx < CBUFFER_SIZE; ++idx)
    {
        cbuffer_put(testCircularBuffer, idx);
    }

    QVERIFY2(cbuffer_isFull(testCircularBuffer), "Invalid value reported for isFull");
}

void Utils::test_circularBufferFlush()
{
    cbuffer_flush(testCircularBuffer);
    QVERIFY2(testCircularBuffer->count == 0, "Buffer not properly flushed");
    QVERIFY2(testCircularBuffer->index == 0, "Buffer not properly flushed");
    QVERIFY2(testCircularBuffer->buffer[0] == 0, "Buffer not properly flushed");
}

void Utils::test_circularBufferEmpty()
{
    QVERIFY(cbuffer_isEmpty(testCircularBuffer));
}

void Utils::test_circularBufferFirstEntryOverrideOnOverflow()
{
    for (uint8_t idx = 0; idx < CBUFFER_SIZE; ++idx)
    {
        cbuffer_put(testCircularBuffer, idx);
    }

    const uint8_t TEST_VALUE = 0b00001111;

    cbuffer_put(testCircularBuffer, TEST_VALUE);

    QVERIFY2(testCircularBuffer->buffer[0] == TEST_VALUE, "Overflowed value not at IDX 0");
}

void Utils::initCircularQueueTestCase()
{
    testCircularQueue = cqueue_init();
    QVERIFY2(testCircularQueue->buffer[0].value.uint8 == 0, "Junk found.Should be 0");
    QVERIFY2(testCircularQueue->count == 0, "Count should be 0.");
    QVERIFY2(testCircularQueue->widx == 0, "Index should be at 0");
    QVERIFY2(testCircularQueue->ridx == 0, "Index should be at 0");
    QVERIFY2(sizeof(testCircularQueue->buffer) == (sizeof(Data) * CQUEUE_SIZE), "Wrong buffer size.");

    p0.data[0] = '0';
    p1.data[0] = '1';
    p2.data[0] = '2';
    p3.data[0] = '3';

    d0.type = Data::PAYLOAD;
    d1.type = Data::PAYLOAD;
    d2.type = Data::PAYLOAD;
    d3.type = Data::PAYLOAD;

    d0.value.payload = p0;
    d1.value.payload = p1;
    d2.value.payload = p2;
    d3.value.payload = p3;
}

void Utils::test_circularQueueEnqueue()
{
    cqueue_enqueue(testCircularQueue, d0);
    QVERIFY2(testCircularQueue->count == 1, "Enqueue failed at 1st element");
    cqueue_enqueue(testCircularQueue, d1);
    QVERIFY2(testCircularQueue->count == 2, "Enqueue failed at 1st element");

    QVERIFY2(testCircularQueue->widx == 2, "Write index at wrong position!");
    QVERIFY2(testCircularQueue->ridx == 0, "Read index at wrong position!");
}

void Utils::test_circularQueueDequeue()
{
    Data d = cqueue_dequeue(testCircularQueue);

    QVERIFY2(d.type == Data::PAYLOAD, "Union is of wrong type. Payload expected!");

    QVERIFY2(testCircularQueue->ridx == 1, "Read index at wrong position!");

    Payload p = d.value.payload;
    QVERIFY2(p.data[0] == '0', "Payload has wrong value. Expected p.data[0] == '0' ");
}

void Utils::test_circularQueueFull()
{

}

void Utils::test_circularQueueFlush()
{

}

void Utils::test_circularQueueEmpty()
{
    Data d = cqueue_dequeue(testCircularQueue);

    QVERIFY2(d.type == Data::PAYLOAD, "Union is of wrong type. Payload expected!");

    QVERIFY2(testCircularQueue->ridx == 2, "Read index at wrong position!");

    Payload p = d.value.payload;
    QVERIFY2(p.data[0] == '1', "Payload has wrong value. Expected p.data[0] == '1' ");

    QVERIFY2(testCircularQueue->count == 0, "Invalid value for count!");
}

void Utils::test_circularQueueEnqueueDequeueAll()
{
    CQueuePtr testQueue;
    testQueue = cqueue_init();

    cqueue_enqueue(testQueue, d0);
    cqueue_enqueue(testQueue, d1);
    cqueue_enqueue(testQueue, d2);
    cqueue_enqueue(testQueue, d3);

    Data d = cqueue_dequeue(testQueue);
    Payload p;
    p = d.value.payload;

    QVERIFY2(p.data[0] == '0', "Expected 0!");

    d = cqueue_dequeue(testQueue);
    p = d.value.payload;

    QVERIFY2(p.data[0] == '1', "Expected 1!");

    d = cqueue_dequeue(testQueue);
    p = d.value.payload;

    QVERIFY2(p.data[0] == '2', "Expected 2!");

    d = cqueue_dequeue(testQueue);
    p = d.value.payload;

    QVERIFY2(p.data[0] == '3', "Expected 3!");
}

QTEST_APPLESS_MAIN(Utils)

#include "tst_utils.moc"
