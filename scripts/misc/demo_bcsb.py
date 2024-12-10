import asyncio
import ssl

from websockets.client import connect

URI = "wss://sbo-brayns.shapes-registry.org:8000"
SSL = ssl.create_default_context()
REQUEST = """{"id": "1", "method": "version"}"""


async def main() -> None:
    async with connect(URI, ssl=SSL) as websocket:
        await websocket.send(REQUEST)
        data = await websocket.recv()
        print(data)


asyncio.run(main())
