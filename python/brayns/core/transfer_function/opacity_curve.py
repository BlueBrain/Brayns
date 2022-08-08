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

from brayns.core.color.color3 import Color3
from brayns.core.color.color4 import Color4
from brayns.core.transfer_function.control_point import ControlPoint


@dataclass
class OpacityCurve:

    control_points: list[ControlPoint]

    def apply(self, colors: list[Color3]) -> list[Color4]:
        result = []
        color_count = len(colors)
        for i in range(color_count):
            normalized_value = _normalize_index(i, color_count)
            opacity = _get_opacity(self.control_points, normalized_value)
            color = Color4.from_color3(colors[i], opacity)
            result.append(color)
        return result


def _get_opacity(control_points: list[ControlPoint], normalized_value: float) -> float:
    lower_index = _find_lower_index(control_points, normalized_value)
    upper_index = lower_index + 1
    lower_control_point = _find(control_points, lower_index)
    upper_control_point = _find(control_points, upper_index)
    lower = lower_control_point.normalized_value
    upper = upper_control_point.normalized_value
    lower_factor = _get_lower_factor(normalized_value, lower, upper)
    upper_factor = 1 - lower_factor
    lower_opacity = lower_control_point.opacity
    upper_opacity = upper_control_point.opacity
    return lower_opacity * lower_factor + upper_opacity * upper_factor


def _find_lower_index(control_points: list[ControlPoint], normalized_value: float) -> int:
    for i, control_point in enumerate(control_points):
        if control_point.normalized_value >= normalized_value:
            return i - 1
    return len(control_points) - 1


def _find(control_points: list[ControlPoint], index: int) -> ControlPoint:
    if index < 0:
        return ControlPoint.start
    if index >= len(control_points):
        return ControlPoint.end
    return control_points[index]


def _normalize_index(index: int, size: int) -> float:
    return index / max(size - 1, 1)


def _get_lower_factor(value: float, lower: float, upper: float) -> float:
    if lower == upper:
        return 0.0
    size = upper - lower
    gap = upper - value
    return gap / size
