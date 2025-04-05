import asyncio
import logging
import math

from asyncua import Server, ua
from asyncua.common.methods import uamethod

async def addNode(server, idx, objectName, variableName):
    myobj = await server.nodes.objects.add_object(idx, objectName)
    myvar = await myobj.add_variable(idx, variableName, 6.7)
    return myobj, myvar

async def writeData(_logger, name, start_angle, variable):
    angle = start_angle
    while True:
        await asyncio.sleep(1)
        _logger.debug(await variable.get_value())
        new_val = math.sin(angle * math.pi / 100) * 100
        _logger.info("%s: Set value of %s to %.1f", name, variable, new_val)
        await variable.write_value(new_val)
        angle = (angle + 1) % 200

async def main():
    _logger = logging.getLogger(__name__)

    server = Server()
    await server.init()
    server.set_endpoint("opc.tcp://0.0.0.0:4840/freeopcua/server/")

    uri = "http://examples.freeopcua.github.io"
    idx = await server.register_namespace(uri)

    object1_name = "Object1"
    object1, var1 = await addNode(server, idx, object1_name, "Variable1")

    object2_name = "Object2"
    object2, var2 = await addNode(server, idx, object2_name, "Variable2")

    _logger.info("Starting server!")
    async with server:
        await asyncio.gather(
            writeData(_logger, object1_name, 0, var1),
            writeData(_logger, object2_name, 100, var2)
        )
        

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    asyncio.run(main(), debug=True)