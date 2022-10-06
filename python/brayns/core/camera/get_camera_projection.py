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

from typing import TypeVar

from brayns.network import Instance

from .projection import Projection

T = TypeVar('T', bound=Projection)


def get_camera_projection(instance: Instance, projection_type: type[T]) -> T:
    """Retreive the current camera projection from an instance.

    The provided projection type must be the same as the current one.

    Returned projection is of type ``projection_type``.

    :param instance: Instance.
    :type instance: Instance
    :param projection_type: Projection type (ex: brayns.PerspectiveProjection).
    :type projection_type: type[T]
    :return: Current camera of ``instance``.
    :rtype: T
    """
    name = projection_type.name
    result = instance.request(f'get-camera-{name}')
    return projection_type.from_properties(result)
