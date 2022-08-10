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
from dataclasses import dataclass
from typing import Any

from brayns.core.color import Color3


@dataclass
class Light(ABC):
    """Base class for all light types.

    :param color: Light color, defaults to white.
    :type color: Color3, optional
    :param intensity: Light intensity, defaults to 1.
    :type intensity: float, optional
    :param visible: Make the light visible or not, defaults to True.
    :type visible: bool, optional
    """

    color: Color3 = Color3.white
    intensity: float = 1.0
    visible: bool = True

    @classmethod
    @property
    @abstractmethod
    def name(cls) -> str:
        """Get the light name.

        :return: Light name.
        :rtype: str
        """
        pass

    @property
    @abstractmethod
    def additional_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        pass

    @property
    def base_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            'color': list(self.color),
            'intensity': self.intensity,
            'visible': self.visible,
        }

    def serialize(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return self.base_properties | self.additional_properties
