# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

from collections.abc import Callable
from dataclasses import dataclass

from brayns.utils import Axis, Bounds, CameraRotation, Rotation, Vector3

from .camera import Camera
from .projection import PerspectiveProjection, Projection
from .view import View


@dataclass
class CameraController:
    """Helper to create a camera to look at a given target.

    Aspect ratio is usually the one of the resolution of the final image.

    Camera translation and rotation can be used to adjust the camera manually.
    They are applied in this order relatively to the front view (use X to move
    right, Y to move up and Z to move further from the target).

    A new projection is created for each camera. It can be configured using the
    ``projection`` parameter, which is a callable returning a new projection.

    A camera using the current settings can be built at any time using the
    ``camera`` property.

    :param target: Camera target bounds.
    :type target: Bounds
    :param aspect_ratio: Viewport aspect ratio (width / height), defaults to 1.
    :type aspect_ratio: float
    :param translation: Camera translation (before rotation), defaults to zero.
    :type translation: Vector3, optional
    :param rotation: Camera rotation, defaults to identity.
    :type rotation: Rotation, optional
    :param projection: Projection factory, defaults to PerspectiveProjection.
    :type projection: Projection, optional
    """

    target: Bounds
    aspect_ratio: float = 1.0
    translation: Vector3 = Vector3.zero
    rotation: Rotation = CameraRotation.front
    projection: Callable[[], Projection] = PerspectiveProjection

    @property
    def camera(self) -> Camera:
        """Create a new camera using current controller settings.

        :return: Camera focusing on controller target.
        :rtype: Camera
        """
        projection = self.projection()
        target = _get_apparent_target(self.target, self.rotation.inverse)
        view = _get_front_view(target, self.aspect_ratio, projection)
        view = view.translate(self.translation)
        view = view.rotate_around_target(self.rotation)
        return Camera(view, projection)


def _get_apparent_target(target: Bounds, rotation: Rotation) -> Bounds:
    return target.rotate(rotation, target.center)


def _get_front_view(
    target: Bounds, aspect_ratio: float, projection: Projection
) -> View:
    distance = _get_camera_distance(target.size, aspect_ratio, projection)
    center = target.center
    return View(
        position=center + distance * Axis.front,
        target=center,
        up=Axis.up,
    )


def _get_camera_distance(
    target_size: Vector3, aspect_ratio: float, projection: Projection
) -> float:
    width, height, depth = target_size
    height = max(height, width / aspect_ratio)
    return projection.look_at(height) + depth / 2
