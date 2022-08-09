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

from brayns.core.vector.vector3 import Vector3
from brayns.core.view.view import View


class MockView:

    @classmethod
    @property
    def view(cls) -> View:
        return View(
            position=Vector3(1, 2, 3),
            target=Vector3(4, 5, 6),
            up=Vector3(7, 8, 9)
        )

    @classmethod
    @property
    def message(cls) -> dict[str, Any]:
        return {
            'position': [1, 2, 3],
            'target': [4, 5, 6],
            'up': [7, 8, 9]
        }