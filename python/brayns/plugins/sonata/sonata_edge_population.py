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

from dataclasses import dataclass


@dataclass
class SonataEdgePopulation:
    """SONATA edge population parameters.

    :param name: Population name.
    :type name: str
    :param afferent: If these edges are afferent or efferent ones.
    :type afferent: bool
    :param afferent: If these edges are afferent ones.
    :type afferent: bool
    :param density: Proportion of edges to load, defaults to 1.
    :type density: float, optional
    :param radius: Edge radii, defaults to 2.
    :type radius: float, optional
    :param report: Report name, defaults to None.
    :type report: str | None, optional
    """

    name: str
    afferent: bool
    density: float = 1.0
    radius: float = 2.0
    report: str | None = None
