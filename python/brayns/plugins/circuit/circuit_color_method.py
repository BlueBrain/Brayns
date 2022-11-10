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

from typing import ClassVar


class CircuitColorMethod:
    """Available coloring methods for circuits."""

    ID: ClassVar[str] = 'id'
    ETYPE: ClassVar[str] = 'etype'
    MTYPE: ClassVar[str] = 'mtype'
    LAYER: ClassVar[str] = 'layer'
    REGION: ClassVar[str] = 'region'
    HEMISPHERE: ClassVar[str] = 'hemisphere'
    MORPHOLOGY: ClassVar[str] = 'morphology'
    MORPHOLOGY_CLASS: ClassVar[str] = 'morphology class'
    MORPHOLOGY_SECTION: ClassVar[str] = 'morphology section'
    SYNAPSE_CLASS: ClassVar[str] = 'synapse class'
