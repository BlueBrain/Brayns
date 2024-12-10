import asyncio
import json

from websockets.client import connect


async def test() -> None:
    async with connect("ws://localhost:5000", ping_interval=None) as socket:
        await socket.send(
            json.dumps(
                {
                    "jsonrpc": "2.0",
                    "id": 0,
                    "method": "render-image",
                    "params": {"send": True, "force": False},
                }
            )
        )
        reply = await socket.recv()
        print(reply)


asyncio.run(test())
