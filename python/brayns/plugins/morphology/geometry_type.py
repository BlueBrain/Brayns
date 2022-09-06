# Copyright (c) 2015-2022 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

from enum import Enum


class GeometryType(Enum):
    """Type of geometry to use when loading a morphology.

    :param ORIGINAL: Use raw geometries dimensions.
    :param SMOOTH: Smooth radius changes between geometries for better visual.
    :param SECTION_SMOOTH: Smooth radius change between whole sections.
    :param CONSTANT_RADII: Apply the same radius to all geometries.
    """

    ORIGINAL = 'original'
    SMOOTH = 'smooth'
    SECTION_SMOOTH = 'section_smooth'
    CONSTANT_RADII = 'constant_radii'
