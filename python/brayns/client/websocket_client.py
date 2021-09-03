from typing import Callable, Union

import ssl
import websockets

from .event_loop import EventLoop


class WebsocketClient:

    def __init__(
        self,
        callback: Callable[[Union[bytes, str]], None]
    ) -> None:
        self._callback = callback
        self._loop = EventLoop()
        self._websocket = None
        self._poll_task = None

    @property
    def connected(self) -> bool:
        return self._websocket is not None and self._websocket.open

    def connect(
        self,
        uri: str,
        secure: bool = False,
        cafile: Union[None, str] = None
    ) -> None:
        if self.connected:
            self.disconnect()
        self._loop.start()
        self._loop.run(
            self._connect(uri, secure, cafile)
        ).result()
        self._polling_task = self._loop.run(
            self._poll()
        )

    def disconnect(self) -> None:
        if not self.connected:
            return
        self._loop.run(
            self._websocket.close()
        ).result()
        self._polling_task.cancel()
        try:
            self._polling_task.result()
        except:
            pass
        self._loop.stop()

    def send(self, data: Union[bytes, str]) -> None:
        self._loop.run(
            self._websocket.send(data)
        ).result()

    async def _connect(self, uri, secure, cafile):
        self._websocket = await websockets.connect(
            uri=('wss://' if secure else 'ws://') + uri,
            ssl=ssl.create_default_context(cafile=cafile) if secure else None,
            ping_interval=None
        )

    async def _poll(self):
        while True:
            self._callback(
                await self._websocket.recv()
            )
