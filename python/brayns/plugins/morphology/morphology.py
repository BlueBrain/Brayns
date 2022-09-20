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

from dataclasses import dataclass

from .geometry_type import GeometryType


@dataclass
class Morphology:
    """Describe how to load a morphology.

    :param radius_multiplier: Radius scaling, defaults to 1.
    :type radius_multiplier: float, optional
    :param load_soma: Wether to load somas, defaults to True.
    :type load_soma: bool, optional
    :param load_axon: Wether to load axons, defaults to False.
    :type load_axon: bool, optional
    :param load_dendrites: Wether to load dendrites, defaults to False.
    :type load_dendrites: bool, optional
    :param geometry_type: How to load geometries, defaults to smooth.
    :type geometry_type: GeometryType, optional
    """

    radius_multiplier: float = 1.0
    load_soma: bool = True
    load_axon: bool = False
    load_dendrites: bool = False
    geometry_type: GeometryType = GeometryType.SMOOTH
