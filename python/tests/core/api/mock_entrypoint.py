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

import brayns


class MockEntrypoint:
    @classmethod
    @property
    def entrypoint(cls) -> brayns.Entrypoint:
        return brayns.Entrypoint(
            method="test1",
            description="test2",
            plugin="test3",
            asynchronous=True,
            params=brayns.JsonSchema(type=brayns.JsonType.OBJECT),
            result=brayns.JsonSchema(type=brayns.JsonType.ARRAY),
        )

    @classmethod
    @property
    def message(cls) -> dict[str, Any]:
        return {
            "title": "test1",
            "description": "test2",
            "plugin": "test3",
            "async": True,
            "params": {"type": "object"},
            "returns": {"type": "array"},
        }
