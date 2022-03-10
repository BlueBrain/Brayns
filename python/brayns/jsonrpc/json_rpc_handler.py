# Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

from .json_rpc_reply import JsonRpcReply
from .json_rpc_error import JsonRpcError
from .json_rpc_progress import JsonRpcProgress
from .json_rpc_listener import JsonRpcListener


def handle_text(data: str, listener: JsonRpcListener) -> None:
    try:
        _handle_text(data, listener)
    except Exception as e:
        listener.on_invalid_message(data, e)


def _handle_text(data: str, listener: JsonRpcListener) -> None:
    message = _parse(data)
    if _handle_error(message, listener):
        return
    if _handle_progress(message, listener):
        return
    if _handle_reply(message, listener):
        return
    raise ValueError('Message is not valid JSON-RPC')


def _parse(data: str) -> dict:
    message = json.loads(data)
    if not isinstance(message, dict):
        raise ValueError('Message is not a JSON object')
    return message


def _handle_error(message: dict, listener: JsonRpcListener) -> bool:
    if 'error' not in message:
        return False
    listener.on_error(JsonRpcError.from_dict(message))
    return True


def _handle_progress(message: dict, listener: JsonRpcListener) -> bool:
    if 'id' in message:
        return False
    listener.on_progress(JsonRpcProgress.from_dict(message))
    return True


def _handle_reply(message: dict, listener: JsonRpcListener) -> bool:
    if 'result' not in message:
        return False
    listener.on_reply(JsonRpcReply.from_dict(message))
    return True
