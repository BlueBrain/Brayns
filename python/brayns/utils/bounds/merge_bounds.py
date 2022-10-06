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

from ..vector import componentwise_max, componentwise_min
from .bounds import Bounds


def merge_bounds(values: list[Bounds]) -> Bounds:
    """Compute the union of all given bounds.

    Returns Bounds.empty if values are empty.

    Assume that all bounds are valid (ie min <= max for each component).

    :param values: Bounds to merge.
    :type values: list[Bounds]
    :return: Union of all bounds in values.
    :rtype: Bounds
    """
    return Bounds(
        min=componentwise_min([value.min for value in values]),
        max=componentwise_max([value.max for value in values]),
    )
