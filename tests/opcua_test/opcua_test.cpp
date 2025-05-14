#include "opcua_test.h"
#include "opcua_core.h"

#include <QtTest>
#include <QSignalSpy>
#include <algorithm>

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
}

void opcua_test::test_read_random_single_node()
{
    OpcUaCore client;
    QSignalSpy connectedSpy(&client, &OpcUaCore::connected);
    QSignalSpy valueSpy(&client, &OpcUaCore::valueRead);
    QSignalSpy errorSpy(&client, &OpcUaCore::errorOccured);

    QString testUrl = "opc.tcp://localhost:4841/freeopcua/server/";

    QVERIFY(client.connectOpc(testUrl));
    QVERIFY(connectedSpy.wait(3000)); // Wait for connection
    QCOMPARE(connectedSpy.count(), 1);

    client.fetchDataFromAnyNode();
    QVERIFY(valueSpy.wait(3000)); // Wait for value to be read

    QCOMPARE(errorSpy.count(), 0);
    QVERIFY(valueSpy.count() > 0); // We received at least one value

    QVariant value = valueSpy.takeFirst().at(0);
    qDebug() << "Read value in test:" << value;
}

void opcua_test::test_read_specific_single_node()
{
    OpcUaCore client;
    QSignalSpy connectedSpy(&client, &OpcUaCore::connected);
    QSignalSpy valueSpy(&client, &OpcUaCore::valueRead);
    QSignalSpy errorSpy(&client, &OpcUaCore::errorOccured);

    QString testUrl = "opc.tcp://localhost:4841/freeopcua/server/";

    QVERIFY(client.connectOpc(testUrl));
    QVERIFY(connectedSpy.wait(3000));
    QCOMPARE(connectedSpy.count(), 1);

    client.fetchDataFromSingleNode("ns=2;i=4");
    QVERIFY(valueSpy.wait(3000));

    QCOMPARE(errorSpy.count(), 0);
    QVERIFY(valueSpy.count() > 0);

    QVariant value = valueSpy.takeFirst().at(0);
    qDebug() << "Read value in test: " << value;
}

void opcua_test::test_read_multiple_nodes()
{
    OpcUaCore client;
    QSignalSpy connectedSpy(&client, &OpcUaCore::connected);
    QSignalSpy valueSpy(&client, &OpcUaCore::valueRead);
    QSignalSpy errorSpy(&client, &OpcUaCore::errorOccured);

    QString testUrl = "opc.tcp://localhost:4841/freeopcua/server/";

    QVERIFY(client.connectOpc(testUrl));
    QVERIFY(connectedSpy.wait(3000));
    QCOMPARE(connectedSpy.count(), 1);

    auto nodes = new QStringList{
      "ns=2;i=4",
      "ns=2;i=2"
    };

    client.fetchDataFromMultipleNodes(*nodes);
    QVERIFY(valueSpy.wait(3000));

    QTRY_COMPARE(valueSpy.count(), nodes->size());
    QCOMPARE(errorSpy.count(), 0);

    QStringList returnedIds;
    for(auto &call : valueSpy){
        returnedIds << call.at(0).toString();
    }

    std::sort(nodes->begin(), nodes->end());
    std::sort(returnedIds.begin(), returnedIds.end());
    QCOMPARE(returnedIds, *nodes);
}



void opcua_test::test_connection_failure()
{
    OpcUaCore client;

    QSignalSpy connectedSpy(&client, &OpcUaCore::connected);
    QSignalSpy errorSpy(&client, &OpcUaCore::errorOccured);

    QString testUrl = "opc.tcp://localhost:4999/freeopcua/server/";

    bool result = client.connectOpc(testUrl);
    QVERIFY(result);

    QVERIFY(errorSpy.wait(6000));// block up to 6s for an error else this test will be a false negative.
    QVERIFY(errorSpy.count() >= 1);

    client.disconnect();
}

QTEST_MAIN(opcua_test)
#include "moc_opcua_test.cpp"
