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

from brayns.utils import Color3, Color4


@dataclass
class ControlPoint:
    """Control point used for opacity curves.

    Normalized values are simulation values (usually voltages) normalized
    between the min and max of the color ramp value range.

    :param normalized_value: Simulation value, normalized.
    :type normalized_value: float
    :param opacity: Color opacity at simulation value.
    :type opacity: float
    """

    normalized_value: float
    opacity: float

    @classmethod
    @property
    def start(cls) -> ControlPoint:
        """Implicit first control point of the opacity curve [0, 0].

        :return: Control point.
        :rtype: ControlPoint
        """
        return ControlPoint(0.0, 0.0)

    @classmethod
    @property
    def end(cls) -> ControlPoint:
        """Implicit last control point of the opacity curve [1, 1].

        :return: Control point.
        :rtype: ControlPoint
        """
        return ControlPoint(1.0, 1.0)


@dataclass
class OpacityCurve:
    """Opacity curve to generate opacity from a list of color3.

    Use control points to map simulation value (usually voltage) to opacity.

    The simulation values are normalized so it is not necessary to know them.

    If no control points are set at 0 or 1, implicit control points [0, 0] and
    [1, 1] will be used for the computations. Otherwise the user-defined ones
    are used.

    :param control_points: Control points of the curve.
    :type control_points: list[ControlPoint]
    """

    control_points: list[ControlPoint]

    def apply(self, colors: list[Color3]) -> list[Color4]:
        """Create a from colors with opacities computed from the curve.

        Works also with a list of Color4 as lists are covariant in Python.

        Simulation normalized value range is inferred from len(colors).

        :param colors: Input colors.
        :type colors: list[Color3]
        :return: Output colors.
        :rtype: list[Color4]
        """
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
    lower_ratio = _get_lower_ratio(normalized_value, lower, upper)
    upper_ratio = 1 - lower_ratio
    lower_opacity = lower_control_point.opacity
    upper_opacity = upper_control_point.opacity
    return lower_opacity * lower_ratio + upper_opacity * upper_ratio


def _find_lower_index(
    control_points: list[ControlPoint], normalized_value: float
) -> int:
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


def _get_lower_ratio(value: float, lower: float, upper: float) -> float:
    if lower == upper:
        return 0.0
    gap = upper - value
    size = upper - lower
    return gap / size
