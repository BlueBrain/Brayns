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

from __future__ import annotations

import copy
from dataclasses import dataclass, field

from brayns.network import Instance
from brayns.utils import Rotation, Vector3

from .projection import (
    PerspectiveProjection,
    Projection,
    get_camera_projection,
    set_camera_projection,
)
from .view import View, get_camera_view, set_camera_view


@dataclass
class Camera:
    """Camera used to render.

    A camera is composed of a ``View`` and a ``Projection``.
    """

    view: View = field(default_factory=lambda: View.front)
    projection: Projection = field(default_factory=PerspectiveProjection)

    @property
    def name(self) -> str:
        """Get the camera name (can be compared with ``get_camera_name``).

        :return: Camera projection name.
        :rtype: str
        """
        return self.projection.name

    @property
    def position(self) -> Vector3:
        """Get camera position.

        :return: Camera position.
        :rtype: Vector3
        """
        return self.view.position

    @position.setter
    def position(self, value: Vector3) -> None:
        """Set camera position.

        :param value: New camera position.
        :type value: Vector3
        """
        self.view.position = value

    @property
    def target(self) -> Vector3:
        """Get camera target.

        :return: Camera target.
        :rtype: Vector3
        """
        return self.view.target

    @target.setter
    def target(self, value: Vector3) -> None:
        """Set camera target.

        :param value: New camera target.
        :type value: Vector3
        """
        self.view.target = value

    @property
    def up(self) -> Vector3:
        """Get camera up direction.

        :return: Camera up.
        :rtype: Vector3
        """
        return self.view.up

    @up.setter
    def up(self, value: Vector3) -> None:
        """Set camera up direction.

        :param value: New camera up.
        :type value: Vector3
        """
        self.view.up = value

    @property
    def vector(self) -> Vector3:
        """Get camera vector (target - position).

        :return: Camera vector.
        :rtype: Vector3
        """
        return self.view.vector

    @property
    def direction(self) -> Vector3:
        """Get camera direction (vector.normalized).

        :return: Normalized camera direction.
        :rtype: Vector3
        """
        return self.view.direction

    @property
    def right(self) -> Vector3:
        """Get the camera right direction (direction x up).

        :return: Camera right direction.
        :rtype: Vector3
        """
        return self.view.right

    @property
    def real_up(self) -> Vector3:
        """Get the camera effective up direction (right x direction).

        :return: Camera up direction.
        :rtype: Vector3
        """
        return self.view.real_up

    @property
    def distance(self) -> float:
        """Get distance between camera position and target.

        :return: Camera distance.
        :rtype: float
        """
        return self.view.distance

    @distance.setter
    def distance(self, value: float) -> None:
        """Move the camera position to be at given distance from target.

        Camera direction remains unchanged.

        :param value: New camera distance.
        :type value: float
        """
        self.view.distance = value

    @property
    def orientation(self) -> Rotation:
        """Get camera orientation compared to the front one.

        :return: Camera orientation.
        :rtype: Rotation
        """
        return self.view.orientation

    def translate(self, translation: Vector3) -> Camera:
        """Translate both camera position and target.

        :param translation: Translation to apply.
        :type translation: Vector3
        :return: New translated camera.
        :rtype: Camera
        """
        return Camera(
            view=self.view.translate(translation),
            projection=copy.deepcopy(self.projection),
        )

    def rotate_around_target(self, rotation: Rotation) -> Camera:
        """Rotate camera view around its target.

        :param rotation: Rotation to apply on camera view.
        :type rotation: Rotation
        :return: New rotated camera.
        :rtype: Camera
        """
        return Camera(
            view=self.view.rotate_around_target(rotation),
            projection=copy.deepcopy(self.projection),
        )


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


def set_camera(instance: Instance, camera: Camera) -> None:
    """Shortcut to update the camera view and projection.

    :param instance: Instance.
    :type instance: Instance
    :param camera: Current camera to use for instance.
    :type camera: Camera
    """
    set_camera_view(instance, camera.view)
    set_camera_projection(instance, camera.projection)
