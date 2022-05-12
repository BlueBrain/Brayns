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

from brayns.core.common.quaternion import Quaternion
from brayns.core.common.transform import Transform
from brayns.core.common.vector3 import Vector3


class MockTransform:

    @classmethod
    @property
    def transform(cls) -> Transform:
        return Transform(
            translation=Vector3(1, 2, 3),
            rotation=Quaternion(1, 2, 3, 4),
            scale=Vector3(4, 5, 6)
        )

    @classmethod
    @property
    def serialized_transform(cls) -> dict:
        return {
            'translation': [1, 2, 3],
            'rotation': [1, 2, 3, 4],
            'scale': [4, 5, 6]
        }
