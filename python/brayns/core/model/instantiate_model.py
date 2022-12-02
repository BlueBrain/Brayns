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

from __future__ import annotations

from typing import Any

from brayns.network import Instance
from brayns.utils import Transform, serialize_transform

from .deserialize_model import deserialize_model
from .model import Model


def instantiate_model(
    instance: Instance,
    model_id: int,
    transform: Transform | None = None
) -> Model:
    """Creates an instance of the given model and returns it.

    :param instance: Instance.
    :type instance: Instance
    :param model_id: ID of the model to instantiate.
    :type model_id: int
    :param transform: New instance transformation, defaults to None
    :type transform: Transform | None, optional
    :return: New instance model.
    :rtype: Model
    """

    params: dict[str, Any] = {
        'model_id': model_id,
    }

    if transform is not None:
        params['transform'] = serialize_transform(transform)

    result = instance.request('instantiate-model', params)
    return deserialize_model(result)
