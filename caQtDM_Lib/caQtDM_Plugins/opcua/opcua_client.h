#ifndef OPCUA_CLIENT_H
#define OPCUA_CLIENT_H

#include <QObject>
#include <qstring.h>
#include "open62541/open62541.h"

class OpcUaClient : public QObject{
    Q_OBJECT
public:
    explicit OpcUaClient(QObject *parent = nullptr);
    ~OpcUaClient();

    bool connect(const QString &url);
    void disconnect();

signals:
    void connected();
    void disconnected();
    void errorOccured(const QString &message);

private:
    UA_Client *m_client;
};


#endif // OPCUA_CLIENT_H
