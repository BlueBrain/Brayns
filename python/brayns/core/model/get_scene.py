# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

from brayns.network import Instance
from brayns.utils import deserialize_bounds

from .deserialize_model import deserialize_model
from .scene import Scene


def get_scene(instance: Instance) -> Scene:
    """Retreive all models and the world boundaries from an instance.

    :param instance: Instance.
    :type instance: Instance
    :return: Models and global bounds.
    :rtype: Scene
    """
    result = instance.request("get-scene")
    return _deserialize_scene(result)


def _deserialize_scene(message: dict[str, Any]) -> Scene:
    return Scene(
        bounds=deserialize_bounds(message["bounds"]),
        models=[deserialize_model(model) for model in message["models"]],
    )
