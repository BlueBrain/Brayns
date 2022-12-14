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

import brayns
from testapi.image_validator import ImageValidator
from testapi.simple_test_case import SimpleTestCase


class TestSetResolution(SimpleTestCase):
    def test_set_resolution(self) -> None:
        brayns.set_resolution(self.instance, brayns.Resolution.ultra_hd)
        test = brayns.get_application(self.instance)
        self.assertEqual(test.resolution, brayns.Resolution.ultra_hd)

    def test_render(self) -> None:
        brayns.set_resolution(self.instance, brayns.Resolution.full_hd)
        self.add_sphere()
        ref = self.folder / "full_hd.png"
        snapshot = brayns.Snapshot()
        data = snapshot.download(self.instance)
        validator = ImageValidator()
        validator.validate_data(data, ref)
