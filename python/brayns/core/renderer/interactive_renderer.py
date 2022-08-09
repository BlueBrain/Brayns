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

from brayns.core.renderer.renderer import Renderer

T = TypeVar('T', bound='InteractiveRenderer')


@dataclass
class InteractiveRenderer(Renderer):

    enable_shadows: bool = True
    ambient_occlusion_samples: int = 0

    @classmethod
    @property
    def name(cls) -> str:
        return 'interactive'

    @classmethod
    def deserialize(cls: type[T], message: dict[str, Any]) -> T:
        return cls.deserialize_with(
            message,
            enable_shadows=message['enable_shadows'],
            ambient_occlusion_samples=message['ao_samples']
        )

    @property
    def additional_properties(self) -> dict[str, Any]:
        return {
            'enable_shadows': self.enable_shadows,
            'ao_samples': self.ambient_occlusion_samples
        }
