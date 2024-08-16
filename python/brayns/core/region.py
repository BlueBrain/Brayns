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

from brayns.network import Instance
from brayns.utils import Vector2


def get_camera_region(instance: Instance) -> tuple[Vector2, Vector2]:
    result = instance.request("get-camera-region")
    return (Vector2(*result["image_start"]), Vector2(*result["image_end"]))


def set_camera_region(instance: Instance, start: Vector2, end: Vector2) -> None:
    params = {"image_start": list(start), "image_end": list(end)}
    instance.request("set-camera-region", params)
