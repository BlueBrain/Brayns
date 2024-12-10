import asyncio
from logging import DEBUG, Logger, StreamHandler


from websockets.client import connect


async def main() -> None:
    logger = Logger("test", DEBUG)
    logger.addHandler(StreamHandler())

    async with connect(
        "ws://localhost:5000",
        open_timeout=None,
        ping_interval=1,
        ping_timeout=3,
        logger=logger,
    ) as websocket:
        await websocket.send("test")
        print(await websocket.recv())
        await websocket.wait_closed()


if __name__ == "__main__":
    asyncio.run(main())
