import asyncio
import json
import ssl
import time

import brayns
import requests
from websockets.client import WebSocketClientProtocol, connect

START = "https://vsm.kcp.bbp.epfl.ch/start"
STATUS = "https://vsm.kcp.bbp.epfl.ch/status/{job_id}"
TOKEN = "eyJhbGciOiJSUzI1NiIsInR5cCIgOiAiSldUIiwia2lkIiA6ICI5T0R3Z1JSTFVsTTJHbFphVDZjVklnenJsb0lzUWJmbTBDck1icXNjNHQ4In0.eyJleHAiOjE2OTk4NjYxNjYsImlhdCI6MTY5OTg2MjU2NiwiYXV0aF90aW1lIjoxNjk5ODYyNTY0LCJqdGkiOiIzYTZmYTM0OC0yOGI3LTRiN2ItOTE3MS00M2YzYjYyZTRkYjciLCJpc3MiOiJodHRwczovL2JicGF1dGguZXBmbC5jaC9hdXRoL3JlYWxtcy9CQlAiLCJhdWQiOlsiaHR0cHM6Ly9zbGFjay5jb20iLCJjb3Jlc2VydmljZXMtZ2l0bGFiIiwiYWNjb3VudCJdLCJzdWIiOiJmOjBmZGFkZWY3LWIyYjktNDkyYi1hZjQ2LWM2NTQ5MmQ0NTljMjphY2ZsZXVyeSIsInR5cCI6IkJlYXJlciIsImF6cCI6ImJicC1icmF5bnNjaXJjdWl0c3R1ZGlvIiwic2Vzc2lvbl9zdGF0ZSI6ImU5M2EwODVjLWE0MDAtNDIxYy04YWZjLWQ0NDZmYjY5ZmY2OCIsImFsbG93ZWQtb3JpZ2lucyI6WyJodHRwOi8vYnJheW5zY2lyY3VpdHN0dWRpby5rY3AuYmJwLmVwZmwuY2giLCJodHRwczovL2Rldi5icmF5bnNjaXJjdWl0c3R1ZGlvLmtjcC5iYnAuZXBmbC5jaCIsImh0dHA6Ly9sb2NhbGhvc3Q6ODA4MCIsImh0dHBzOi8vczIuYnJheW5zY2lyY3VpdHN0dWRpby5rY3AuYmJwLmVwZmwuY2giLCJodHRwOi8vczIuYnJheW5zY2lyY3VpdHN0dWRpby5rY3AuYmJwLmVwZmwuY2giLCJodHRwOi8vbG9jYWxob3N0OjgwODEiLCJodHRwOi8vZGV2LmJyYXluc2NpcmN1aXRzdHVkaW8ua2NwLmJicC5lcGZsLmNoIiwiaHR0cHM6Ly9icmF5bnNjaXJjdWl0c3R1ZGlvLmtjcC5iYnAuZXBmbC5jaCIsImh0dHA6Ly9zMS5icmF5bnNjaXJjdWl0c3R1ZGlvLmtjcC5iYnAuZXBmbC5jaCIsImh0dHBzOi8vczMuYnJheW5zY2lyY3VpdHN0dWRpby5rY3AuYmJwLmVwZmwuY2giLCJodHRwOi8vczMuYnJheW5zY2lyY3VpdHN0dWRpby5rY3AuYmJwLmVwZmwuY2giLCJodHRwczovL3MxLmJyYXluc2NpcmN1aXRzdHVkaW8ua2NwLmJicC5lcGZsLmNoIl0sInJlYWxtX2FjY2VzcyI6eyJyb2xlcyI6WyJiYnAtcGFtLWF1dGhlbnRpY2F0aW9uIiwib2ZmbGluZV9hY2Nlc3MiLCJ1bWFfYXV0aG9yaXphdGlvbiIsImRlZmF1bHQtcm9sZXMtYmJwIl19LCJyZXNvdXJjZV9hY2Nlc3MiOnsiaHR0cHM6Ly9zbGFjay5jb20iOnsicm9sZXMiOlsicmVzdHJpY3RlZC1hY2Nlc3MiXX0sImNvcmVzZXJ2aWNlcy1naXRsYWIiOnsicm9sZXMiOlsicmVzdHJpY3RlZC1hY2Nlc3MiXX0sImFjY291bnQiOnsicm9sZXMiOlsibWFuYWdlLWFjY291bnQiLCJtYW5hZ2UtYWNjb3VudC1saW5rcyIsInZpZXctcHJvZmlsZSJdfX0sInNjb3BlIjoib3BlbmlkIG5leHVzIHByb2ZpbGUgbG9jYXRpb24gZ3JvdXBzIGVtYWlsIiwic2lkIjoiZTkzYTA4NWMtYTQwMC00MjFjLThhZmMtZDQ0NmZiNjlmZjY4IiwiZW1haWxfdmVyaWZpZWQiOnRydWUsIm5hbWUiOiJBZHJpZW4gQ2hyaXN0aWFuIEZsZXVyeSIsImxvY2F0aW9uIjoiQjEgNCAyNjEuMDM4IiwicHJlZmVycmVkX3VzZXJuYW1lIjoiYWNmbGV1cnkiLCJnaXZlbl9uYW1lIjoiQWRyaWVuIENocmlzdGlhbiIsImZhbWlseV9uYW1lIjoiRmxldXJ5IiwiZW1haWwiOiJhZHJpZW4uZmxldXJ5QGVwZmwuY2gifQ.I5mCIpJ6xUzGqOHJdDMIHQMvTiBA3uLLFrt-jIG_pTKnwgkF3A0gteB67V89G_zK1Jfy6tUPEMJ8irng8pTMz7y-z0MDDaX-3eJX8YYNuw2P8Fvf4c0CGYIo1qsFDmoUQAZqgOppzopFXPHqO9-pHOnaygUafv1u-pEmLjOcioQsLyGyWQflRUAgGG4jNE2yM593jPYtAOzgOGuzAbOrT5HPQBkkVjAZhbfKF1T3gaGXRFQWHzA0K8lq8lVYZ8v-zAQ-HwKiU3PcPXpdGTaO4yAQhWKNeprpzQNx3Bzeo-G9WOiY-bsgUg9PKOVSHnmHyu79TYpWhC7Bd6owVLM1wg"
RENDERER = "vsm-proxy.kcp.bbp.epfl.ch/{job_id}/renderer?token={token}"
BACKEND = "wss://vsm-proxy.kcp.bbp.epfl.ch/{job_id}/backend?token={token}"
SSL = brayns.SslClientContext()
BACKEND_REQUEST = {
    "jsonrpc": "2.0",
    "id": "MjU=",
    "method": "sonata-list-populations",
    "params": {
        "path": "/gpfs/bbp.cscs.ch/data/scratch/proj134/sims/f12a89c3-e0bf-4be8-b02d-6fee921a2ec9_1/3/simulation_config.json"
    },
}
BACKEND_REQUEST = {"jsonrpc": "2.0", "id": "MjU=", "method": "version"}


