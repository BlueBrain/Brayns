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

from brayns.core.model.model import Model
from tests.core.common.mock_bounds import MockBounds
from tests.core.common.mock_transform import MockTransform


class MockModel:

    @classmethod
    @property
    def model(cls) -> Model:
        return Model(
            id=0,
            bounds=MockBounds.bounds,
            metadata={'test': 1},
            visible=True,
            transform=MockTransform.transform
        )

    @classmethod
    @property
    def serialized_model(cls) -> dict:
        return {
            'model_id': 0,
            'bounds': MockBounds.serialized_bounds,
            'metadata': {'test': 1},
            'is_visible': True,
            'transformation': MockTransform.serialized_transform
        }
