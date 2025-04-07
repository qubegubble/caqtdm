#include <QtTest>
#include "opcua_client.h"

// add necessary includes here

class opcua_test : public QObject
{
    Q_OBJECT

public:
    opcua_test();
    ~opcua_test();

private slots:
    void test_case1();
};

opcua_test::opcua_test() {}

opcua_test::~opcua_test() {}

void opcua_test::test_case1() {}

QTEST_APPLESS_MAIN(opcua_test)

#include "tst_opcua_test.moc"
