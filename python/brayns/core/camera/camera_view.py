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

from dataclasses import dataclass

from brayns.core.common.vector3 import Vector3
from brayns.instance.instance import Instance


@dataclass
class CameraView:

    position: Vector3 = Vector3.zero
    target: Vector3 = Vector3.zero
    up: Vector3 = Vector3.up

    @staticmethod
    def from_instance(instance: Instance) -> 'CameraView':
        result = instance.request('get-camera-look-at')
        return CameraView.deserialize(result)

    @staticmethod
    def deserialize(message: dict) -> 'CameraView':
        return CameraView(
            position=Vector3(*message['position']),
            target=Vector3(*message['target']),
            up=Vector3(*message['up'])
        )

    def use_for_main_camera(self, instance: Instance) -> None:
        params = self.serialize()
        instance.request('set-camera-look-at', params)

    def serialize(self) -> dict:
        return {
            'position': list(self.position),
            'target': list(self.target),
            'up': list(self.up)
        }
