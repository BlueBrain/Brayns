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

from typing import Any

from brayns.core.camera.perspective_camera import PerspectiveCamera
from brayns.core.view.fovy import Fovy
from brayns.instance.instance import Instance


def get_perspective_camera(instance: Instance) -> PerspectiveCamera:
    name = PerspectiveCamera.name
    result = instance.request(f'get-camera-{name}')
    return _deserialize_perspective_camera(result)


def _deserialize_perspective_camera(message: dict[str, Any]) -> PerspectiveCamera:
    return PerspectiveCamera(
        fovy=Fovy(message['fovy'], degrees=True),
        aperture_radius=message['aperture_radius'],
        focus_distance=message['focus_distance'],
    )
