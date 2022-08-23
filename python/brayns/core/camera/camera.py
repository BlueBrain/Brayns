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

from abc import ABC, abstractmethod
from typing import Any, TypeVar

from ...utils import Bounds, View

T = TypeVar('T', bound='Camera')


class Camera(ABC):
    """Base class of all supported cameras (plugin dependent).

    All cameras defined in the package inherit from this class.

    Cameras can be identified using a unique name (ex: 'perspective').
    """

    @classmethod
    @property
    @abstractmethod
    def name(cls) -> str:
        """Name of the camera to identify it.

        :return: Camera name.
        :rtype: str
        """
        pass

    @classmethod
    @abstractmethod
    def from_target(cls: type[T], target: Bounds) -> T:
        """Create a camera to see entirely the given target.

        Default values are used for parameters independent from the target.

        :param target: Camera target.
        :type target: Bounds
        :return: Camera instance.
        :rtype: T
        """
        pass

    @classmethod
    @abstractmethod
    def deserialize(cls: type[T], message: dict[str, Any]) -> T:
        """Low level API to deserialize from JSON."""
        pass

    @abstractmethod
    def get_front_view(self, target: Bounds) -> View:
        """Compute front view for the given target.

        :param target: Camera target.
        :type target: Bounds
        :return: Front view to see the target entirely.
        :rtype: View
        """
        pass

    @abstractmethod
    def serialize(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        pass

    def serialize_with_name(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            'name': self.name,
            'params': self.serialize()
        }
