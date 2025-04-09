#ifndef OPCUA_CLIENT_H
#define OPCUA_CLIENT_H

#include <QObject>
#include <QtOpcUa/QOpcUaClient>
#include <QtOpcUa/QOpcUaProvider>

class OpcUaCore : public QObject
{
    Q_OBJECT

public:
    explicit OpcUaCore(QObject *parent = nullptr);
    ~OpcUaCore();

    bool connect(const QString &url);
    void disconnect();

signals:
    void connected();
    void disconnected();
    void errorOccured(const QString &message);

private:
    QOpcUaProvider m_provider;
    QOpcUaClient *m_client;
};

#endif // OPCUA_CLIENT_H
