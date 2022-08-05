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

from brayns.core.model.model import Model
from brayns.core.transform.transform import Transform
from brayns.instance.instance import Instance


def update_model(
    instance: Instance,
    model_id: int,
    visible: bool | None = None,
    transform: Transform | None = None,
) -> Model:
    properties = {}
    if visible is not None:
        properties['is_visible'] = visible
    if transform is not None:
        properties['transform'] = transform.serialize()
    params = {
        'model_id': model_id,
        'model': properties,
    }
    result = instance.request('update-model', params)
    return Model.deserialize(result)
