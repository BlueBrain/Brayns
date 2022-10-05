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

from brayns.utils import Bounds, View

T = TypeVar('T', bound='Projection')


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
    def look_at(self, target: Bounds, aspect_ratio: float) -> View:
        """Return the front view to focus on given target.

        Update projection properties to look a target and returns the
        corresponding front view.

        :param target: Camera target.
        :type target: Bounds
        :param aspect_ratio: Viewport aspect ratio.
        :type aspect_ratio: float
        :return: Front view to see the target entirely.
        :rtype: View
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
            'name': self.name,
            'params': self.get_properties(),
        }
