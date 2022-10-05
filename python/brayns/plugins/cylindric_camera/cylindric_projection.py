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
from typing import Any, ClassVar

from brayns.core import Projection
from brayns.utils import Bounds, Fovy, View


@dataclass
class CylindricProjection(Projection):
    """Cylindric camera projection used to correct curved screen distorsion.

    :param fovy: Field of view, defaults to OpenDeck one.
    :type fovy: Fovy, optional
    """

    OPENDECK_FOVY: ClassVar[Fovy] = Fovy(48.549, degrees=True)

    fovy: Fovy = OPENDECK_FOVY

    @classmethod
    @property
    def name(cls) -> str:
        """Get projection name.

        :return: Projection name.
        :rtype: str
        """
        return 'cylindric'

    def look_at(self, target: Bounds, aspect_ratio: float) -> View:
        """Compute front view using field of view.

        :param target: Camera target.
        :type target: Bounds
        :param aspect_ratio: Viewport aspect ratio.
        :type aspect_ratio: float
        :return: Front view to see the target entirely.
        :rtype: View
        """
        return self.fovy.look_at(target, aspect_ratio)

    def get_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            'fovy': self.fovy.degrees,
        }

    def update_properties(self, message: dict[str, Any]) -> None:
        """Low level API to deserialize from JSON."""
        self.fovy = Fovy(message['fovy'], degrees=True)
