from opcua import Client
import time

if __name__ == '__main__':
    url = "opc.tcp://0.0.0.0:4840"
    client = Client(url)

    try:
        client.connect()
        print(f"Connected to {url}")

        objects = client.get_objects_node()
        children = objects.get_children()
        for child in children:
            print(f"Node: {child.get_browse_name()}")
    
        myobj = objects.get_child(["2:MyObject"])
        myvar = myobj.get_child(["2:MyVariable"])

        while True:
            time.sleep(1)

            value = myvar.get_value()
            print(f"Current value: {value}")


    except Exception as e:
        print(f"An error occurred: {e}")

    finally:
        client.disconnect()
        print("Disconnected from server")
