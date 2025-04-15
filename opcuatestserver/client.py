import asyncio
import time
import logging
import os

from asyncua import Client



async def main():
    _logger = logging.getLogger(__name__)

    DEFAULT_PORT = '4840'
    DEFAULT_SERVER = 'server1'

    env = os.environ['PORT']
    server_name = os.environ['SERVER_NAME']
    if env is None or server_name is None:
        _logger.warning('No env variable found, using default instead ')
        env = DEFAULT_PORT
        server_name = DEFAULT_SERVER

    url = f'opc.tcp://{server_name}:{env}/freeopcua/server/'
    namespace = "http://examples.freeopcua.github.io"

    print(f"Connecting to {url} ...")
    while True:
        try:
            async with Client(url=url) as client:
                nsidx = await client.get_namespace_index(namespace)
                print(f"Namespace Index for '{namespace}': {nsidx}")

                object_name = "Object2"
                variable_name = "Variable2"
                var = await client.nodes.root.get_child(
                    f"0:Objects/{nsidx}:{object_name}/{nsidx}:{variable_name}"
                )

                while True:
                    await asyncio.sleep(1)
                    value = await var.read_value()
                    _logger.info(f"Value of MyVariable ({var}): {value}")
        except Exception as e:
            _logger.error(f"Connection failed: {e}. Retrying in 5 seconds...")
            time.sleep(5)

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    asyncio.run(main())