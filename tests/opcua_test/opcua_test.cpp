#include "opcua_test.h"
#include "opcua_core.h"

#include <QtTest>
#include <QSignalSpy>

using namespace opc;

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

    QString testUrl = "opc.tcp://localhost:4841/freeopcua/server/";

    bool result = client.connectOpc(testUrl);

    QVERIFY(result);
    QVERIFY2(connectedSpy.wait(3000), "Did not receive 'connected' signal in time");
    QCOMPARE(connectedSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 0);

    client.disconnect();
}

void opcua_test::test_connection_failure()
{
    OpcUaCore client;

    QSignalSpy connectedSpy(&client, &OpcUaCore::connected);
    QSignalSpy errorSpy(&client, &OpcUaCore::errorOccured);

    QString testUrl = "opc.tcp://localhost:4999/freeopcua/server/";

    bool result = client.connectOpc(testUrl);
    QVERIFY(result);

    QCOMPARE(connectedSpy.count(), 0);
    QVERIFY(errorSpy.count() < 1);

    client.disconnect();
}

QTEST_MAIN(opcua_test)
#include "moc_opcua_test.cpp"
