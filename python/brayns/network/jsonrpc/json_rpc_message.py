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
from typing import Any, Protocol, TypeVar

T = TypeVar('T', bound='JsonRpcMessage')


class JsonRpcMessage(Protocol):

    @classmethod
    def from_json(cls: type[T], data: str) -> T:
        obj = json.loads(data)
        return cls.from_dict(obj)

    @classmethod
    def from_dict(cls: type[T], obj: dict[str, Any]) -> T:
        message = cls()
        message.update(obj)
        return message

    def to_json(self) -> str:
        obj = self.to_dict()
        return json.dumps(obj)

    def to_dict(self) -> dict[str, Any]:
        raise NotImplementedError()

    def update(self, obj: dict[str, Any]) -> None:
        raise NotImplementedError()
