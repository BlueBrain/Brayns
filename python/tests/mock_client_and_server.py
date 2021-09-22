from typing import Any

from brayns.client.client import Client

from mock_server import MockServer


class MockClientAndServer:

    def __init__(
        self,
        host: str,
        port: int,
        request_handler: MockServer.RequestHandler,
        secure: bool = False,
        certfile: str = None,
        keyfile: str = None,
        password: str = None,
    ) -> None:
        self._server = MockServer(
            host=host,
            port=port,
            request_handler=request_handler,
            secure=secure,
            certfile=certfile,
            keyfile=keyfile,
            password=password
        )
        self._server.start()
        self._client = Client(
            uri=f'{host}:{port}',
            secure=secure,
            cafile=certfile
        )

    def request(self, method: str, params: Any = None) -> Any:
        return self._client.request(method, params)

    def check_client_has_method(self, method: str) -> bool:
        return hasattr(self._client, method.replace('-', '_'))
