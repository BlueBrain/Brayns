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

from dataclasses import dataclass, field
from typing import Any

import brayns


@dataclass
class MockInstance(brayns.Instance):

    reply: Any = None
    method: str = field(default='', init=False)
    params: Any = field(default=None, init=False)
    binary: bytes = field(default=b'', init=False)

    def send(self, request: brayns.Request) -> brayns.RequestFuture:
        self.method = request.method
        self.params = request.params
        self.binary = request.binary
        return brayns.RequestFuture.from_result(self.reply)
