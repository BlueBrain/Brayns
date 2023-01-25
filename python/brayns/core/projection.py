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

import math
from abc import ABC, abstractmethod
from dataclasses import dataclass
from typing import Any, TypeVar

from brayns.network import Instance

T = TypeVar("T", bound="Projection")


class Projection(ABC):
    """Base class of all supported camera projections (plugin dependent).

    All camera projections defined in the package inherit from this class.

    Projections can be identified using a unique name (ex: 'perspective').
    """

    @classmethod
    @property
    @abstractmethod
    def name(cls) -> str:
        """Name of the projection to identify it.

        :return: Camera name.
        :rtype: str
        """
        pass

    @abstractmethod
    def look_at(self, height: float) -> float:
        """Focus on a target with given height.

        Update projection properties to look at a target of given height and
        return the minimal distance to see it entirely.

        :param height: Target height.
        :type target: float
        :return: Distance to see target entirely.
        :rtype: float
        """
        pass

    @abstractmethod
    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        pass

    @abstractmethod
    def update_properties(self, message: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        pass

    @classmethod
    def from_properties(cls: type[T], message: dict[str, Any]) -> T:
        """Low level API to deserialize from JSON."""
        projection = cls()
        projection.update_properties(message)
        return projection

    def get_properties_with_name(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "name": self.name,
            "params": self.get_properties(),
        }


@dataclass
class OrthographicProjection(Projection):
    """Orthographic camera projection.

    Orthographic camera makes all objects having the same size regardless their
    distance from the camera.

    The viewport width is computed using the aspect ratio of the current
    resolution of the instance (framebuffer size).

    :param height: Viewport height in world coordinates.
    :type height: float
    """

    height: float = 0.0

    @classmethod
    @property
    def name(cls) -> str:
        """Projection name.

        :return: Projection name.
        :rtype: str
        """
        return "orthographic"

    def look_at(self, height: float) -> float:
        """Update viewport height to given one.

        Camera distance doesn't matter in orthographic projections.

        :param height: Target height.
        :type target: float
        :return: Distance to see target entirely.
        :rtype: float
        """
        self.height = height
        return 0

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {"height": self.height}

    def update_properties(self, message: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        self.height = message["height"]


class Fovy:
    """Field of view (angle) of a camera."""

    def __init__(self, angle: float, degrees: bool = False) -> None:
        """Construct with angle.

        :param angle: Angle value.
        :type angle: float
        :param degrees: True if angle is in degrees, defaults to False
        :type degrees: bool, optional
        """
        self._angle = math.radians(angle) if degrees else angle

    def __eq__(self, other: object) -> bool:
        """Comparison operator.

        :param other: Object to compare.
        :type other: object
        :return: True if other is Fovy and has same value.
        :rtype: bool
        """
        if not isinstance(other, Fovy):
            return False
        return self._angle == other._angle

    @property
    def radians(self) -> float:
        """Get angle in radians.

        :return: Angle in radians.
        :rtype: float
        """
        return self._angle

    @property
    def degrees(self) -> float:
        """Get angle in degrees.

        :return: Angle in degrees.
        :rtype: float
        """
        return math.degrees(self._angle)

    def get_distance(self, height: float) -> float:
        """Compute the distance to have a viewport with given height.

        :param height: Viewport height.
        :type height: float
        :return: Distance to have the given viewport height.
        :rtype: float
        """
        return height / 2 / math.tan(self.radians / 2)


@dataclass
class PerspectiveProjection(Projection):
    """Perspective camera projection.

    Perspective projection use a field of view angle to compute the size of the
    objects depending on their distance from the camera.

    The field of view (fovy) can be used to compute full screen view of a given
    target object.

    :param fovy: Field of view angle (45 degrees by default).
    :type fovy: Fovy
    :param aperture_radius: Optional aperture radius.
    :type aperture_radius: float
    :param focus_distance: Optional focus distance.
    :type focus_distance: float
    """

    fovy: Fovy = Fovy(45, degrees=True)
    aperture_radius: float = 0.0
    focus_distance: float = 1.0

    @classmethod
    @property
    def name(cls) -> str:
        """Camera name.

        :return: Camera name.
        :rtype: str
        """
        return "perspective"

    def look_at(self, height: float) -> float:
        """Compute camera distance using field of view.

        :param height: Target height.
        :type target: float
        :return: Distance to see target entirely.
        :rtype: float
        """
        return self.fovy.get_distance(height)

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            "fovy": self.fovy.degrees,
            "aperture_radius": self.aperture_radius,
            "focus_distance": self.focus_distance,
        }

    def update_properties(self, message: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        self.fovy = Fovy(message["fovy"], degrees=True)
        self.aperture_radius = message["aperture_radius"]
        self.focus_distance = message["focus_distance"]


def get_camera_name(instance: Instance) -> str:
    """Retreive the name of the current camera of an instance.

    A camera name is the name of the projection it uses.

    The returned name is the same as Camera.name and can be used to check
    if a given camera is the current one like this:

    :param instance: Instance.
    :type instance: Instance
    :return: Current camera name.
    :rtype: str
    """
    return instance.request("get-camera-type")


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
    result = instance.request(f"get-camera-{name}")
    return projection_type.from_properties(result)


def set_camera_projection(instance: Instance, camera: Projection) -> None:
    """Set the current camera of the given instance.

    :param instance: Instance.
    :type instance: Instance
    :param camera: Current camera.
    :type camera: Camera
    """
    name = camera.name
    params = camera.get_properties()
    instance.request(f"set-camera-{name}", params)
