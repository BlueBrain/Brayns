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

from typing import Any

from brayns.core.geometry.add_geometries import add_geometries
from brayns.core.geometry.capsule import Capsule
from brayns.core.model.model import Model
from brayns.instance.instance import Instance


def add_capsules(instance: Instance, capsules: list[Capsule]) -> Model:
    return add_geometries(instance, 'add-capsules', capsules, _serialize_capsule)


def _serialize_capsule(capsule: Capsule) -> dict[str, Any]:
    return {
        'p0': list(capsule.start_point),
        'r0': capsule.start_radius,
        'p1': list(capsule.end_point),
        'r1': capsule.end_radius,
    }
