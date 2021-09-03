from typing import Any, Union

import json
import threading

from . import reply


class Message:

    def __init__(
        self,
        method: str,
        params: Any = None,
        request_id: Union[None, int, str] = None,
        jsonrpc: str = '2.0'
    ) -> None:
        self.method = method
        self.params = params
        self.request_id = request_id
        self.jsonrpc = jsonrpc

    def is_notification(self) -> bool:
        return self.request_id is None


class Monitor:

    def __init__(self):
        self._reply = None
        self._lock = threading.Condition()

    def get_reply(self, timeout: Union[None, float]) -> reply.Message:
        with self._lock:
            if not self._lock.wait(timeout):
                raise TimeoutError('Message timeout')
        return self._reply

    def set_reply(self, reply: reply.Message) -> None:
        with self._lock:
            self._reply = reply
            self._lock.notify_all()


class Manager:

    def __init__(self) -> None:
        self._requests = {}

    def add_request(self, request: Message) -> None:
        if not request.is_notification():
            self._requests[request.request_id] = Monitor()

    def add_reply(self, reply: reply.Message) -> None:
        request = self._requests.get(reply.request_id)
        if request is not None:
            request.set_reply(reply)

    def get_reply(
        self,
        request: Message,
        timeout: Union[None, float] = None
    ) -> reply.Message:
        request_id = request.request_id
        monitor = self._requests.get(request_id)
        if monitor is None:
            raise KeyError('No pending request with id {request_id!r}')
        try:
            return monitor.get_reply(timeout)
        finally:
            del self._requests[request_id]

    def clear(self):
        for request in self._requests.values():
            request.set_reply(
                reply.Message(
                    error={'code': 0, 'message': 'Connection closed'}
                )
            )
        self._requests.clear()


def to_json(request: Message) -> str:
    message = {
        'jsonrpc': request.jsonrpc,
        'method': request.method
    }
    if request.params is not None:
        message['params'] = request.params
    if request.request_id is not None:
        message['id'] = request.request_id
    return json.dumps(message)
