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

from dataclasses import dataclass
from typing import Any, ClassVar, TypeVar

from ...core import Camera
from ...utils import Bounds, Fovy, View

T = TypeVar('T', bound='CylindricCamera')


@dataclass
class CylindricCamera(Camera):
    """Cylindric camera used to correct curved screen distorsion.

    :param fovy: Field of view, defaults to OpenDeck's.
    :type fovy: Fovy, optional
    """

    OPENDECK_FOVY: ClassVar[Fovy] = Fovy(48.549, degrees=True)

    fovy: Fovy = OPENDECK_FOVY

    @classmethod
    @property
    def name(cls) -> str:
        """Get camera name.

        :return: Camera name.
        :rtype: str
        """
        return 'cylindric'

    @classmethod
    def from_target(cls: type[T], _: Bounds) -> T:
        """Return default constructed camera.

        :return: Perspective camera.
        :rtype: T
        """
        return cls()

    @classmethod
    def deserialize(cls: type[T], message: dict[str, Any]) -> T:
        """Low level API to deserialize from JSON."""
        return cls(
            fovy=Fovy(message['fovy'], degrees=True)
        )

    def get_front_view(self, target: Bounds) -> View:
        """Use fovy to compute the front view.

        :param target: Camera target.
        :type target: Bounds
        :return: Front view based on self.fovy.
        :rtype: View
        """
        return self.fovy.get_front_view(target)

    def serialize(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        message = {}
        if self.fovy is not None:
            message['fovy'] = self.fovy.degrees
        return message
