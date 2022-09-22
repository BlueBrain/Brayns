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

from brayns.network import Instance

from .camera import Camera
from .set_camera_projection import set_camera_projection
from .set_camera_view import set_camera_view


def set_camera(instance: Instance, camera: Camera) -> None:
    """Shortcut to update the camera view and projection.

    :param instance: Instance.
    :type instance: Instance
    :param camera: Current camera to use for instance.
    :type camera: Camera
    """
    set_camera_view(instance, camera.view)
    set_camera_projection(instance, camera.projection)
