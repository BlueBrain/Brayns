from typing import Coroutine

import asyncio
import threading


class EventLoop:

    def __init__(self) -> None:
        self._loop = asyncio.new_event_loop()
        self._thread = None

    @property
    def running(self) -> bool:
        return self._loop.is_running()

    def start(self) -> None:
        if self.running:
            return
        self._thread = threading.Thread(
            target=self._loop.run_forever,
            daemon=True
        )
        self._thread.start()

    def stop(self) -> None:
        if not self.running:
            return
        self._loop.call_soon_threadsafe(self._loop.stop)

    def run(self, coroutine: Coroutine) -> asyncio.Future:
        return asyncio.run_coroutine_threadsafe(coroutine, self._loop)
