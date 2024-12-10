import asyncio
import json

from websockets.client import connect

URI = "ws://localhost:8000"
REQUEST = {
    "jsonrpc": "2.0",
    "id": "Mjk=",
    "method": "sonata-list-populations",
    "params": {
        "path": "/gpfs/bbp.cscs.ch/data/scratch/proj134/home/king/BBPP134-917/gpfs_simulation_config2.json",
    },
}


def parse(data: bytes) -> tuple[str, bytes]:
    size = int.from_bytes(data[:4], "little", signed=False)
    text = data[4 : 4 + size].decode()
    binary = data[4 + size :]
    return text, binary


async def main() -> None:
    async with connect(URI, ping_interval=None) as websocket:
        request = json.dumps(REQUEST)
        await websocket.send(request)
        reply = await websocket.recv()
        if isinstance(reply, bytes):
            text, binary = parse(reply)
            print(text)
            print(binary)
        if isinstance(reply, str):
            print(reply)


if __name__ == "__main__":
    asyncio.run(main())
