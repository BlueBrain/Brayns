# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

from typing import Any

import brayns


class MockInstance(brayns.Instance):
    def __init__(self, result: Any = None, binary: bytes = b"") -> None:
        self._result = result
        self._binary = binary
        self.method = ""
        self.params: Any = None
        self.binary = b""

    def send(self, request: brayns.JsonRpcRequest) -> brayns.JsonRpcFuture:
        self.method = request.method
        self.params = request.params
        self.binary = request.binary
        reply = brayns.JsonRpcReply(
            id=request.id,
            result=self._result,
            binary=self._binary,
        )
        return brayns.JsonRpcFuture.from_reply(reply)
