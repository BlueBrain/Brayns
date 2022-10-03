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
from typing import Any

import brayns


class MockReply:

    @classmethod
    @property
    def reply(cls) -> brayns.JsonRpcReply:
        return brayns.JsonRpcReply(
            id=0,
            result=123,
        )

    @classmethod
    @property
    def binary_reply(cls) -> brayns.JsonRpcReply:
        reply = cls.reply
        reply.binary = b'123'
        return reply

    @classmethod
    @property
    def message(cls) -> dict[str, Any]:
        return {
            'jsonrpc': '2.0',
            'id': 0,
            'result': 123,
        }

    @classmethod
    @property
    def text(cls) -> str:
        return json.dumps(cls.message, sort_keys=True)

    @classmethod
    @property
    def binary(cls) -> bytes:
        text = cls.text.encode('utf-8')
        size = len(text).to_bytes(4, byteorder='little', signed=False)
        binary = cls.binary_reply.binary
        return b''.join([size, text, binary])
