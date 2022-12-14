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


class MockTransform:
    @classmethod
    @property
    def rotation(cls) -> brayns.Rotation:
        quaternion = brayns.Quaternion.identity
        return brayns.Rotation.from_quaternion(quaternion)

    @classmethod
    @property
    def transform(cls) -> brayns.Transform:
        return brayns.Transform(
            translation=brayns.Vector3(0, 1, 2),
            rotation=cls.rotation,
            scale=brayns.Vector3(7, 8, 9),
        )

    @classmethod
    @property
    def message(cls) -> dict[str, Any]:
        return {
            "translation": [0, 1, 2],
            "rotation": list(cls.rotation.quaternion),
            "scale": [7, 8, 9],
        }
