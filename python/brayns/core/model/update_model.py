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

from typing import Any

from brayns.network import Instance
from brayns.utils import Transform, serialize_transform

from .deserialize_model import deserialize_model
from .model import Model


def update_model(
    instance: Instance,
    model_id: int,
    transform: Transform | None = None,
    visible: bool | None = None,
) -> Model:
    """Modify the properties of a given model and return its updated version.

    Parameters left as None will remain in their current state.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: ID of the model to update.
    :type model_id: int
    :param transform: Model transformation, defaults to None
    :type transform: Transform | None, optional
    :param visible: Model visibility, defaults to None
    :type visible: bool | None, optional
    :return: Updated model.
    :rtype: Model
    """
    properties = dict[str, Any]()
    if transform is not None:
        properties['transform'] = serialize_transform(transform)
    if visible is not None:
        properties['is_visible'] = visible
    params = {
        'model_id': model_id,
        'model': properties,
    }
    result = instance.request('update-model', params)
    return deserialize_model(result)