def start() -> str:
    reply = requests.post(
        url=START,
        json={"usecase": "SBO1"},
        headers={"Authorization": f"Bearer {TOKEN}"},
        verify=False,
    )
    reply.raise_for_status()
    job = reply.json()
    return job["job_id"]


def status(job_id: str) -> bool:
    reply = requests.get(
        url=STATUS.format(job_id=job_id),
        headers={"Authorization": f"Bearer {TOKEN}"},
        verify=False,
    )
    if not reply.ok:
        return False
    job = reply.json()
    return job["job_running"] and job["brayns_started"]


def brayns_proxy(job_id: str) -> brayns.Version:
    uri = RENDERER.format(job_id=job_id, token=TOKEN)
    connector = brayns.Connector(uri, SSL)
    with connector.connect() as instance:
        return brayns.get_version(instance)


async def backend_direct(node: str) -> bytes | str:
    async with connect(f"ws://{node}.bbp.epfl.ch:8000") as websocket:
        await websocket.send(json.dumps(BACKEND_REQUEST))
        return await websocket.recv()


async def backend_proxy(job_id: str) -> bytes | str:
    uri = BACKEND.format(job_id=job_id, token=TOKEN)
    context = ssl.create_default_context()
    async with connect(uri, ssl=context) as websocket:
        await websocket.send(json.dumps(BACKEND_REQUEST))
        return await websocket.recv()


async def connect_backend(job_id: str) -> WebSocketClientProtocol:
    uri = BACKEND.format(job_id=job_id, token=TOKEN)
    context = ssl.create_default_context()
    return await connect(uri, ssl=context)


async def main() -> None:
    job_id = start()
    while not status(job_id):
        time.sleep(1)


if __name__ == "__main__":
    print(status("8078e508-60db-4ea8-9c1a-82772ed128ff"))
