#include "opcua_core.h"
#include <QDebug>
namespace opc{
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

    bool OpcUaCore::connectOpc(const QString &url)
    {
        if (!m_client) {
            emit errorOccured("Client is not initialized.");
            return false;
        }

        if (!m_endpointsHooked) {
            connect(m_client, &QOpcUaClient::endpointsRequestFinished, this,
                    [this](const QVector<QOpcUaEndpointDescription> &endpoints,
                           QOpcUa::UaStatusCode status,
                           const QUrl &) {
                        if (endpoints.isEmpty() || status != QOpcUa::UaStatusCode::Good) {
                            emit errorOccured("No endpoints received or status not good.");
                            return;
                        }
                        m_client->connectToEndpoint(endpoints.first());
                    });
            m_endpointsHooked = true;
        }

        m_client->requestEndpoints(url);
        return true;
    }


    void OpcUaCore::disconnect()
    {
        if (m_client) {
            m_client->disconnectFromEndpoint();
            qDebug() << "Disconnected from OPC UA server.";
        }
    }

    void OpcUaCore::fetchDataFromSingleNode(const QString &nodeId)
    {
        if (!m_client || m_client->state() != QOpcUaClient::Connected) {
            emit errorOccured("Client is not connected.");
            return;
        }

        QOpcUaNode *node = m_client->node(nodeId);
        if (!node) {
            emit errorOccured("Failed to create node object.");
            return;
        }

        connect(node, &QOpcUaNode::attributeRead, this, [this, node](QOpcUa::NodeAttributes attrs) {
            if (attrs.testFlag(QOpcUa::NodeAttribute::Value)) {
                QVariant val = node->attribute(QOpcUa::NodeAttribute::Value);
                qDebug() << "Read value:" << val;
                emit valueRead(val);
            } else {
                emit errorOccured("Attribute read failed or did not include value.");
            }
            node->deleteLater(); // Clean up
        });

        node->readAttributes(QOpcUa::NodeAttribute::Value);
    }

    void OpcUaCore::browseRoot()
    {
        auto node = m_client->node("ns=0;i=85"); // Objects node
        connect(node, &QOpcUaNode::childrenRead, this, [node]() {
            for (const QString &childId : node->childrenIds()) {
                qDebug() << "Child NodeId:" << childId;
            }
        });
        node->browseChildren();
    }

}
