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

from brayns.core.material.get_material import get_material
from brayns.core.material.matte_material import MatteMaterial
from brayns.instance.instance import Instance


def get_matte_material(instance: Instance, model_id: int) -> MatteMaterial:
    return get_material(
        instance=instance,
        model_id=model_id,
        material=MatteMaterial,
        deserializer=_deserialize_matte_material
    )


def _deserialize_matte_material(message: dict[str, Any]) -> MatteMaterial:
    return MatteMaterial(
        opacity=message['opacity']
    )
