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

from brayns.utils import Axis, Bounds, Rotation, Vector3, View

from .camera import Camera
from .camera_rotation import CameraRotation
from .perspective_projection import PerspectiveProjection
from .projection import Projection


def look_at(
    target: Bounds,
    aspect_ratio: float = 1.0,
    translation: Vector3 = Vector3.zero,
    rotation: Rotation = CameraRotation.front,
    projection: Projection | None = None
) -> Camera:
    """Create a camera to look at given target.

    Aspect ratio is usually the one of the resolution of the final image.

    Camera translation and rotation can be used to adjust the camera manually.
    They are applied in this order relatively to the front view (use X to move
    right, Y to move up and Z to move further from the target).

    :param target: Camera target bounds.
    :type target: Bounds
    :param aspect_ratio: Viewport aspect ratio (width / height), defaults to 1.
    :type aspect_ratio: float
    :param translation: Camera translation (before rotation), defaults to zero.
    :type translation: Vector3, optional
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
    view.position += translation
    view = view.rotate_around_target(rotation)
    return Camera(view, projection)


def _get_apparent_target(target: Bounds, rotation: Rotation) -> Bounds:
    return target.rotate(rotation, target.center)


def _get_front_view(target: Bounds, aspect_ratio: float, projection: Projection) -> View:
    center = target.center
    distance = _get_camera_distance(target.size, aspect_ratio, projection)
    return View(
        position=center + distance * Axis.front,
        target=center,
    )


def _get_camera_distance(target_size: Vector3, aspect_ratio: float, projection: Projection) -> float:
    width, height, depth = target_size
    height = max(height, width / aspect_ratio)
    return projection.look_at(height) + depth / 2
