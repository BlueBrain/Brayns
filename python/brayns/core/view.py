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

from dataclasses import dataclass
from typing import Any

from brayns.network import Instance
from brayns.utils import Axis, Rotation, Vector3


@dataclass
class View:
    """Represent a viewpoint with position and target.

    Up and direction doesn't need to be perpendicular. However, having direction
    equal to zero or colinear with up will make an invalid view.

    :param position: Observation position.
    :type position: Vector3
    :param target: Target position.
    :type target: Vector3
    :param up: Up vector, defaults to Axis.up.
    :type up: Vector3, optional
    """

    position: Vector3
    target: Vector3
    up: Vector3 = Axis.up

    @classmethod
    @property
    def front(cls) -> View:
        """Front view with X right, Y up and Z front.

        :return: Front view.
        :rtype: View
        """
        return View(
            position=Vector3.zero,
            target=Axis.back,
        )

    @property
    def vector(self) -> Vector3:
        """Get view vector (target - position).

        :return: View vector.
        :rtype: Vector3
        """
        return self.target - self.position

    @property
    def direction(self) -> Vector3:
        """Get normalized view vector.

        :return: View vector normalized.
        :rtype: Vector3
        """
        return self.vector.normalized

    @property
    def right(self) -> Vector3:
        """Get right direction relative to view (direction x up).

        :return: Right direction normalized.
        :rtype: Vector3
        """
        return self.direction.cross(self.up).normalized

    @property
    def real_up(self) -> Vector3:
        """Get effective up direction (right x direction).

        Equal to up if up is perpendicular to direction and normalized.

        :return: Real up direction normalized.
        :rtype: Vector3
        """
        return self.right.cross(self.direction)

    @property
    def distance(self) -> float:
        """Get the distance between the observator and the target.

        :return: View vector norm.
        :rtype: float
        """
        return self.vector.norm

    @distance.setter
    def distance(self, value: float) -> None:
        """Move the observer position to be at given distance from target.

        View direction remains unchanged.

        :param value: New observer distance.
        :type value: float
        """
        self.position = self.target - value * self.direction

    @property
    def orientation(self) -> Rotation:
        """Get orientation compared to the front view.

        :return: Rotation from front view to self.
        :rtype: Rotation
        """
        return self.get_orientation(View.front)

    def get_orientation(self, reference: View) -> Rotation:
        """Get orientation from given reference.

        The rotation obtained goes from reference.direction and
        reference.real_up to self.direction and self.real_up.

        :return: Rotation from given view to self.
        :rtype: Rotation
        """
        first = Rotation.between(reference.direction, self.direction)
        up = first.apply(reference.real_up)
        second = Rotation.between(up, self.real_up)
        return first.then(second)

    def translate(self, translation: Vector3) -> View:
        """Translate position and target in given direction.

        :param translation: Translation to apply on position and target.
        :type translation: Vector3
        :return: New translated view.
        :rtype: View
        """
        return View(
            position=self.position + translation,
            target=self.target + translation,
            up=self.up,
        )

    def rotate_around_target(self, rotation: Rotation) -> View:
        """Rotate observation position around target.

        :param rotation: Rotation to apply on observer.
        :type rotation: Rotation
        :return: New rotated view.
        :rtype: View
        """
        return View(
            position=rotation.apply(self.position, center=self.target),
            target=self.target,
            up=rotation.apply(self.up),
        )


def get_camera_view(instance: Instance) -> View:
    """Get the current camera view of an instance.

    :param instance: Instance.
    :type instance: Instance
    :return: Camera view.
    :rtype: View
    """
    result = instance.request("get-camera-view")
    return deserialize_view(result)


def set_camera_view(instance: Instance, view: View) -> None:
    """Set the current camera view of an instance.

    :param instance: Instance.
    :type instance: Instance
    :param view: New camera view.
    :type view: View
    """
    params = serialize_view(view)
    instance.request("set-camera-view", params)


def deserialize_view(obj: dict[str, Any]) -> View:
    return View(
        position=Vector3(*obj["position"]),
        target=Vector3(*obj["target"]),
        up=Vector3(*obj["up"]),
    )


def serialize_view(view: View) -> dict[str, Any]:
    return {
        "position": list(view.position),
        "target": list(view.target),
        "up": list(view.up),
    }
