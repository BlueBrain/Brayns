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
from typing import Any

from brayns.network import Instance
from brayns.utils import Color4, Vector


class ValueRange(Vector[float]):
    """Specify color ramp value range.

    Usually the simulation values are voltages but it can be anything.

    :param min: Minimal simulation value.
    :type min: float
    :param max: Maximal simulation value.
    :type max: float
    """

    def __new__(cls, min: float, max: float) -> ValueRange:
        return super().__new__(cls, min, max)

    @property
    def min(self) -> float:
        return self[0]

    @property
    def max(self) -> float:
        return self[1]

    @property
    def size(self) -> float:
        """Get the difference between min and max.

        :return: Value range.
        :rtype: float
        """
        return self.max - self.min

    def normalize(self, value: float) -> float:
        """Normalize given value between min and max.

        Result is not clamped between min and max.

        For example a value of 5 with min = 0 and max = 10 gives 0.5.

        :param value: Simulation value.
        :type value: float
        :return: Normalized simulation value.
        :rtype: float
        """
        return (value - self.min) / self.size


@dataclass
class ColorRamp:
    """Color ramp to map simulation values to colors.

    Simulation values below value_range.min have colors[0].

    Simulation values above value_range.max have colors[-1].

    Otherwise, the two closest colors in the list are interpolated linearly.

    :param value_range: Simulation value range (usually voltages).
    :type value_range: ValueRange
    :param colors: List of colors mapped to value range.
    :type colors: list[Color4]
    """

    value_range: ValueRange
    colors: list[Color4]


def get_color_ramp(instance: Instance, model_id: int) -> ColorRamp:
    """Retreive the color ramp of the given model.

    Model must have a color ramp attached (usually report loaded).

    :param instance: Instance.
    :type instance: Instance
    :param model_id: Model ID.
    :type model_id: int
    :return: Color ramp.
    :rtype: ColorRamp
    """
    params = {"id": model_id}
    result = instance.request("get-color-ramp", params)
    return _deserialize_color_ramp(result)


def set_color_ramp(instance: Instance, model_id: int, ramp: ColorRamp) -> None:
    """Set the current color ramp of the given model.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: Model ID.
    :type model_id: int
    :param color_ramp: Color ramp.
    :type color_ramp: ColorRamp
    """
    params = {
        "id": model_id,
        "color_ramp": _serialize_color_ramp(ramp),
    }
    instance.request("set-color-ramp", params)


def _deserialize_color_ramp(message: dict[str, Any]) -> ColorRamp:
    return ColorRamp(
        value_range=ValueRange(*message["range"]),
        colors=[Color4(*color) for color in message["colors"]],
    )


def _serialize_color_ramp(ramp: ColorRamp) -> dict[str, Any]:
    return {
        "range": list(ramp.value_range),
        "colors": [list(color) for color in ramp.colors],
    }
