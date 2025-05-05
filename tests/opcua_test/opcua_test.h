#ifndef OPCUA_TEST_H
#define OPCUA_TEST_H

#include <QObject>
#include <QSignalSpy>

class opcua_test : public QObject{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void test_connection_success();
    void test_read_single_node();
    void test_connection_failure();
};

#endif // OPCUA_TEST_H
