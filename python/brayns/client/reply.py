# Copyright 2015-2024 Blue Brain Project/EPFL
# All rights reserved. Do not distribute without permission.
#
# Responsible Author: adrien.fleury@epfl.ch
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 3.0 as published
# by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

import json
from typing import Any, Union

from .reply_error import ReplyError


class Reply:

    @staticmethod
    def from_json(data: str):
        message = None
        try:
            message = json.loads(data)
        except Exception as e:
            raise ReplyError(0, f'JSON syntax error: {str(e)}')
        if not isinstance(message, dict):
            raise ReplyError(0, 'Not a JSON object')
        return Reply.from_dict(message)

    @staticmethod
    def from_dict(message: dict):
        return Reply(
            request_id=message.get('id'),
            params=message.get('params'),
            result=message.get('result'),
            error=message.get('error'),
            jsonrpc=message.get('jsonrpc')
        )

    @staticmethod
    def from_error_message(message: dict):
        return Reply(
            error=ReplyError(0, message)
        )

    def __init__(
        self,
        jsonrpc: str = '2.0',
        request_id: Union[int, str, None] = None,
        params: Any = None,
        result: Any = None,
        error: Union[ReplyError, None] = None,
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
            raise ReplyError.from_dict(self.error)
        return self.result
