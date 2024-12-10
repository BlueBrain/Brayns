import asyncio
import json
from struct import Struct

from websockets.client import connect

URI = "ws://localhost:8000"
REGISTER = {
    "jsonrpc": "2.0",
    "id": "Mjk=",
    "method": "sonata-load-nodes",
    "params": {
        "path": "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/GeneratedTest/simulation_config.json",
        "population": "cerebellum_neurons",
        "count": 5,
    },
}
REGISTER = {
    "jsonrpc": "2.0",
    "id": "Mjk=",
    "method": "sonata-load-nodes",
    "params": {
        "path": "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/FULL_BRAIN_WITH_SIM_15_06_2023/simulation_config.json",
        "population": "root__neurons",
        "count": 100,
        "node_sets": ["Excitatory", "Inhibitory"],
    },
}
UNREGISTER = {
    "jsonrpc": "2.0",
    "id": "Mjk=",
    "method": "sonata-unload-nodes",
    "params": {"id": 0},
}
IDS = {
    "jsonrpc": "2.0",
    "id": "Mjk=",
    "method": "sonata-get-node-ids",
    "params": {"id": 0},
}
POSITIONS = {
    "jsonrpc": "2.0",
    "id": "Mjk=",
    "method": "sonata-get-node-positions",
    "params": {"id": 0},
}
REPORT = {
    "jsonrpc": "2.0",
    "id": "Mjk=",
    "method": "sonata-load-node-report",
    "params": {"nodes_id": 0, "name": "soma"},
}
UNREPORT = {
    "jsonrpc": "2.0",
    "id": "Mjk=",
    "method": "sonata-unload-node-report",
    "params": {"id": 0},
}
FRAME = {
    "jsonrpc": "2.0",
    "id": "Mjk=",
    "method": "sonata-get-report-frame",
    "params": {
        "report_id": 0,
        "frame": 36,
        "min_value": -100,
        "max_value": 80,
    },
}
REQUEST = IDS


def parse(data: bytes) -> tuple[str, bytes]:
    size = int.from_bytes(data[:4], "little", signed=False)
    text = data[4 : 4 + size].decode()
    binary = data[4 + size :]
    return text, binary


def parse_positions(data: bytes) -> list[tuple[float, float, float]]:
    layout = Struct("3f")
    result = []
    for i in range(0, len(data), 12):
        result.append(layout.unpack(data[i : i + 12]))
    return result


def parse_frame(data: bytes) -> tuple[float, ...]:
    layout = Struct(f"{len(data) // 4}f")
    return layout.unpack(data)


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
