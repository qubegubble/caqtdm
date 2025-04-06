#include "opcua_client.h"
#include <QDebug>

OpcUaClient::OpcUaClient(QObject *parent) : QObject(parent), m_client(nullptr) {

    m_client = UA_Client_new();
    if(m_client){
        UA_ClientConfig_setDefault(UA_Client_getConfig(m_client));
    }else{
        emit errorOccured("Failed to create OPC UA client instance.");
    }
}

OpcUaClient::~OpcUaClient(){

    disconnect();
    if(m_client){
        UA_Client_delete(m_client);
        m_client = nullptr;
    }
}

bool OpcUaClient::connect(const QString &url){
    if(!m_client){
        emit errorOccured("Client is not initialized");
        return false;
    }

    UA_StatusCode status = UA_Client_connect(m_client, url.toUtf8().constData());
    if(status != UA_STATUSCODE_GOOD){
        UA_Client_disconnect(m_client);
        emit errorOccured(QString("Failed to connect: %1").arg(UA_StatusCode_name(status)));
        return false;
    }

    emit connected();
    qDebug() << "Connected to OPC UA server at " << url;
    return true;
}

void OpcUaClient::disconnect(){
    if(m_client){
        UA_Client_disconnect(m_client);
        emit disconnected();
        qDebug() << "Disconnected from OPC UA server.";
    }
}

