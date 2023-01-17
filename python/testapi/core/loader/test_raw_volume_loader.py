# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

import brayns
from testapi.simple_test_case import SimpleTestCase


class TestRawVolumeLoader(SimpleTestCase):
    def test_load_model(self) -> None:
        path = self.folder / "chest_ct_scan.raw"
        dimensions = brayns.Vector3(256, 256, 112)
        spacing = brayns.Vector3(1.16, 1.16, 2.5)
        data_type = brayns.VolumeDataType.FLOAT
        loader = brayns.RawVolumeLoader(dimensions, spacing, data_type)
        models = loader.load_models(self.instance, str(path))
        self.assertEqual(len(models), 1)
        model = models[0]

        min_bound = brayns.Vector3.zero
        max_bound = brayns.Vector3(296.96, 296.96, 280)
        ref = brayns.Bounds(min_bound, max_bound)
        self.assertEqual(model.bounds, ref)
