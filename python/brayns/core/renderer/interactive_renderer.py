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
from typing import Any, TypeVar

from .renderer import Renderer

T = TypeVar('T', bound='InteractiveRenderer')


@dataclass
class InteractiveRenderer(Renderer):
    """Default renderer used for fast rendering (streaming, tests).

    :param enable_shadows: Enable shadows, defaults to True.
    :type enable_shadows: bool, optional.
    :param ambient_occlusion_samples: AO samples, defaults to 0.
    :type ambient_occlusion_samples: int, optional.
    """

    enable_shadows: bool = True
    ambient_occlusion_samples: int = 0

    @classmethod
    @property
    def name(cls) -> str:
        """Get renderer name.

        :return: Renderer name.
        :rtype: str
        """
        return 'interactive'

    @classmethod
    def deserialize(cls: type[T], message: dict[str, Any]) -> T:
        """Low level API to deserialize from JSON."""
        return cls.deserialize_with(
            message,
            enable_shadows=message['enable_shadows'],
            ambient_occlusion_samples=message['ao_samples']
        )

    @property
    def additional_properties(self) -> dict[str, Any]:
        """Low level API to serialize to JSON."""
        return {
            'enable_shadows': self.enable_shadows,
            'ao_samples': self.ambient_occlusion_samples
        }
