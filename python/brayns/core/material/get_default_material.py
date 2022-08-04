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

from brayns.core.material.default_material import DefaultMaterial
from brayns.core.material.get_material import get_material
from brayns.instance.instance import Instance


def get_default_material(instance: Instance, model_id: int) -> DefaultMaterial:
    return get_material(
        instance=instance,
        model_id=model_id,
        material=DefaultMaterial,
        deserializer=_deserialize_default_material
    )


def _deserialize_default_material(message: dict[str, Any]) -> DefaultMaterial:
    return DefaultMaterial(
        opacity=message['opacity']
    )
