from typing import Any

import abc


class Client(abc.ABC):

    @abc.abstractmethod
    def get(method: str, params: Any) -> Any:
        pass


def get_result(client: Client, method: str, params: dict) -> Any:
    args = {
        key: value
        for key, value in params.items()
        if key != 'self' and value is not None
    }
    if not args:
        return client.get(method, None)
    return client.get(method, args)
