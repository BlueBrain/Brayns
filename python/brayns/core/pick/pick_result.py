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
from typing import Any

from brayns.utils import Vector3


@dataclass
class PickResult:
    """Information about the model found at an inspected screen position.

    Metadata depend on the kind of model found at given position and are
    specific to the primitive (subpart) hitted.

    :param position: World position matching screen coordinates.
    :type position: Vector3
    :param model_id: ID of them model at screen position.
    :type model_id: int
    :param metadata: Information about the model primitive at given position.
    :type metadata: Any
    """

    position: Vector3
    model_id: int
    metadata: Any
