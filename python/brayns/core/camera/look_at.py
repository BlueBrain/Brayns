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

from brayns.utils import Bounds, Rotation, Vector3, View

from .camera import Camera
from .perspective_projection import PerspectiveProjection
from .projection import Projection


def look_at(
    target: Bounds,
    aspect_ratio: float = 1.0,
    rotation: Rotation = Rotation.identity,
    projection: Projection | None = None
) -> Camera:
    """Create a camera to look at given target.

    If no aspect ratio is given (1), then only the target height is taken into
    account for the distance computation.

    :param target: Camera target bounds.
    :type target: Bounds
    :param aspect_ratio: Viewport aspect ratio (width / height), defaults to 1.
    :type aspect_ratio: float
    :param rotation: Camera rotation, defaults to identity.
    :type rotation: Rotation, optional
    :param projection: Camera projection, defaults to PerspectiveProjection.
    :type projection: Projection | None, optional
    :return: Camera looking at target.
    :rtype: Camera
    """
    if projection is None:
        projection = PerspectiveProjection()
    target = _get_apparent_target(target, rotation.inverse)
    view = _get_front_view(target, aspect_ratio, projection)
    view.rotate_around_target(rotation)
    return Camera(view, projection)


def _get_apparent_target(target: Bounds, rotation: Rotation) -> Bounds:
    center = target.center
    return Bounds(
        min=rotation.apply(target.min, center),
        max=rotation.apply(target.max, center),
    )


def _get_front_view(target: Bounds, aspect_ratio: float, projection: Projection) -> View:
    center = target.center
    width, height, depth = target.size
    height = _get_viewport_height(width, height, aspect_ratio)
    distance = projection.look_at(height) + depth / 2
    return View(
        position=center + distance * Vector3.forward,
        target=center,
    )


def _get_viewport_height(width: float, height: float, aspect_ratio: float) -> float:
    return max(height, width / aspect_ratio)
