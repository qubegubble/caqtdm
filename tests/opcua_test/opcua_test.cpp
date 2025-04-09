#include "opcua_test.h"
#include "opcua_core.h"

#include <QtTest>
#include <QSignalSpy>

void opcua_test::initTestCase()
{
    qDebug() << "Starting OPC UA test suite...";
}

void opcua_test::cleanupTestCase()
{
    qDebug() << "Finished OPC UA test suite.";
}

void opcua_test::test_connection_success()
{
    OpcUaCore client;

    QSignalSpy connectedSpy(&client, &OpcUaCore::connected);
    QSignalSpy errorSpy(&client, &OpcUaCore::errorOccured);

    QString testUrl = "opc.tcp://localhost:4840";

    bool result = client.connect(testUrl);

    QVERIFY(result);
    QCOMPARE(connectedSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 0);

    client.disconnect();
}

void opcua_test::test_connection_failure()
{
    OpcUaCore client;

    QSignalSpy connectedSpy(&client, &OpcUaCore::connected);
    QSignalSpy errorSpy(&client, &OpcUaCore::errorOccured);

    QString testUrl = "opc.tcp://localhost:9999";

    bool result = client.connect(testUrl);

    QVERIFY(!result);
    QCOMPARE(connectedSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);

    client.disconnect();
}

QTEST_MAIN(opcua_test)
#include "moc_opcua_test.cpp"
