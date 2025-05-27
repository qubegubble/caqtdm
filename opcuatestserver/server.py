import time
import math
import os
import logging
from opcua import Server

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    log = logging.getLogger(__name__)

    # pick up PORT from env or default to 4840
    port = int(os.environ.get("PORT", "4840"))
    endpoint = f"opc.tcp://0.0.0.0:{port}/freeopcua/server/"

    # 1) set up server
    server = Server()
    server.set_endpoint(endpoint)
    uri = "http://examples.freeopcua.github.io"
    idx = server.register_namespace(uri)

    # 2) add two objects, each with one variable
    objects = server.get_objects_node()
    obj1 = objects.add_object(idx, "Object1")
    var1 = obj1.add_variable(idx, "Variable1", 6.7)
    var1.set_writable()   # clients can write if they wish
    # log the NodeId for Variable1
    log.info("Object1.Variable1 NodeId: %s", var1.nodeid.to_string())

    obj2 = objects.add_object(idx, "Object2")
    var2 = obj2.add_variable(idx, "Variable2", -6.7)
    var2.set_writable()
    # log the NodeId for Variable2
    log.info("Object2.Variable2 NodeId: %s", var2.nodeid.to_string())

    log.info(f"Starting OPC UA Server at {endpoint}")
    server.start()

    try:
        angle1 = 0
        angle2 = 100
        while True:
            time.sleep(1.0)

            # compute & write new values
            v1 = math.sin(angle1 * math.pi / 100) * 100
            v2 = math.sin(angle2 * math.pi / 100) * -100

            var1.set_value(v1)
            var2.set_value(v2)

            log.info("Object1.Variable1 -> %.1f", v1)
            log.info("Object2.Variable2 -> %.1f", v2)

            angle1 = (angle1 + 1) % 200
            angle2 = (angle2 + 1) % 200

    except KeyboardInterrupt:
        log.info("Shutting down server.")
    finally:
        server.stop()