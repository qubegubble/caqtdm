#include "opcua_core.h"
#include <QDebug>

OpcUaCore::OpcUaCore(QObject *parent)
    : QObject(parent), m_client(nullptr)
{
    m_client = m_provider.createClient("open62541");

    if (!m_client) {
        emit errorOccured("Failed to create OPC UA client instance.");
        return;
    }

    connect(m_client, &QOpcUaClient::connected, this, &OpcUaCore::connected);
    connect(m_client, &QOpcUaClient::disconnected, this, &OpcUaCore::disconnected);
    connect(m_client, &QOpcUaClient::errorChanged, this, [this](QOpcUaClient::ClientError error) {
        emit errorOccured(QString("Client error: %1").arg(static_cast<int>(error)));
    });

}

OpcUaCore::~OpcUaCore()
{
    if (m_client) {
        m_client->disconnectFromEndpoint();
        delete m_client;
        m_client = nullptr;
    }
}

bool OpcUaCore::connect(const QString &url)
{
    if (!m_client) {
        emit errorOccured("Client is not initialized.");
        return false;
    }

    m_client->connectToEndpoint(QUrl(url));
    return true;
}


void OpcUaCore::disconnect()
{
    if (m_client) {
        m_client->disconnectFromEndpoint();
        qDebug() << "Disconnected from OPC UA server.";
    }
}
