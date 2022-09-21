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
from .get_camera_projection import get_camera_projection
from .get_camera_view import get_camera_view
from .projection import Projection


def get_camera(instance: Instance, projection_type: type[Projection]) -> Camera:
    """Shortcut to retreive the current camera of an instance.

    Use get_camera_projection and get_camera_view.

    projection_type must be of the same type as the current projection.

    :param instance: Instance.
    :type instance: Instance
    :param projection_type: Current camera projection type of instance.
    :type projection_type: type[Projection]
    :return: Camera with current view and projection.
    :rtype: Camera
    """
    view = get_camera_view(instance)
    projection = get_camera_projection(instance, projection_type)
    return Camera(view, projection)
