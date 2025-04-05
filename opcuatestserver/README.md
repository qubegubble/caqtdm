# OPCUA Tesserver

Note: You have to have installed docker on your machine

## Run Testserver
- Go into this directory
- Build the server with `docker compose build`
- Start the containers with `docker compose up -d`
- See the logs with `docker compose logs -f server` or `docker compose logs -f client`
- Stop the containers with `docker compose down`

## Run Testserver without Docker Locally

```shell
# create virtual environment
python3 -m venv venv
# initialize virtual environment
source venv/bin/activate
# install dependencies
python3 -m pip install -r requirements.txt
# upgrade pip
python3 -m pip install --upgrade pip
# delete virtual environment
rm -rf venv
```

# Dokumentaiton von dependencies

- [OPCUA async documentation](https://opcua-asyncio.readthedocs.io/en/latest/usage/get-started/installation.html#)
- [OPCUA Informations](https://www.open62541.org/doc/master/)

