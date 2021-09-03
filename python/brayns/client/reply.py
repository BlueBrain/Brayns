from typing import Any, Type, Union

import json


class ParsingError(Exception):
    pass


class Error(Exception):

    def __init__(
        self,
        code: int,
        message: str,
        data: Any = None
    ) -> None:
        self.code = code
        self.message = message
        self.data = data

    def __str__(self) -> str:
        return self.message


def load_error(error: dict) -> Error:
    return Error(
        code=error.get('code'),
        message=error.get('message'),
        data=error.get('data')
    )


class Message:

    def __init__(
        self,
        jsonrpc: str = '2.0',
        request_id: Union[None, int, str] = None,
        params: Any = None,
        result: Any = None,
        error: Union[None, Error] = None,
    ) -> None:
        self.jsonrpc = jsonrpc
        self.request_id = request_id
        self.params = params
        self.result = result
        self.error = error

    def is_notification(self) -> bool:
        return self.request_id is None

    def is_error(self) -> bool:
        return self.error is not None

    def get_result(self) -> Any:
        if self.is_error():
            raise load_error(self.error)
        return self.result


def load_message(message: dict) -> Message:
    return Message(
        request_id=message.get('id'),
        params=message.get('params'),
        result=message.get('result'),
        error=message.get('error'),
        jsonrpc=message.get('jsonrpc')
    )


def from_json(data: str) -> Message:
    message = None
    try:
        message = json.loads(data)
    except Exception as e:
        raise ParsingError(f'JSON syntax error: {str(e)}')
    if not isinstance(message, dict):
        raise ParsingError('Not a JSON object')
    return load_message(message)
