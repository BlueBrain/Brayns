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

from .color_ramp import ColorRamp
from .control_point import ControlPoint
from .deserialize_color_ramp import deserialize_color_ramp
from .get_color_ramp import get_color_ramp
from .opacity_curve import OpacityCurve
from .serialize_color_ramp import serialize_color_ramp
from .set_color_ramp import set_color_ramp
from .value_range import ValueRange

__all__ = [
    'ColorRamp',
    'ControlPoint',
    'deserialize_color_ramp',
    'get_color_ramp',
    'OpacityCurve',
    'serialize_color_ramp',
    'set_color_ramp',
    'ValueRange',
]
