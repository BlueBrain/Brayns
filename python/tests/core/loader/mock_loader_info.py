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

from brayns.core.api.json_schema import JsonSchema
from brayns.core.api.json_type import JsonType
from brayns.core.image.resolution import Resolution
from brayns.core.loader.loader_info import LoaderInfo


class MockLoaderInfo:

    @classmethod
    @property
    def loader_info(cls) -> LoaderInfo:
        return LoaderInfo(
            name='test',
            extensions=['test1', 'test2'],
            schema=JsonSchema(type=JsonType.INTEGER)
        )

    @classmethod
    @property
    def message(cls) -> dict[str, Any]:
        return {
            'name': 'test',
            'extensions': ['test1', 'test2'],
            'input_parameters_schema': {
                'type': 'integer'
            }
        }
