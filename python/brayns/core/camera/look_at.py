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

from __future__ import annotations

from brayns.utils import Bounds

from .camera import Camera
from .perspective_projection import PerspectiveProjection
from .projection import Projection


def look_at(target: Bounds, projection: Projection | None = None) -> Camera:
    """Shortcut to create a camera focusing on given target.

    See ``Camera.look_at`` for more details.

    :param target: Camera target bounds.
    :type target: Bounds
    :param projection: Camera projection, defaults to PerspectiveProjection.
    :type projection: Projection | None, optional
    :return: Camera looking at target.
    :rtype: Camera
    """
    if projection is None:
        projection = PerspectiveProjection()
    camera = Camera(projection=projection)
    camera.look_at(target)
    return camera
