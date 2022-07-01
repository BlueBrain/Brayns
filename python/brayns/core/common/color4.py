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

from collections.abc import Iterator
from dataclasses import dataclass, replace

from brayns.core.common.color3 import Color3


@dataclass(frozen=True, order=True)
class Color4(Color3):

    a: float = 1.0

    @staticmethod
    def from_color3(color: Color3, alpha: float = 1.0) -> 'Color4':
        return Color4(color.r, color.g, color.b, alpha)

    def __iter__(self) -> Iterator[float]:
        yield from super().__iter__()
        yield self.a

    @property
    def transparent(self) -> 'Color4':
        return replace(self, a=0.0)

    @property
    def opaque(self) -> 'Color4':
        return replace(self, a=1.0)

    @property
    def without_alpha(self) -> Color3:
        return Color3(self.r, self.g, self.b)
