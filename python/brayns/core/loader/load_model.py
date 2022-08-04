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

from brayns.core.loader.loader import Loader
from brayns.core.model.deserialize_model import deserialize_model
from brayns.core.model.model import Model
from brayns.instance.instance import Instance


def load_model(instance: Instance, loader: Loader, path: str) -> list[Model]:
    params = _serialize_loader(loader, path)
    result = instance.request('add-model', params)
    return [
        deserialize_model(model)
        for model in result
    ]


def _serialize_loader(loader: Loader, path: str) -> dict[str, Any]:
    return {
        'path': path,
        'loader_name': loader.name,
        'loader_properties': loader.properties
    }
