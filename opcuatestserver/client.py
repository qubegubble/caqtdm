import asyncio
import time
import logging

from asyncua import Client

url = "opc.tcp://server:4840/freeopcua/server/"
namespace = "http://examples.freeopcua.github.io"


async def main():
    _logger = logging.getLogger(__name__)

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