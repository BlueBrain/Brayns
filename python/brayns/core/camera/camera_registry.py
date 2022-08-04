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

from typing import Any, TypeVar, cast

from brayns.core.camera.camera import Camera
from brayns.core.camera.camera_handler import CameraHandler

T = TypeVar('T', bound=Camera)


class CameraRegistry:

    def __init__(self) -> None:
        self._handlers = dict[type[T], CameraHandler]()

    def register(self, camera: type[T], handler: CameraHandler) -> None:
        assert camera not in self._handlers
        self._handlers[camera] = handler

    def deserialize(self, camera: type[T], message: dict[str, Any]) -> T:
        handler = self._handlers[type(camera)]
        result = handler.deserialize(message)
        return cast(T, result)

    def serialize(self, camera: Camera) -> dict[str, Any]:
        handler = self._handlers[type(Camera)]
        return handler.serialize(camera)


camera_registry = CameraRegistry()
