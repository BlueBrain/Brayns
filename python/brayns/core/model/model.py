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

from brayns.utils import Bounds, Transform


@dataclass
class Model:
    """Loaded model.

    All models are loaded without transform (identity) but it doesn't mean that
    their center of mass is at the origin (depends on the source file).

    Model metadata are a str -> str map and depends on the model type.

    :param id: Model ID.
    :type id: int
    :param type: Model type.
    :type type: str
    :param bounds: Model bounding box.
    :type bounds: Bounds
    :param info: Intrinsic nodel information.
    :type info: dict[str, Any]
    :param visible: Check wether the model is rendered or not.
    :type visible: bool
    :param transform: Model transform relative to the origin.
    :type transform: Transform
    """

    id: int
    type: str
    bounds: Bounds
    info: dict[str, Any]
    visible: bool
    transform: Transform
