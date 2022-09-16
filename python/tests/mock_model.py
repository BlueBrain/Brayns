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


class MockModel:

    @classmethod
    @property
    def model(cls) -> brayns.Model:
        quaternion = brayns.Quaternion(3, 4, 5, 6)
        return brayns.Model(
            id=0,
            bounds=brayns.Bounds(
                min=brayns.Vector3.zero,
                max=brayns.Vector3.one,
            ),
            metadata={'test': '1'},
            visible=True,
            transform=brayns.Transform(
                translation=brayns.Vector3(0, 1, 2),
                rotation=brayns.Rotation.from_quaternion(quaternion),
                scale=brayns.Vector3(7, 8, 9),
            ),
        )

    @classmethod
    @property
    def message(cls) -> dict[str, Any]:
        return {
            'model_id': 0,
            'bounds': MockBounds.message,
            'metadata': {'test': '1'},
            'is_visible': True,
            'transform': MockTransform.message
        }
