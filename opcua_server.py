import time
import random

from opcua import ua, Server

if __name__ == '__main__':
    server = Server()

    server.set_endpoint('opc.tcp://0.0.0.0:4840')
    server.set_server_name('OPC UA Test Server')

    uri = "http://test.opcua.com"
    idx = server.register_namespace(uri)

    objects = server.get_objects_node()

    myobj = objects.add_object(idx, "MyObject")
    myvar = myobj.add_variable(idx, "MyVariable", 0)
    myvar.set_writable()

    server.start()

    print(f'OPC UA Server started at {server.endpoint}')

    try:
        while True:
            time.sleep(1)

            current_val = myvar.get_value()
            print(f'Current value: {current_val}')
            new_val = current_val + random.random()
            myvar.set_value(new_val)
    except KeyboardInterrupt:
        print('Server shutting down')
    finally:
        server.stop()
