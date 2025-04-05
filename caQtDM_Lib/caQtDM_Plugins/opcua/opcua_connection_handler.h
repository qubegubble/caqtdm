#ifndef OPCUA_CONNECTION_HANDLER_H
#define OPCUA_CONNECTION_HANDLER_H

#include <qcoreapplication.h>
#include <QOpcUaClient>

namespace opcua{
    class opcua_connection_handler{
        public:

            opcua_connection_handler();
            ~opcua_connection_handler();

            void connect(std::string connectionString);
            bool isConnected();
            void disconnect();


    };
}


#endif // OPCUA_CONNECTION_HANDLER_H
