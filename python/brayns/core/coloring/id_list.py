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

from __future__ import annotations

from collections.abc import Iterable
from dataclasses import dataclass


@dataclass
class IdList:
    """Compressed list of IDs in a string.

    IDs are comma-separated and consecutive IDs are represented as a range
    with start and end separated by a dash.

    Examples: '1,3,5-9,11'.

    Some factory methods are provided to help formatting.
    """

    value: str

    @staticmethod
    def from_integers(values: Iterable[int]) -> IdList:
        """Create an ID list from integer values."""
        return IdList.from_strings(str(value) for value in values)

    @staticmethod
    def from_strings(values: Iterable[str]) -> IdList:
        """Create an ID list from string values."""
        return IdList(','.join(values))

    @staticmethod
    def from_range(start: int, end: int) -> IdList:
        """Create an ID list from a range of values."""
        return IdList(f'{start}-{end}')

    def __or__(self, other: IdList) -> IdList:
        """Combine two lists of IDs."""
        return IdList.from_strings([self.value, other.value])
